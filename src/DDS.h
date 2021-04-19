#ifndef DDS_H
#define DDS_H_

#include "STM_GPIO_CONFIG.h"
#include "TaskList.h"



// Priority levels


#define PriorityLevel_LOW (0)
#define PriorityLevel_MED (1)
#define PriorityLevel_HIGH (2)
#define PrioirtyLevel_MAX (5)



#define MAX_NUM_DDS_MSGS (5) // MAX Number of Messages in DDS Msg queue
#define MAX_NUM_MONITOR_MSGS (2) // MAX Number of Messages in Monitor msg Queue
#define bufferSize (configMAX_TASK_NAME_LEN + 100) // output  tasklist buffersize


xQueueHandle xDDS_Msg_Queue;
xQueueHandle xMonitor_Msg_Queue;




// Message types
typedef enum DD_Message_Type_t
{
    Msg_Create_DDT = 0x1,
	Msg_Release_DDT = 0x2,
	Msg_Delete_DDT = 0x4 ,
	Msg_Complete_DDT = 0x8,
	Msg_Suspend_DDT = 0x16,
	Msg_ActiveList = 0x32 ,
	Msg_OverDueList = 0x64,
	Msg_CompleteList = 0x128,


}DD_Message_Type_t;




// Message structure
typedef struct DD_Message_t
{
    DD_Message_Type_t     type;
    TaskHandle_t      	  pTask;
    void*     			  data;
    pTaskListHandle_t     pList;
    bool 				 taskExists;
}DD_Message_t;




void DDS_Init(void);

uint32_t create_dd_task(DD_TaskHandle_t );
uint32_t release_dd_task(DD_TaskHandle_t );

uint32_t delete_dd_task(DD_TaskHandle_t );
uint32_t complete_dd_task(pTaskHandle_t pTask );
pTaskHandle_t task_exists_in_List(pTaskListHandle_t , pTaskHandle_t  );



DD_TaskList_t taskList_ACTIVE;
DD_TaskList_t taskList_OVERDUE;
DD_TaskList_t taskList_COMPLETED;


//pNamesNode_t taskList_COMPLETED;


#endif
