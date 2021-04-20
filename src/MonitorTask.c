#include "UserDefinedTasks.h"
#include "TaskList.h"
#include "DDS.h"
#include "STM_GPIO_CONFIG.h"
#include "MonitorTask.h"

uint32_t get_active_dd_task_list(void);
uint32_t get_completed_dd_task_list(void);
uint32_t get_overdue_dd_task_list(void);

/*
 *
 * F-Task to extract information from the DDS and report scheduling information.
 *
 */

void Monitor_Task(void *pvParameters) {

	while (1) {

		debugPrint("\nMONITOR TASK:\n");
		get_active_dd_task_list();
		get_completed_dd_task_list();
		get_overdue_dd_task_list();

		vTaskDelay(100);

	}

}

/*
 * HELPER FUCNTION: Loops through all tasks. Prints all tasks in LL
 *
 * Input: pointer to linked list of task handles
 *
 *
 */

void print_LL_Nodes(pTaskListHandle_t LinkedList) {

	if (LinkedList->head == NULL && LinkedList->tail == NULL) {

		printf("\nNo Task In list!\n");

		return;

	}

	pTaskHandle_t LL_index = LinkedList->head;

	while (LL_index != NULL) {

		printf("Task: [%s]", LL_index->task_name);

		printf("\n");
		LL_index = LL_index->next;

	}
	printf("\n");

	return;

}

/*
 *
 *
 * This function sends a message to a queue requesting the Active Task List from the DDS.
 * Once a response is received from the DDS, the function returns the list.
 */

uint32_t get_active_dd_task_list(void) {

	DD_Message_t msg_activeList = { Msg_ActiveList, NULL, NULL, NULL, NULL };

	if ( xQueueSend(xDDS_Msg_Queue, &msg_activeList,portMAX_DELAY) != pdPASS) // ensure the message was sent
	{

		printf(
				"\nERROR:Unable to send ACTIVE LIST message to DDS Msg Queue!\n");
		return 0;
	}

	if ( xQueueReceive(xMonitor_Msg_Queue,&msg_activeList,
			portMAX_DELAY) == pdTRUE) {

		debugPrint("ACTIVE TASKS:\n");

		print_LL_Nodes(msg_activeList.pList);

		msg_activeList.pList = NULL;
	}
	return 1;

}

/*
 * This function sends a message to a queue requesting the Completed Task List from the DDS.
 *  Once a response is received from the DDS, the function returns the list.
 *
 *
 */

uint32_t get_completed_dd_task_list(void) {

	DD_Message_t msg_completedList =
			{ Msg_CompleteList, NULL, NULL, NULL, NULL };

	if ( xQueueSend(xDDS_Msg_Queue, &msg_completedList,
			portMAX_DELAY) != pdPASS) // ensure the message was sent
	{

		printf(
				"\nERROR:Unable to send COMPLETED LIST message to DDS Msg Queue!\n");
		return 0;
	}
	if ( xQueueReceive(xMonitor_Msg_Queue, &msg_completedList,
			portMAX_DELAY) == pdTRUE) {

		debugPrint("COMPLETED TASKS:\n");

		print_LL_Nodes(msg_completedList.pList);

		msg_completedList.pList = NULL;
	}

	return 1;

}

/*
 * This function sends a message to a queue requesting the Overdue Task List from the DDS.
 *  Once a response is received from the DDS, the function returns the list.
 *
 */

uint32_t get_overdue_dd_task_list(void) {

	DD_Message_t msg_overdueList = { Msg_OverDueList, NULL, NULL, NULL, NULL };
//
	if ( xQueueSend(xDDS_Msg_Queue, &msg_overdueList,
			portMAX_DELAY) != pdPASS) // ensure the message was sent
	{

		printf(
				"\nERROR:Unable to send OVERDUE LIST message to DDS Msg Queue!\n");

		return 0;
	}

	if ( xQueueReceive(xMonitor_Msg_Queue, &msg_overdueList,
			portMAX_DELAY) == pdTRUE) {

		debugPrint("OVERDUE TASKS:\n");
		print_LL_Nodes(msg_overdueList.pList);

		msg_overdueList.pList = NULL;
	}
	return 1;

}

