#include "TaskList.h"
#include "DDS.h"
#include "MonitorTask.h"

EventGroupHandle_t xTaskStatus;
QueueHandle_t xDDS_Msg_Queue;

pTaskHandle_t TaskHandle = NULL;

uint32_t create_dd_task( DD_TaskHandle_t);
uint32_t release_dd_task( DD_TaskHandle_t);

uint32_t delete_dd_task( DD_TaskHandle_t);
uint32_t complete_dd_task(pTaskHandle_t pTask);

/*
 * Implements the EDF algorithm and controls the
 * priorities of user-defined F-tasks from an actively-managed list of DD-Tasks
 *
 */

void DDS_Task(void *pvParameters) {
	printf(
			"\n\n********************  Starting Scheduler  ****************\n\n");

	DD_Message_t msg;

	pTaskHandle_t recievedTask = NULL;

	uint8_t testCounter = 0;

	while (1) {

		if ( xQueueReceive(xDDS_Msg_Queue, (void* )&msg,
				portMAX_DELAY) == pdTRUE) {
			recievedTask = (pTaskHandle_t) msg.data;

			// Debuging.. Recieving NULL tasks upon second second iteration
			//if (recievedTask->execution_time != NULL) {

			// sorts the overdue nodes from active nodes
			Sort_Overdue_From_Active(&taskList_ACTIVE, &taskList_OVERDUE);

			if (msg.type & Msg_Create_DDT) {

				recievedTask->task_state = CreateState;

				if (!(bool) msg.taskExists) {

					Insert_DDT_to_LL(recievedTask, &taskList_ACTIVE);

				}

//					else {
//
//						release_dd_task(recievedTask);
//
//					}

				//TEST_xTaskNotifyGive(msg.pTask);

			}

			else if (msg.type & Msg_Release_DDT) {

				recievedTask->task_state = ActiveState;

			}

			else if (msg.type & Msg_Delete_DDT) {

				recievedTask->task_state = DeleteState;

				remove_DDT_From_LL(&taskList_ACTIVE, recievedTask, true);

			}

			else if (msg.type & Msg_Complete_DDT) {

				recievedTask->task_state = CompleteState;

				// add task to completed LL
				add_DDT_to_Completed(recievedTask);

				recievedTask->task_state = ActiveState;

				//create_dd_task(recievedTask);

			}

			else if (msg.type & Msg_ActiveList) {

				msg.pList = &taskList_ACTIVE;

				if (uxQueueSpacesAvailable(xMonitor_Msg_Queue) == 0) {
					xQueueReset(xMonitor_Msg_Queue);
				}

				if (xMonitor_Msg_Queue != NULL) {

					if (xMonitor_Msg_Queue != NULL) {
						if ( xQueueSend(xMonitor_Msg_Queue, &msg,
								(TickType_t) portMAX_DELAY) != pdPASS) {

							break;
						}

					}

				}

			}

			else if (msg.type & Msg_OverDueList) {

				msg.pList = &taskList_OVERDUE;

				if (uxQueueSpacesAvailable(xMonitor_Msg_Queue) == 0) {
					xQueueReset(xMonitor_Msg_Queue);
				}

				if (xMonitor_Msg_Queue != NULL) {
					if ( xQueueSend(xMonitor_Msg_Queue, &msg,
							(TickType_t) portMAX_DELAY) != pdPASS) {

						break;
					}
				}

			}

			else if (msg.type & Msg_CompleteList) {

				msg.pList = &taskList_COMPLETED;

				if (uxQueueSpacesAvailable(xMonitor_Msg_Queue) == 0) {
					xQueueReset(xMonitor_Msg_Queue);
				}

				if (xMonitor_Msg_Queue != NULL) {
					if ( xQueueSend(xMonitor_Msg_Queue, &msg,
							(TickType_t) portMAX_DELAY) != pdPASS) {

						break;
					}
				}

			}

			//}

		}
	}

}

/*
 * This function receives all of the information necessary to create a new
 * dd_task struct (excluding the release time and completion time).
 * The struct is packaged as a message and sent to a queue for the DDS to receive.
 *
 *
 *
 */

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

/// Detelete this only used for debugging
//void demote_priorities(pTaskListHandle_t taskList) {
//
//	uint32_t priority_index = uxTaskPriorityGet(taskList->head->task_handle);
//	pTaskHandle_t LL_index = taskList->head;
//	while (LL_index != NULL) {
//
//		vTaskPrioritySet(LL_index->task_handle, priority_index - 1); // demote priority of each task 1
//		priority_index--;
//		LL_index = LL_index->next;
//
//	}
//
//}

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

	//createTask->task_state = ActiveState;

	vTaskSuspend(createTask->task_handle);


	DD_Message_t create_task_msg = { Msg_Create_DDT, createTask->task_handle,
			createTask,
			NULL, task_exists };


	debugPrint("\n\n| Task [%s] Released | current Time [0] | \n\n", createTask->task_name);





	if (uxQueueSpacesAvailable(xDDS_Msg_Queue) == 0) {
		xQueueReset(xDDS_Msg_Queue);
	}

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

//void release_from_taskList(){
//
//	pTaskHandle_t iterator = &taskList
//
//
//
//}

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

	debugPrint("\n\n|Task [%s] Completed | current Time [%u] |\n\n",
			pTask->task_name, (unsigned int) xTaskGetTickCount());

	vTaskSuspend(NULL);

	if ( xQueueSend(xDDS_Msg_Queue, &msg_Complete_Task,portMAX_DELAY) != pdPASS) {
		printf(
				" ERROR While Sending Delete Msg to DDS Queue:  See DDS.c, Line 172 \n");
		return 0;
	}

	return 1;

}

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

	TEST_ulTaskNotifyTake( pdTRUE, portMAX_DELAY ); // ignore

	vTaskDelete(pTaskToDelete->task_handle);

	return 1;

}

void DDS_Init(void) {

	debugPrint("\n\n.............Initializing Task lists............:\n\n");

	Init_DD_TaskList(&taskList_ACTIVE);
	Init_DD_TaskList(&taskList_OVERDUE);
	Init_DD_TaskList(&taskList_COMPLETED);

	// Crate Queue for DDS Communication
	xDDS_Msg_Queue = xQueueCreate(MAX_NUM_DDS_MSGS, sizeof(DD_Message_t));
	vQueueAddToRegistry(xDDS_Msg_Queue, "DDS Queue");

	//Create  Queue for Monitor Messages
	xMonitor_Msg_Queue = xQueueCreate(MAX_NUM_MONITOR_MSGS,
			sizeof(DD_Message_t)); // Should only ever have 2 requests on the queue.
	vQueueAddToRegistry(xMonitor_Msg_Queue, "Monitor Queue");

	// create tasks for DDS and Monitor Functionality
	xTaskCreate(DDS_Task, "DDS Task", configMINIMAL_STACK_SIZE, NULL,
	PrioirtyLevel_MAX, NULL);

# if MONITOR_MODE == 1
	xTaskCreate(MonitorTask, "Monitor Task", configMINIMAL_STACK_SIZE, NULL,
	PriorityLevel_MED, NULL);

#endif

}