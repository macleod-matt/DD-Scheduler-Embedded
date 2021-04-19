
#include "UserDefinedTasks.h"
#include "STM_GPIO_CONFIG.h"
#include "TaskList.h"
#include "DDS.h"



TaskHandle_t taskHandle1;
TaskHandle_t taskHandle2;
TaskHandle_t taskHandle3;



void Read_User_Defined_Tasks(void){

	xTaskCreate(Task1_Generator,"_Task_1", configMINIMAL_STACK_SIZE, NULL, PriorityLevel_MED, &taskHandle1);
	xTaskCreate(Task2_Generator,"_Task_2", configMINIMAL_STACK_SIZE, NULL, PriorityLevel_MED, &taskHandle2);
	xTaskCreate(Task3_Generator,"_Task_3", configMINIMAL_STACK_SIZE, NULL, PriorityLevel_MED, &taskHandle3);

}



int main(void)
{

	DDS_Init(); //Initalize DDS
	Read_User_Defined_Tasks(); // Read User defined task
    vTaskStartScheduler(); // Start Scheduler

}




void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	printf("MALLOC FAILED DIPSHIT\n");
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */

	printf("STATIC STACK OVERFLOW DIPSHIT!\n");
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}

