/*
 * main.c
 *
 *  Created on: Feb 1, 2020
 *      Author: Ahemd_Elbadwi 
 */
#include "std_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "AppTasks.h"

QueueHandle_t KeypadMsgToUart_Queue=NULL;
QueueHandle_t KeypadMsgToPushButton_Queue=NULL;
QueueHandle_t ReceivedUartKeypadMsg_Queue=NULL;
SemaphoreHandle_t Uart_BinarySemaphore=NULL;
SemaphoreHandle_t LastByteSignal_BinarySemaphore=NULL;

int main(void)
{
	KeypadMsgToPushButton_Queue=xQueueCreate(MAX_KEYPAD_MESSAGE,sizeof(uint8));
	KeypadMsgToUart_Queue=xQueueCreate(MAX_KEYPAD_MESSAGE,sizeof(uint8));
	ReceivedUartKeypadMsg_Queue=xQueueCreate(MAX_KEYPAD_MESSAGE,sizeof(uint8));
	Uart_BinarySemaphore=xSemaphoreCreateBinary();
	LastByteSignal_BinarySemaphore=xSemaphoreCreateBinary();

	xTaskCreate(Tasks_Init,"InitTasks",100,NULL,Init_Task_PRIORITY,NULL);
	xTaskCreate(KeyPad_Task,"keypad",200,NULL,KEYPAD_PRIORITY,NULL);
	/* Create task to scan if Lcd request had come */
	xTaskCreate(LcdCyclic_Task,"LcdCyclic",100,NULL,LCD_CYCLIC_PRIORITY,NULL);
	xTaskCreate(MonitorPushButton1_Task,"BTN",300,NULL,PUSH_BUTTON_PRIORITY,NULL);
	xTaskCreate(Uart_SendTask,"UART_TX",200,NULL,UART_SEND_PRIORITY,NULL);
	//xTaskCreate(LedToggle_200,"Led",100,NULL,1,&LedToggle_200_Handller);
	xTaskCreate(Uart_ReceiveTask,"UART_RX",200,NULL,2,NULL);

	/* Run Scheduler */
	vTaskStartScheduler();
	/* If all is well then main() will never reach here as the scheduler will now be
	running the tasks. If main() does reach here then it is likely that there was
	insufficient heap memory available for the idle task to be created. Chapter 2
	provides more information on heap memory management. */
	while(1);

	return 0;
}
