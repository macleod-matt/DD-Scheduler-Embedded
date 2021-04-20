#include "TaskList.h"
#include "DDS.h"
#include "UserDefinedTasks.h"
#include "MonitorTask.h"


#define testBench (2)


#if testBench == 1

	# define task1_PERIOD pdMS_TO_TICKS(500)
	# define task1_ET   pdMS_TO_TICKS(95)

	# define task2_PERIOD pdMS_TO_TICKS(500)
	# define task2_ET   pdMS_TO_TICKS(150)

	# define task3_PERIOD pdMS_TO_TICKS(750)
	# define task3_ET  pdMS_TO_TICKS(250)



#endif

#if testBench == 2

	# define task1_PERIOD pdMS_TO_TICKS(250)
	# define task1_ET   pdMS_TO_TICKS(95)

	# define task2_PERIOD pdMS_TO_TICKS(500)
	# define task2_ET   pdMS_TO_TICKS(150)

	# define task3_PERIOD pdMS_TO_TICKS(750)
	# define task3_ET  pdMS_TO_TICKS(250)

#endif


#if testBench == 3

	# define task1_PERIOD pdMS_TO_TICKS(500)
	# define task1_ET   pdMS_TO_TICKS(100)

	# define task2_PERIOD pdMS_TO_TICKS(500)
	# define task2_ET   pdMS_TO_TICKS(200)

	# define task3_PERIOD pdMS_TO_TICKS(500)
	# define task3_ET  pdMS_TO_TICKS(200)
#endif



/*
 * Test Bench 1 (see macros in UserDefinedTasks.h):
 *
 *  Task                	     execution time   				Period (ms)
 *  	1                             95                          	500
 *  	2                             150                         	500
 *  	3                             250                         	750
 *
 *
 *  Test Bench 2 (see macros in UserDefinedTasks.h):
 *
 *  Task                         execution time         		Period (ms)
 *  	1	                        95                              250
 *  	2	                       150                              500
 *  	3	                       250                              750
 *
 *
 *  Test Bench 3 (see macros in UserDefinedTasks.h):
 *
 *    Task                     execution time            		Period (ms)
 *  	1	                       100                          	500
 *  	2	                       200                              500
 *  	3	                       200                              500
 */

pTaskHandle_t Create_DD_Task_Node(void) {
	pTaskHandle_t taskNode = (pTaskHandle_t) pvPortMalloc(sizeof(dd_task_t));

	taskNode->task_handle = NULL;
	taskNode->task_function = NULL;
	taskNode->task_name = "";
	taskNode->task_type = DD_PERIODIC;
	taskNode->release_time = 0;
	taskNode->absolute_deadline = 0;
	taskNode->task_state = 0;
	taskNode->next = NULL;
	taskNode->previous = NULL;

	return taskNode;
}

/*
 *
 * Periodically creates DD-Tasks that need to be scheduled by the DD Scheduler.
 *
 *
 *

 */

void Periodic_Task_Routine(void *pvParameters) {

	pTaskHandle_t taskNode = (pTaskHandle_t) pvParameters;

	TickType_t currentTime = 0;
	TickType_t lastTick = 0;
	TickType_t relativeDeadline = 0;
	TickType_t absDeadline = 0;

	uint32_t tickCounter = 0;

	while (1) {

		if (taskNode->task_state == ActiveState) {

			currentTime = xTaskGetTickCount();

			absDeadline = taskNode->absolute_deadline;

			debugPrint(
					"\nExecuting [%s] | Time: %u | Prior: %u \n",
					taskNode->task_name, (unsigned int) currentTime,
					(unsigned int) uxTaskPriorityGet( taskNode->task_handle));

			lastTick = currentTime;

			tickCounter = 0;

			// Simulating execution time.
			while (tickCounter < taskNode->execution_time) {

				currentTime = xTaskGetTickCount();

				if (currentTime != lastTick) {

					tickCounter++;

				}

				lastTick = currentTime;
			}

			relativeDeadline = absDeadline - currentTime;

			complete_dd_task(taskNode);
		}

		if (taskNode->task_type == DD_APERIODIC) {
			delete_dd_task(taskNode->task_handle);

		}

	}

	vTaskDelete(NULL);

}

/*
 * Contains the actual deadline-sensitive application code written by the user.
 *
 *
 *
 */

void Task1_Generator(void *pvParameters) {

	pTaskHandle_t newTask = Create_DD_Task_Node();

	TickType_t deadline = task1_PERIOD;

	newTask->task_function = Periodic_Task_Routine;
	newTask->task_name = "Task1";
	newTask->task_type = DD_PERIODIC;
	newTask->execution_time = task1_ET;

	TickType_t currentTime = xTaskGetTickCount();
	newTask->release_time = currentTime;
	newTask->absolute_deadline = currentTime + deadline;

	while (1) {

		create_dd_task(newTask);

		vTaskDelay(deadline);

	}
}

void Task2_Generator(void *pvParameters) {

	TickType_t deadline = task2_PERIOD;
	pTaskHandle_t newTask = Create_DD_Task_Node();

	newTask->task_function = Periodic_Task_Routine;
	newTask->task_name = "Task2";
	newTask->task_type = DD_PERIODIC;
	newTask->execution_time = task2_ET;

	TickType_t currentTime = xTaskGetTickCount();
	newTask->release_time = currentTime;
	newTask->absolute_deadline = currentTime + deadline;

	while (1) {

		create_dd_task(newTask);

		vTaskDelay(deadline);

	}
}

void Task3_Generator(void *pvParameters) {
	TickType_t deadline = task3_PERIOD;
	pTaskHandle_t newTask = Create_DD_Task_Node();

	newTask->task_function = Periodic_Task_Routine;
	newTask->task_name = "Task3";
	newTask->task_type = DD_PERIODIC;
	newTask->execution_time = task3_ET;

	TickType_t currentTime = xTaskGetTickCount();
	newTask->release_time = currentTime;
	newTask->absolute_deadline = currentTime + deadline;

	while (1) {

		create_dd_task(newTask);

		vTaskDelay(deadline);

	}
}

