/*
 *  File Name   : main.c
 *  Description : contains the main function
 *  Created on: Jan 31, 2020
 *  Author: Ahemd_Elbadwi
 */

/*************************************************************************/
/*                         Includes                                      */
/*************************************************************************/
#include "std_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "AppTasks.h"
/*************************************************************************/
/*                         Global Variables                              */
/*************************************************************************/
QueueHandle_t KeypadMsgToUart_Queue=NULL;
QueueHandle_t KeypadMsgToPushButton_Queue=NULL;
QueueHandle_t ReceivedUartKeypadMsg_Queue=NULL;
SemaphoreHandle_t Uart_BinarySemaphore=NULL;
SemaphoreHandle_t LastByteSignal_BinarySemaphore=NULL;
SemaphoreHandle_t RxLed_Semaphore=NULL;
/*************************************************************************/
/*                         Main Function                                 */
/*************************************************************************/
int main(void)
{
	/* Create a Queue for sending/receiving pressed keypad buttons. */
	KeypadMsgToPushButton_Queue=xQueueCreate(MAX_KEYPAD_MESSAGE,sizeof(uint8));
	/* Create a Queue for sending to UART Send task/receiving pressed keypad buttons
	 * to UART send task/from push button task.
	 */
	KeypadMsgToUart_Queue=xQueueCreate(MAX_KEYPAD_MESSAGE,sizeof(uint8));
	/* Create a Queue to receive keypad message from the sender peer */
	ReceivedUartKeypadMsg_Queue=xQueueCreate(MAX_KEYPAD_MESSAGE,sizeof(uint8));
	/* Create a binary semaphore for signaling UART TASK to start sending the keypad message. */
	Uart_BinarySemaphore=xSemaphoreCreateBinary();
	/* Create a binary semaphore for signaling a Led Task to toggle led once received a new message. */
	RxLed_Semaphore = xSemaphoreCreateBinary();
	/*
	 *Tasks Creation
	 */
	/* Create task to initialize tasks */
	xTaskCreate(Tasks_Init,"InitTasks",configMINIMAL_STACK_SIZE,NULL,Init_Task_PRIORITY,NULL);
	/* Create task to get pressed buttons of keypad  */
	xTaskCreate(KeyPad_Task,"keypad",100,NULL,KEYPAD_PRIORITY,NULL);
	/* Create task to scan if LCD request had come */
	xTaskCreate(LcdCyclic_Task,"LcdCyclic",100,NULL,LCD_CYCLIC_PRIORITY,NULL);
	/* Create task to Monitor push button */
	xTaskCreate(MonitorPushButton1_Task,"BTN",150,NULL,PUSH_BUTTON_PRIORITY,NULL);
	/* Create task to send a keypad message through UART channel. */
	xTaskCreate(Uart_SendTask,"UART_TX",200,NULL,UART_SEND_PRIORITY,NULL);
	/* Create task to Receive a keypad message through UART channel. */
	xTaskCreate(Uart_ReceiveTask,"UART_RX",150,NULL,4,NULL);
	/* Create task to toggle led with rate 400 MSEC. */
	xTaskCreate(LedToggle_400,"Led_400",50,NULL,2,NULL);
	/* Run Scheduler */
	vTaskStartScheduler();
	/* If all is well then main() will never reach here as the scheduler will now be
	running the tasks. If main() does reach here then it is likely that there was
	insufficient heap memory available for the idle task to be created. Chapter 2
	provides more information on heap memory management. */
	while(1);

	return 0;
}
