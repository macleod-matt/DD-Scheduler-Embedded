#include "TaskList.h"

#ifndef USERDEFINEDTASKS_H_
#define USERDEFINEDTASKS_H_

#define testBench (1)



#if testBench == 1

	# define task1_PERIOD (3000)
	# define task1_ET   (1000/portTICK_PERIOD_MS)

	# define task2_PERIOD (6000)
	# define task2_ET   (1000/portTICK_PERIOD_MS)

	# define task3_PERIOD (9000)
	# define task3_ET   (1000/portTICK_PERIOD_MS)

// Aperiotic Task

	# define task4_PERIOD (9000)
	# define task4_ET   (1000/portTICK_PERIOD_MS)

#endif

#if testBench == 2

	# define task1_PERIOD (3000)
	# define task1_ET   (1000/portTICK_PERIOD_MS)

	# define task2_PERIOD (6000)
	# define task2_ET   (1000/portTICK_PERIOD_MS)

	# define task3_PERIOD (9000)
	# define task3_ET   (1000/portTICK_PERIOD_MS)

// Aperiotic Task

	# define task4_PERIOD (9000)
	# define task4_ET   (1000/portTICK_PERIOD_MS)

#endif


#if testBench == 3

	# define task1_PERIOD (3000)
	# define task1_ET   (1000/portTICK_PERIOD_MS)

	# define task2_PERIOD (6000)
	# define task2_ET   (1000/portTICK_PERIOD_MS)

	# define task3_PERIOD (9000)
	# define task3_ET   (1000/portTICK_PERIOD_MS)

// Aperiotic Task

	# define task4_PERIOD (9000)
	# define task4_ET   (1000/portTICK_PERIOD_MS)

#endif



pTaskHandle_t Create_DD_Task_Node();
TaskHandle_t taskHandle1;
TaskHandle_t taskHandle2;
TaskHandle_t taskHandle3;



void Periodic_Task_Routine(void *pvParameters);
void Task1_Init(void *pvParameters);
void Task2_Init(void *pvParameters);
void Task3_Init(void *pvParameters);







#endif
