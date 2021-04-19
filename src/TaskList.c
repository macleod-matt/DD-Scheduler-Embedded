/*
 * This module is responsible for all linked list sorting helper functions seen in the  API
 *
 *
 *
 */

#include "UserDefinedTasks.h"
#include "TaskList.h"
#include "DDS.h"
#include "STM_GPIO_CONFIG.h"
#include "MonitorTask.h"


//
//uint8_t get_List_Size(DD_TaskList_t taskList){
//
//	iterator = tastkList->head;
//
//	uint8_t count = 0;
//	while(iterator != NULL){
//		count ++;
//
//		iterator = iterator->next;
//	}
//
//	return count;
//
//}


bool Free_DDT_NODE(pTaskHandle_t task_to_remove) {
	// Check input parameters are not NULL
	if (task_to_remove == NULL) {
		printf(
				"ERROR(DD_Task_Free): one of the pa1rameters passed was NULL.\n");
		return false;
	}

	// return false if the task wasn't removed from active/overdue queue, or removed from active tasks.
	if (task_to_remove->next != NULL || task_to_remove->previous != NULL) {
		printf(
				"ERROR(DD_Task_Free): Forgot to remove task from list, not deleting it. Fix the code.\n");
		return false;
	}

	// Reset the data before freeing - some errors can occur if not done
	task_to_remove->task_handle = NULL;
	task_to_remove->task_function = NULL;
	task_to_remove->task_name = "";
	task_to_remove->task_type = DD_PERIODIC;
	task_to_remove->release_time = 0;
	task_to_remove->absolute_deadline = 0;
	task_to_remove->next = NULL;
	task_to_remove->previous = NULL;

	// free the memory used by the task
	vPortFree((void*) task_to_remove);

	return true;
}

void Init_DD_TaskList(pTaskListHandle_t DDT_List) {
	// Check input parameters are not NULL

	if (DDT_List != NULL) {

		DDT_List->head = NULL;
		DDT_List->tail = NULL;

	} else {

		printf("ERROR! NULL Task List. Unable to instantiate linked list!");

	}

}




// Insert task by deadline

void Insert_DDT_to_LL(pTaskHandle_t task, pTaskListHandle_t taskList) {



	if ((taskList->head == NULL) && (taskList->tail == NULL)) {

		taskList->head = task;
		taskList->tail = task;
		vTaskPrioritySet(task->task_handle, PriorityLevel_HIGH); // Set high priority initally since it is the only task in LL


		return;

	}

	pTaskHandle_t LL_index = taskList->head;
	uint32_t priority_index = uxTaskPriorityGet(LL_index->task_handle);

	priority_index++;

	while (LL_index != NULL) {

		// Check if deadline of new task is less than that of the current head of list
		if (task->absolute_deadline < LL_index->absolute_deadline) {

			// check if Linked list index is the head
			if (LL_index == taskList->head) {
				taskList->head = task; // If so, make the head of the list now the new task
			}

			task->next = LL_index;
			task->previous = LL_index->previous;
			LL_index->previous = task;

			vTaskPrioritySet(task->task_handle, priority_index);

			return;

		} else {

			if (LL_index->next == NULL) { // reached end of list
				task->next = NULL;
				task->previous = LL_index;
				LL_index->next = task;
				taskList->tail = task;

				vTaskPrioritySet(LL_index->task_handle, priority_index); // update the current cell's priority
				vTaskPrioritySet(task->task_handle, PriorityLevel_HIGH); // as the item is now the bottom, assign it bottom priority.
				return;

			}

			vTaskPrioritySet(LL_index->task_handle, priority_index);
			priority_index--;
			LL_index = LL_index->next;
		}


	}

}







/*
 * HELPER FUNCTION:
 *
 *Removes a task from  list
 *
 *LinkedList: LL Of connected Task Strucures
 *
 *task: DDT Task to remove from list
 *
 *
 *
 */

void remove_DDT_From_LL(pTaskListHandle_t linkedList, pTaskHandle_t taskToRemove, bool task_Completed) {




	if (linkedList == NULL) {

		printf("error... Attempting to mutate values from a null Linked list");

		return;

	}

	pTaskHandle_t LL_index = linkedList->head;

	if (LL_index == NULL) {

		debugPrint("Unable to locate head of list\n");

		return;

	}

	uint32_t priority_index = uxTaskPriorityGet(LL_index->task_handle); // grab the highest priority value

	while (LL_index != NULL) {

		if (LL_index->task_handle == taskToRemove->task_handle) {

//			Insert_Completed_TaskNames( &taskList_COMPLETED, LL_index);


			// Insert Task to completed List

			if (linkedList->head->task_handle == linkedList->tail->task_handle) { // if head TH == tail TH, only one element in LL

				linkedList->head = NULL;
				linkedList->tail = NULL;
			}

			else if (taskToRemove->task_handle == linkedList->head->task_handle) { // check if attempting to remove head
				linkedList->head = LL_index->next;
				LL_index->next->previous = NULL;

			} else if (taskToRemove->task_handle = linkedList->tail->task_handle) { // Check if we are attempting to remove tail of list

				linkedList->head = LL_index->previous;
				LL_index->previous->next = NULL;

			}

			else { // Removing somewhere in the middle of the list

				LL_index->previous->next = LL_index->next;
				LL_index->next->previous = LL_index->previous;

			}
			LL_index->previous = NULL;
			LL_index->next = NULL;





			if (task_Completed) {


				Free_DDT_NODE(LL_index);
			}

			return;

		}

		priority_index--;

		vTaskPrioritySet(LL_index->task_handle, priority_index);

		LL_index = LL_index->next;


	}

}

void Sort_Overdue_From_Active(pTaskListHandle_t Active_TaskList,
		pTaskListHandle_t Overdue_TaskList) {

	// Check input parameters are not NULL



	if ((Active_TaskList == NULL) || (Overdue_TaskList == NULL)) {
		printf(
				"ERROR(DD_TaskList_Transfer_Overdue): one of the parameters passed was NULL.\n");
		return;
	}

	pTaskHandle_t LL_index = Active_TaskList->head;

	TickType_t currentTime = xTaskGetTickCount();

	pTaskHandle_t tempNode;


	while (LL_index != NULL) {

		if (LL_index->absolute_deadline < currentTime) { // Deadline has passed


			LL_index->task_state = OverdueState;


			// Remove Task handle from linked list
			remove_DDT_From_LL(&Active_TaskList, LL_index, true);

			if ((Overdue_TaskList->head == NULL)
					&& (Overdue_TaskList->tail == NULL)) { // Check if no items are in list

				Overdue_TaskList->head = LL_index;
				Overdue_TaskList->tail = LL_index;
			}

			else {

				tempNode = Overdue_TaskList->tail;
				Overdue_TaskList->tail = LL_index;
				tempNode->next = LL_index;
				LL_index->previous = tempNode;

			}

//			if (LL_index->task_type == DD_APERIODIC) {
//
//
//				vTaskSuspend(LL_index->task_handle);
//				vTaskDelete(LL_index->task_handle);
//			}
		} else {

			return;
		}

		LL_index = LL_index->next;

	}

}





void add_DDT_to_Completed(pTaskHandle_t pTask){

	if (pTask == NULL) {

			printf(
					"ERROR: Request to create task with null pointer to task handle\n");
			return 0;

		}

	// Remove pTask from active list -- Dont delete the node
		remove_DDT_From_LL(&taskList_ACTIVE, pTask, false);



		pTaskListHandle_t completedList = &taskList_COMPLETED;

		pTaskHandle_t iterator = completedList->head;


		if (iterator == NULL) {

			completedList->head = pTask;
			completedList->tail = NULL;

		} else {


			while(iterator!=NULL){

				if(iterator == completedList->head ){

					completedList->head = pTask;
				}

				pTask->next = iterator;
				pTask->previous = iterator->previous;
				iterator->previous = pTask;

				iterator = iterator->next;


			}


		}



}
