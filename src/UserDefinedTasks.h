#include "TaskList.h"

#ifndef USERDEFINEDTASKS_H_
#define USERDEFINEDTASKS_H_





pTaskHandle_t Create_DD_Task_Node();
extern TaskHandle_t taskHandle1;
extern TaskHandle_t taskHandle2;
extern TaskHandle_t taskHandle3;



void Periodic_Task_Routine(void *pvParameters);
void Task1_Generator(void *pvParameters);
void Task2_Generator(void *pvParameters);
void Task3_Generator(void *pvParameters);







#endif
