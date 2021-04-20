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

	pTaskHandle_t LL_index = taskList->head;

	if ((taskList->head == NULL) && (taskList->tail == NULL)) {
		taskList->head = task;
		taskList->tail = task;
	} else {
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
				break;

			} else {

				if (LL_index->next == NULL) { // reached end of list
					task->next = NULL;
					task->previous = LL_index;
					LL_index->next = task;
					taskList->tail = task;
					break;
				}

				LL_index = LL_index->next;
			}
		}
	}

	uint32_t priority_index = PriorityLevel_MAXT;

	LL_index = taskList->head;


	debugPrint("\n");

	while (LL_index != NULL) {

		debugPrint("[%s-%u]", LL_index->task_name, priority_index);

		vTaskPrioritySet(LL_index->task_handle, priority_index);

		LL_index = LL_index->next;

		priority_index--;

	}

	debugPrint("\n");

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

void remove_DDT_From_LL(pTaskListHandle_t linkedList, pTaskHandle_t taskToRemove) {

	if (linkedList == NULL) {

		printf("error... Attempting to mutate values from a null Linked list");

		return;

	}

	pTaskHandle_t LL_index = linkedList->head;

	if (LL_index == NULL) {

		debugPrint("Unable to locate head of list\n");

		return;

	}

	while (LL_index != NULL) {

		if (LL_index->task_handle == taskToRemove->task_handle) {

			// Insert Task to completed List

			if (linkedList->head->task_handle
					== linkedList->tail->task_handle) { // if head TH == tail TH, only one element in LL

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

			return;

		}

		LL_index = LL_index->next;

	}

}

void Sort_Overdue_From_Active(pTaskListHandle_t Active_TaskList, pTaskListHandle_t Overdue_TaskList) {

	// Check input parameters are not NULL

	if ((Active_TaskList == NULL) || (Overdue_TaskList == NULL)) {
		printf(
				"ERROR(DD_TaskList_Transfer_Overdue): one of the parameters passed was NULL.\n");
		return;
	}

	pTaskHandle_t LL_index = Active_TaskList->head;

	pTaskHandle_t tempNode;

	while (LL_index != NULL) {

		TickType_t currentTime = xTaskGetTickCount();

		if (LL_index->absolute_deadline < currentTime) { // Deadline has passed

			LL_index->task_state = OverdueState;

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

			// Remove Node From Ative List

			remove_DDT_From_LL(&taskList_ACTIVE, LL_index);

		} else {

			return;
		}

		LL_index = LL_index->next;

	}

}

void add_DDT_to_Completed(pTaskHandle_t pTask) {

	if (pTask == NULL) {

//		printf(
//				"ERROR: Request to create task with null pointer to task handle\n");
		return ;

	}

	// Remove pTask from active list -- Dont delete the node

	pTaskListHandle_t completedList = &taskList_COMPLETED;

	pTaskHandle_t iterator = completedList->head;

	if (iterator == NULL) {

		completedList->head = pTask;
		completedList->tail = NULL;

	} else {

		while (iterator != NULL) {

			if (iterator == completedList->head) {

				completedList->head = pTask;
			}

			pTask->next = iterator;
			pTask->previous = iterator->previous;
			iterator->previous = pTask;

			iterator = iterator->next;

		}

	}

	// remove task from Active or overdue list


	pTaskHandle_t taskExists_ACTIVE = task_exists_in_List(&taskList_ACTIVE,
			pTask);

	pTaskHandle_t taskExists_OVERDUE = task_exists_in_List(&taskList_OVERDUE,
			pTask);

	if (taskExists_ACTIVE != NULL) {
		remove_DDT_From_LL(&taskList_ACTIVE, pTask);

	}

	else if (taskExists_OVERDUE != NULL) {

		remove_DDT_From_LL(&taskExists_OVERDUE, pTask);

	}

}
