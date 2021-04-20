#include "TaskList.h"
#include "DDS.h"
#include "MonitorTask.h"
#include "STM_GPIO_CONFIG.h"


// Initalize LL task Lists

DD_TaskList_t taskList_ACTIVE;
DD_TaskList_t taskList_OVERDUE;
DD_TaskList_t taskList_COMPLETED;


// Helper Fucntions

uint32_t create_dd_task( DD_TaskHandle_t);
uint32_t release_dd_task( DD_TaskHandle_t);

uint32_t delete_dd_task( DD_TaskHandle_t);
uint32_t complete_dd_task(pTaskHandle_t pTask);


// Initalize Global task handle for DDS

pTaskHandle_t TaskHandle = NULL;




/*
 * DDS initalization task
 *
 *
 */

void DDS_Init(void) {

	debugPrint("Init Task lists:");

	debugPrint("\n");

	Init_DD_TaskList(&taskList_ACTIVE);
	Init_DD_TaskList(&taskList_OVERDUE);
	Init_DD_TaskList(&taskList_COMPLETED);

	// create tasks for DDS and Monitor Functionality

# if MONITOR_MODE == 1

	xTaskCreate(Monitor_Task, "Monitor Task", configMINIMAL_STACK_SIZE, NULL,
	PriorityLevel_MONITOR, NULL);

#endif

	xTaskCreate(DDS_Task, "DDS Task", configMINIMAL_STACK_SIZE, NULL,
	PriorityLevel_SCHEDULER, NULL);

}






/*
 *DDS TASK:
 *
 *- Recieves Queue Messages from xDDS_Msg_Queueswithces
 * - Controls scheduling through toggling Task_State and using helper functions to sort tasks based on EDF
 * 	and classification state
 */

void DDS_Task(void *pvParameters) {

	debugPrint("\n");
	debugPrint("Starting Scheduler:");
	debugPrint("\n");
	debugPrint("\n");

	DD_Message_t msg;

	pTaskHandle_t recievedTask = NULL;

	while (1) {

		// Recieve DDS MSG QUEUE

		if ( xQueueReceive(xDDS_Msg_Queue, (void* )&msg,
				portMAX_DELAY) == pdTRUE) {

			recievedTask = (pTaskHandle_t) msg.data;

			if (recievedTask != NULL) {

				// sorts the overdue nodes from active nodes
				Sort_Overdue_From_Active(&taskList_ACTIVE, &taskList_OVERDUE);

				if (msg.type & Msg_Create_DDT) {

					recievedTask->task_state = CreateState;

					// check if task already exits

					if (!(bool) msg.taskExists) {

						// if not, add to ACTIVE LIST

						Insert_DDT_to_LL(recievedTask, &taskList_ACTIVE);

					}
					else{
						TickType_t deadline = recievedTask->absolute_deadline;

						recievedTask->absolute_deadline =  xTaskGetTickCount() + deadline;
					}

				}

				else if (msg.type & Msg_Release_DDT) { // request to release created/period of exiting task

					// Toggle Active state on task

					recievedTask->task_state = ActiveState;

				}

				else if (msg.type & Msg_Delete_DDT) {

					// Switch task state to delete

					recievedTask->task_state = DeleteState;

					// remove task from active list

					remove_DDT_From_LL(&taskList_ACTIVE, recievedTask);

				}

				else if (msg.type & Msg_Complete_DDT) {

					recievedTask->task_state = CompleteState;

					// add task to completed LL
					add_DDT_to_Completed(recievedTask);

				}

			}

			if (msg.type & Msg_ActiveList) {

				msg.pList = &taskList_ACTIVE;

				if ( xQueueSend(xMonitor_Msg_Queue, &msg,
						(TickType_t) portMAX_DELAY) != pdPASS) {

					break;
				}

			}

			else if (msg.type & Msg_OverDueList) {

				msg.pList = &taskList_OVERDUE;

				if ( xQueueSend(xMonitor_Msg_Queue, &msg,
						(TickType_t) portMAX_DELAY) != pdPASS) {

					break;
				}

			}

			else if (msg.type & Msg_CompleteList) {

				msg.pList = &taskList_COMPLETED;

				if ( xQueueSend(xMonitor_Msg_Queue, &msg,
						(TickType_t) portMAX_DELAY) != pdPASS) {

					break;
				}

			}

		}
	}

}




/*
 * HELPER FUCNTION:
 *
 * Checks Task list for exitance of specific task
 * Returns:  BOOL
 *
 *
 *
 */

//
//bool task_exists_in_List(pTaskListHandle_t taskList,
//		pTaskHandle_t task) {
//
//	if (taskList == NULL || task == NULL) {
//
//		return 0;
//
//	}
//
//	pTaskHandle_t LL_index = taskList->head;
//
//	while (LL_index != NULL) {
//
//		if (LL_index->task_handle == task->task_handle) {
//
//			return true;
//		}
//
//		LL_index = LL_index->next;
//
//	}
//
//	return false;
//
//}
pTaskHandle_t task_exists_in_List(pTaskListHandle_t taskList,
		pTaskHandle_t task) {

	if (taskList == NULL || task == NULL) {

		return 0;

	}

	pTaskHandle_t LL_index = taskList->head;

	while (LL_index != NULL) {

		if (LL_index->task_handle == task->task_handle) {

			return LL_index;
		}

		LL_index = LL_index->next;

	}

	return 0;

}


/*
 * This function receives all of the information necessary to create a new
 * dd_task struct (excluding the release time and completion time).
 * The struct is packaged as a message and sent to a queue for the DDS to receive.
 *
 *
 *
 */
//
//uint32_t create_dd_task(pTaskHandle_t createTask) {
//
//	if (createTask == NULL) {
//
//		printf(
//				"ERROR: Request to create task with null pointer to task handle\n");
//		return 0;
//
//	}
//
//
//
//	// Functional block to check if task exits before creating a new one ----------
//
//	bool task_exists = false;
//
//
//
//
//	if (task_exists_in_List(&taskList_ACTIVE, createTask)) {
//
//
//		task_exists = true;
//
//	}
//
//	else if (task_exists_in_List(&taskList_OVERDUE, createTask)) {
//
//		// Move to active list
//
//		Insert_DDT_to_LL(createTask, &taskList_ACTIVE);
//
//		// Remove from completed list
//
//		remove_DDT_From_LL(&taskList_OVERDUE, createTask);
//
//		task_exists = true;
//
//	}
//
//	else if (task_exists_in_List(&taskList_COMPLETED,createTask)) {
//
//
//		// Move to active list
//		Insert_DDT_to_LL(createTask, &taskList_ACTIVE);
//
//		// Remove from completed list -- free node
//
//		remove_DDT_From_LL(&taskList_COMPLETED, createTask);
//
//		task_exists = true;
//
//	}
//
//	else {
//
//
//		xTaskCreate(createTask->task_function, createTask->task_name,
//		configMINIMAL_STACK_SIZE, (void*) createTask,
//		PriorityLevel_LOW, &(createTask->task_handle));
//
//	}
//
//
//	// ------------------------------------------------------
//
//
//
//	vTaskSuspend(createTask->task_handle); // suspend indefinetly
//
//
//	// boot stap for task message
//
//	DD_Message_t create_task_msg = { Msg_Create_DDT,
//									createTask->task_handle,
//									createTask,
//									NULL,
//									task_exists };
//
//	debugPrint("\n");
//
//	debugPrint("[%s] Released | Time [%u]", createTask->task_name,
//			xTaskGetTickCount());
//
//	debugPrint("\n");
//
//
//	// Send created message
//	if ( xQueueSend(xDDS_Msg_Queue, &create_task_msg,
//			portMAX_DELAY) != pdPASS) // ensure the message was sent
//	{
//		printf(
//				"\nTask [%s]  Unable to Release \n unable to send New Task Message sent to DDS Queue \n",
//				createTask->task_name);
//		return 0;
//	}
//
//	// release task message
//
//	release_dd_task(createTask);
//
//	return 1;
//
//}
//

uint32_t create_dd_task(pTaskHandle_t newTask) {

	if (newTask == NULL) {

		printf(
				"ERROR: Request to create task with null pointer to task handle\n");
		return 0;

	}

	// Check if DDT already exists in Active, Completed and overdue lists
	pTaskHandle_t taskExists_ACTIVE = task_exists_in_List(&taskList_ACTIVE,
			newTask);

	pTaskHandle_t taskExists_OVERDUE = task_exists_in_List(&taskList_OVERDUE,
			newTask);

	pTaskHandle_t taskExists_COMPLETED = task_exists_in_List(
			&taskList_COMPLETED, newTask);

	pTaskHandle_t createTask = NULL;

	bool task_exists = false;

	if (taskExists_ACTIVE != NULL) {

		createTask = taskExists_ACTIVE;

		task_exists = true;

	}

	else if (taskExists_OVERDUE != NULL) {

		//demote_priorities(&taskList_OVERDUE);
		createTask = taskExists_OVERDUE;

		// Move to active list
		Insert_DDT_to_LL(createTask, &taskList_ACTIVE);

		// Remove from completed list -- free node

		remove_DDT_From_LL(&taskList_OVERDUE, createTask, true);

		task_exists = true;

	}

	else if (taskExists_COMPLETED != NULL) {

		createTask = taskExists_COMPLETED;

		// Move to active list
		Insert_DDT_to_LL(createTask, &taskList_ACTIVE);

		// Remove from completed list -- free node

		remove_DDT_From_LL(&taskList_COMPLETED, createTask, true);

		task_exists = true;

	}

	else {

		createTask = newTask;

		xTaskCreate(createTask->task_function, createTask->task_name,
		configMINIMAL_STACK_SIZE, (void*) createTask,
		PriorityLevel_LOW, &(createTask->task_handle));

	}

	vTaskSuspend(createTask->task_handle);

	DD_Message_t create_task_msg = { Msg_Create_DDT, createTask->task_handle,
			createTask,
			NULL, task_exists };

	debugPrint("\n");

	debugPrint("[%s] Released | Time [%u]", createTask->task_name,
			xTaskGetTickCount());

	debugPrint("\n");

	if ( xQueueSend(xDDS_Msg_Queue, &create_task_msg,
			portMAX_DELAY) != pdPASS) // ensure the message was sent
	{
		printf(
				"\nTask [%s]  Unable to Release \n unable to send New Task Message sent to DDS Queue \n",
				createTask->task_name);
		return 0;
	}

	release_dd_task(createTask);

	return 1;

}





uint32_t release_dd_task(pTaskHandle_t pTask) {

	if (pTask == NULL) {

		printf(
				"ERROR: Request to create task with null pointer to task handle\n");
		return 0;

	}

	DD_Message_t release_task_msg = { Msg_Release_DDT, pTask->task_handle,
			pTask,
			NULL };

	if ( xQueueSend(xDDS_Msg_Queue, &release_task_msg,
			portMAX_DELAY) != pdPASS) // ensure the message was sent
	{
		printf(
				"\nTask [%s]  Unable to Release \n unable to send New Task Message sent to DDS Queue \n",
				pTask->task_name);
		return 0;
	}

	vTaskResume(pTask->task_handle);

}



/*
 * HELPER FUNCTION:
 *
 * Sends message from task after completion cycle to DDS
 * DDS changes state from ACTIVE to COMPLETED,
 * Upon confirmation helper function will suspend task
 *
 *
 */

uint32_t complete_dd_task(pTaskHandle_t pTask) {

	if (pTask == NULL) {

		printf(
				"ERROR: Request to create task with null pointer to task handle\n");
		return 0;

	}

	DD_Message_t msg_Complete_Task = { Msg_Complete_DDT, pTask->task_handle,
			pTask,
			NULL,
			NULL };

	debugPrint("\n");

	debugPrint("[%s] Completed | Time [%u]", pTask->task_name,
			(unsigned int) xTaskGetTickCount());

	debugPrint("\n");

	vTaskSuspend(pTask->task_handle);

	if ( xQueueSend(xDDS_Msg_Queue, &msg_Complete_Task,portMAX_DELAY) != pdPASS) {
		printf(
				" ERROR While Sending Delete Msg to DDS Queue:  See DDS.c, Line 172 \n");
		return 0;
	}

	return 1;

}


/*
 * HELPER FUNCTION:
 *
 *
 * Triggered only at the end of exection of an Aperiotic Task
 * Sends message to DDS, DDS toggles state from completed to delete
 * Helper function deletes task
 *
 *
 */

uint32_t delete_dd_task(pTaskHandle_t pTaskToDelete) {

	DD_Message_t msg_Delete_Task = { Msg_Delete_DDT, pTaskToDelete->task_handle,
			pTaskToDelete,
			NULL,
			NULL };

	if ( xQueueSend(xDDS_Msg_Queue, &msg_Delete_Task,portMAX_DELAY) != pdPASS) {
		printf(
				" ERROR While Sending Delete Msg to DDS Queue:  See DDS.c, Line 172 \n");
		return 0;
	}



	// Delete Task handle
	vTaskDelete(pTaskToDelete->task_handle);

	return 1;

}



