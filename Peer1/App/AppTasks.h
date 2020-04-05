/*
 *  File Name :AppTasks.h
 *  Created on: Jan 31, 2020
 *  Author    : Ahemd_Elbadwi
 */

#ifndef APPTASKS_H_
#define APPTASKS_H_
/*************************************************************************/
/*                         Includes                                      */
/*************************************************************************/
#include "AppTasks_cfg.h"
/*************************************************************************/
/*                         #Defines                                      */
/*************************************************************************/
#define MAX_KEYPAD_MESSAGE (17U)
/*************************************************************************/
/*                         Tasks Exports                                 */
/*************************************************************************/
/*******************************************************************************
 * 	Function name: 		Tasks_Init
 *
 * 	Description  :      UART,KeyPad,Push Button,led and LCD Initialization.
 *
 * 	Arguments:
 * 		-				pvParameters:void pointer to argument(Not used)
 *
 * 	Return:
 * 		-				NA
 *******************************************************************************/
extern void Tasks_Init(void *pvParameters);
/*******************************************************************************
 * 	Function name: 		KeyPad_Task
 *
 * 	Description  :      Send a pressed keypad buttons to a push button task
 * 	                    a Queue through.
 *
 * 	Arguments:
 * 		-				pvParameters:void pointer to argument(Not used)
 *
 * 	Return:
 * 		-				NA
 *******************************************************************************/
extern void KeyPad_Task(void *pvParameters);
/*******************************************************************************
 * 	Function name: 		MonitorPushButton0_Task
 *
 * 	Description  :      Get status of BTN 0 and send it periodically through a Queue
 * 	                    to LedSwitch_Task and LcdRequest_Task .
 *
 * 	Arguments:
 * 		-				pvParameters:void pointer to argument(Not used)
 *
 * 	Return:
 * 		-				NA
 *******************************************************************************/
extern void MonitorPushButton1_Task(void *pvParameter);
/*******************************************************************************
 * 	Function name: 		LcdCyclic_Task
 *
 * 	Description  :      check periodically for coming requests to display data
 * 	                    or send commands to LCD.
 *
 * 	Arguments:
 * 		-				pvParameters:void pointer to argument(Not used)
 *
 * 	Return:
 * 		-				NA
 *******************************************************************************/
extern void LcdCyclic_Task(void *pvParameter);
/*******************************************************************************
 * 	Function name: 		Uart_SendTask
 *
 * 	Description  :      Receive KeypadMsg Queue and send it through UART channel
 * 	Arguments:
 * 		-				pvParameters:void pointer to argument(Not used)
 *
 * 	Return:
 * 		-				NA
 *******************************************************************************/
extern void Uart_SendTask(void *pvParameter);

/*******************************************************************************
 * 	Function name: 		Uart_ReceiveTask
 *
 * 	Description  :      Receive KeypadMsg Queue From the UART RX Complete ISR.
 * 	Arguments:
 * 		-				pvParameters:void pointer to argument(Not used)
 *
 * 	Return:
 * 		-				NA
 *******************************************************************************/
extern void Uart_ReceiveTask(void *pvParameter);

/*******************************************************************************
 * 	Function name: 		LedToggle_400
 *
 * 	Description  :      this task will toggle led with 400 MSEC only when
 * 	                    UART received task give it the semaphore.
 * 	Arguments:
 * 		-				pvParameters:void pointer to argument(Not used)
 *
 * 	Return:
 * 		-				NA
 *******************************************************************************/
extern void LedToggle_400(void *pvParameters);



#endif /* APPTASKS_H_ */
