
#ifndef MONITORTASK_H_
#define MONITORTASK_H_

void print_LL_Nodes(pTaskListHandle_t LinkedList);
void Monitor_Task(void *pvParameters);


uint32_t get_active_dd_task_list(void);
uint32_t get_completed_dd_task_list(void);
uint32_t get_overdue_dd_task_list(void);

extern xQueueHandle xDDS_Msg_Queue;
extern xQueueHandle xMonitor_Msg_Queue;


#endif


