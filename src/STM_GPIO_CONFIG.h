
#ifndef STM_GPIO_CONFIG_H_
#define STM_GPIO_CONFIG_H_

#include <stdint.h>
#include <stdio.h>
#include "stm32f4_discovery.h"
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"
#include "../FreeRTOS_Source/include/event_groups.h"

TaskHandle_t taskHandle1;
TaskHandle_t taskHandle2;
TaskHandle_t taskHandle3;


# define DEBUG_MODE (1)

# if DEBUG_MODE == 1
    # define debugPrint printf

# else
    # define debugPrint
# endif

#define TEST_COMS (0)

# if TEST_COMS == 1
    # define TEST_ulTaskNotifyTake ulTaskNotifyTake
	# define TEST_xTaskNotifyGive xTaskNotifyGive

# else
    # define TEST_ulTaskNotifyTake
	#define TEST_xTaskNotifyGive
# endif



#define INSERTLIST_DEBUG (1)

#if INSERTLIST_DEBUG == 1

	#define AList_Print printf
#else
	#define AList_Print

#endif


#define MONITOR_MODE (0)






#endif
