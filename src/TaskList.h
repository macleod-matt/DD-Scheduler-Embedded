
#ifndef TASKLIST_H_
#define TASKLIST_H_

#include "STM_GPIO_CONFIG.h"




typedef enum task_type_t {

	DD_PERIODIC,
	DD_APERIODIC

}DD_task_type_t;


typedef enum DD_State_Type_t
{

	ActiveState = 0x1,
	CreateState = 0x2,
	CompleteState = 0x4,
	DeleteState = 0x8,
	OverdueState = 0x16,

}DD_State_Type_t;



typedef struct dd_task_t {

	TaskHandle_t task_handle;
	TaskFunction_t    task_function;
	DD_task_type_t task_type;
	const char *  task_name;
	TickType_t release_time;
	TickType_t execution_time;
	TickType_t absolute_deadline;
	uint32_t 	task_state;
	struct dd_task_t* next;
	struct dd_task_t* previous;

}dd_task_t, * pTaskHandle_t ;





void ReadUserDefinedTasks(void *pvParameters);
void Periotic_Task_Routine(void);


typedef struct DD_TaskList_t
{
    pTaskHandle_t head;
    pTaskHandle_t tail;

} DD_TaskList_t, * pTaskListHandle_t;



//void Init_Task_Names_List(pNamesNode_t namesNode);

void MonitorTask(void);


void Sort_Overdue_From_Active( pTaskListHandle_t   , pTaskListHandle_t );

void Insert_DDT_to_LL(pTaskHandle_t task, pTaskListHandle_t taskList);


uint8_t  get_List_Size(DD_TaskList_t );
#endif
