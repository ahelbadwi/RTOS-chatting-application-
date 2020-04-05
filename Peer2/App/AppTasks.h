/*
 * AppTasks.h
 *
 *  Created on: Feb 1, 2020
 *      Author: Ahemd_Elbadwi 
 */

#ifndef APPTASKS_H_
#define APPTASKS_H_

#include "AppTasks_cfg.h"

#define MAX_KEYPAD_MESSAGE (17U)

extern void Tasks_Init(void *pvParameters);
extern void KeyPad_Task(void *pvParameters);
extern void LcdCyclic_Task(void *pvParameter);
extern void MonitorPushButton1_Task(void *pvParameter);
extern void Uart_SendTask(void *pvParameter);
extern void LedToggle_200(void *pvParameters);

extern void Uart_ReceiveTask(void *pvParameter);

extern TaskHandle_t LedToggle_200_Handller;

#endif /* APPTASKS_H_ */
