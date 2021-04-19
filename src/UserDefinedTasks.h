#include "TaskList.h"

#ifndef USERDEFINEDTASKS_H_
#define USERDEFINEDTASKS_H_

#define testBench (1)



#if testBench == 1

	# define task1_PERIOD pdMS_TO_TICKS(500)
	# define task1_ET   pdMS_TO_TICKS(95)

	# define task2_PERIOD pdMS_TO_TICKS(500)
	# define task2_ET   pdMS_TO_TICKS(150)

	# define task3_PERIOD pdMS_TO_TICKS(750)
	# define task3_ET  pdMS_TO_TICKS(250)



#endif

#if testBench == 2

	# define task1_PERIOD (250)
	# define task1_ET   (150/portTICK_PERIOD_MS)

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
extern TaskHandle_t taskHandle1;
extern TaskHandle_t taskHandle2;
extern TaskHandle_t taskHandle3;



void Periodic_Task_Routine(void *pvParameters);
void Task1_Generator(void *pvParameters);
void Task2_Generator(void *pvParameters);
void Task3_Generator(void *pvParameters);







#endif
