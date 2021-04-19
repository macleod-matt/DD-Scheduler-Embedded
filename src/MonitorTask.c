
#include "UserDefinedTasks.h"
#include "TaskList.h"
#include "DDS.h"
#include "STM_GPIO_CONFIG.h"


uint32_t get_active_dd_task_list(void);
uint32_t get_completed_dd_task_list(void);
uint32_t get_overdue_dd_task_list(void);

/*
 *
 * F-Task to extract information from the DDS and report scheduling information.
 *
 */

void MonitorTask(void){

	while (1) {

		debugPrint("\n\n MONITORING TASK:\n Current Time: [%u]\n Priority Level: [%u] \n\n",
				(unsigned int) xTaskGetTickCount(),
				(unsigned int) uxTaskPriorityGet( NULL));

		get_active_dd_task_list();
		get_completed_dd_task_list();
		get_overdue_dd_task_list();

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

		printf("\nTask: [%s], Deadline: [%u] \n", LL_index->task_name,
				(unsigned int) LL_index->absolute_deadline);

		//vPortFree((void*) LL_index); // free memory block

		LL_index = LL_index->next;
		//delete_dd_task(LL_index);

	}
	return;

}



/*
 *
 *
 * This function sends a message to a queue requesting the Active Task List from the DDS.
 * Once a response is received from the DDS, the function returns the list.
 *
 */

uint32_t get_active_dd_task_list(void) {

	DD_Message_t msg_activeList = { Msg_ActiveList, NULL, NULL, NULL };

	if (xDDS_Msg_Queue != NULL) // Check that the queue exists
	{
		if ( xQueueSend(xDDS_Msg_Queue, &msg_activeList,portMAX_DELAY) != pdPASS) // ensure the message was sent
		{

			printf(
					"\nERROR:Unable to send ACTIVE LIST message to DDS Msg Queue!\n");
			return 0;
		}
	} else {
		printf("ERROR: DD_Scheduler_Message_Queue is NULL.\n");
		return 0;
	}

	if (xMonitor_Msg_Queue != NULL) // Check that the queue exists
	{
		if ( xQueueReceive(xMonitor_Msg_Queue,&msg_activeList,
				portMAX_DELAY) == pdTRUE) {

			debugPrint("ACTIVE TASKS:\n");

			print_LL_Nodes(msg_activeList.pList);


			msg_activeList.pList = NULL;
		}
	} else {
		printf("ERROR: DD_Monitor_Message_Queue is NULL.\n");
		return 0;
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

	DD_Message_t msg_completedList = { Msg_CompleteList, NULL, NULL, NULL };

	if (xDDS_Msg_Queue != NULL) // Check that the queue exists
	{
		if ( xQueueSend(xDDS_Msg_Queue, &msg_completedList,
				portMAX_DELAY) != pdPASS) // ensure the message was sent
		{

			printf(
					"\nERROR:Unable to send COMPLETED LIST message to DDS Msg Queue!\n");
			return 0;
		}
	} else {
		printf("ERROR: DD_Scheduler_Message_Queue is NULL.\n");
		return 0;
	}

	if (xMonitor_Msg_Queue != NULL) // Check that the queue exists
	{
		if ( xQueueReceive(xMonitor_Msg_Queue, &msg_completedList,
				portMAX_DELAY) == pdTRUE) {

			debugPrint(
					"\n\n................COMPLETED TASKS: .................: \n\n");

			print_LL_Nodes(msg_completedList.pList);

			debugPrint(
					"\n\n...................................................\n\n");

			//vPortFree((void*)msg_completedList.pList);

			msg_completedList.pList = NULL;
		}
	} else {
		printf("ERROR: DD_Monitor_Message_Queue is NULL.\n");
		return 0;
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
	if (xDDS_Msg_Queue != NULL) // Check that the queue exists
	{
		if ( xQueueSend(xDDS_Msg_Queue, &msg_overdueList,
				portMAX_DELAY) != pdPASS) // ensure the message was sent
		{

			printf(
					"\nERROR:Unable to send OVERDUE LIST message to DDS Msg Queue!\n");

			return 0;
		}
	} else {
		printf("ERROR: DD_Scheduler_Message_Queue is NULL.\n");
		return 0;
	}

	if (xMonitor_Msg_Queue != NULL) // Check that the queue exists
	{
		if ( xQueueReceive(xMonitor_Msg_Queue, &msg_overdueList,
				portMAX_DELAY) == pdTRUE) {

			debugPrint(
					"\n\n................OVERDUE TASKS .................: \n\n");

			print_LL_Nodes(msg_overdueList.pList);

			//vPortFree((void*)msg_overdueList.pList);

			debugPrint(
					"\n\n................................................\n\n");

			msg_overdueList.pList = NULL;
		}
	} else {
		printf("ERROR: DD_Monitor_Message_Queue is NULL.\n");
		return 0;
	}

	return 1;

}

