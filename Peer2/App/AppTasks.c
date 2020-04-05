/*
 * File Name  : Tasks.c
 * Layer      : Application
 * Description: this file contains Tasks Definitions
 * Created on: Jan 31, 2020
 * Author: Ahemd_Elbadwi
 */


/*************************************************************************/
/*                         Includes                                      */
/*************************************************************************/
#include "UART.h"
#include "PushButton.h"
#include "LED.h"
#include "Lcd.h"
#include "Keypad.h"
#include "FreeRtos.h"
#include "semphr.h"
#include "AppTasks.h"
/*************************************************************************/
/*                          CBK Definitions                              */
/*************************************************************************/
TaskHandle_t LedToggle_200_Handller=NULL;

ErrorStatus_t UART_TxcompleteCBK(void)
{
	extern SemaphoreHandle_t Uart_BinarySemaphore;
	ErrorStatus_t ErrorStatus_Tx_complete_CBK=SUCCESS;
	BaseType_t xHigherPriorityTaskWoken= pdTRUE;
	/* Give Uart_BinarySemaphore to UART send task to complete sending the keypad message */
	xSemaphoreGiveFromISR( Uart_BinarySemaphore, &xHigherPriorityTaskWoken );
	return ErrorStatus_Tx_complete_CBK;
}

ErrorStatus_t UART_RxcompleteCBK(void)
{
	extern QueueHandle_t ReceivedUartKeypadMsg_Queue;
	ErrorStatus_t ErrorStatus_Rx_complete_CBK=SUCCESS;
	static uint8 ByteCounter=ZERO_VALUE;
	static uint8 au8_receivedKeypadMsg[MAX_KEYPAD_MESSAGE];
	/* Receive UART byte */
	UART_ReceiveByte(&au8_receivedKeypadMsg[ByteCounter]);
	/* Send received Keypad message to UART Receive task through KeypadMsgToUart_Queue */
	if(pdPASS == xQueueSendToBack(ReceivedUartKeypadMsg_Queue,&au8_receivedKeypadMsg[ByteCounter],0))
	{
		ByteCounter++;
	}
	else
	{
		/* Do Nothing */
	}
	return ErrorStatus_Rx_complete_CBK;
}

/*************************************************************************/
/*                         Tasks Definitions                             */
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
void Tasks_Init(void *pvParameters)
{
	/* Configure and Initialize UART */
	UART_cfg_s UART_cfg_Init;
	UART_cfg_Init.DataRegisterEmpty_Cbk_ptr=NULL;
	UART_cfg_Init.Receive_Cbk_ptr=UART_RxcompleteCBK;
	UART_cfg_Init.Transmit_Cbk_ptr=UART_TxcompleteCBK;
	UART_cfg_Init.u32_BaudRate=9600;
	UART_cfg_Init.u8_DataSize=UART_8_BIT;
	UART_cfg_Init.u8_DesiredOperation=TRANSCEIVER;
	UART_cfg_Init.u8_DoubleSpeed=UART_NO_DOUBLE_SPEED;
	UART_cfg_Init.u8_InterruptMode=UART_INTERRUPT;
	UART_cfg_Init.u8_ParityBit=UART_NO_PARITY;
	UART_cfg_Init.u8_StopBit=UART_ONE_STOP_BIT;
	UART_Init(&UART_cfg_Init);
	/* Push button_1 initialization */
	pushButton_Init(BTN_1);
	/* Led Initialization */
	Led_Init(LED_0);
	/* Lcd initialization */
	LCD_Init();
	/* Keypad initialization */
	keypad_Init();
	/* one time execution = suspend task after first execution */
	vTaskSuspend(NULL);
}


void KeyPad_Task(void *pvParameters)
{
	extern QueueHandle_t KeypadMsgToPushButton_Queue;

	static uint8 u8_BtnCounter=ZERO_VALUE;
	uint8 au8_Btn[2]={ZERO_VALUE,NULL_CHAR};
	Keypad_state_t Keypad_state=Keypad_Idle;
	const TickType_t xTask_periodicity=pdMS_TO_TICKS(KEYPAD_PERIODICITY);
	/* The xLastWakeTime variable needs to be initialized with the current tick count */
	TickType_t xLastWakeTime=xTaskGetTickCount();
	while(1)
	{
		/* get keypad button if pressed */
		Keypad_cyclic_Getbutton(&au8_Btn[ZERO_VALUE],&Keypad_state);
		if(Keypad_state == Keypad_end)
		{
			u8_BtnCounter++;
			/* Handling if user enter more than MAX LCD ROW Capacity */
			if(u8_BtnCounter < MAX_KEYPAD_MESSAGE)
			{
				/* Send a request to LCD to display pressed button */
				LCD_Display_Request((char*)au8_Btn);
				/* Send pressed button through a queue to Pushbutton task */
				xQueueSendToBack(KeypadMsgToPushButton_Queue,&au8_Btn[ZERO_VALUE],5);
			}
			else
			{
				u8_BtnCounter=ZERO_VALUE;
			}

		}
		else
		{
			/* Do Nothing */
		}
		/* send this tasks to the blocking state till xTask_periodicity */
		vTaskDelayUntil(&xLastWakeTime,xTask_periodicity);
	}
}

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
void LcdCyclic_Task(void *pvParameter)
{

	const TickType_t xTask_periodicity=LCD_CYCLIC_PERIODICITY;
	/* The xLastWakeTime variable needs to be initialized with the current tick count */
	TickType_t xLastWakeTime=xTaskGetTickCount();
	while(1)
	{
		LCD_Cyclic();
		vTaskDelayUntil(&xLastWakeTime,xTask_periodicity);
	}
}

/*******************************************************************************
 * 	Function name: 		MonitorPushButton1_Task
 *
 * 	Description  :      Get status of BTN 1 and send it periodically through a Queue
 * 	                    to LedSwitch_Task and LcdRequest_Task .
 *
 * 	Arguments:
 * 		-				pvParameters:void pointer to argument(Not used)
 *
 * 	Return:
 * 		-				NA
 *******************************************************************************/
void MonitorPushButton1_Task(void *pvParameter)
{
	extern QueueHandle_t KeypadMsgToPushButton_Queue;
	extern QueueHandle_t KeypadMsgToUart_Queue;
	extern SemaphoreHandle_t Uart_BinarySemaphore;

	static bool b_LocalPressedFlag=FALSE;
	static uint8 au8_receivedKeypadMsg[MAX_KEYPAD_MESSAGE];
	static uint8 KeypadBtnCounter1=0,KeypadBtnCounter2=0;
	En_buttonStatus_t BTN1_status=Released;
	const TickType_t xTask_periodicity=PUSH_BUTTON1_PERIODICITY;
	/* The xLastWakeTime variable needs to be initialized with the current tick count */
	TickType_t xLastWakeTime=xTaskGetTickCount();
	while(1)
	{
		/* receive pressed keys from keypad task */
		if(pdPASS == xQueueReceive(KeypadMsgToPushButton_Queue,&au8_receivedKeypadMsg[KeypadBtnCounter1],10))
		{
			KeypadBtnCounter1++;
		}
		else
		{
			/* Do Nothing */
		}

		/* get status of push button */
		pushButton_GetStatus(BTN_1,&BTN1_status);
		/* take action after releasing button */
		if(Prereleased == BTN1_status)
		{
			b_LocalPressedFlag=TRUE;
			/* add NULL character at the end of received Keypad message. */
			au8_receivedKeypadMsg[KeypadBtnCounter1]=NULL_CHAR;

		}
		else
		{
			/* Do Nothing */
		}

		if(TRUE == b_LocalPressedFlag)
		{
			/* Send received Keypad message to UART task through KeypadMsgToUart_Queue */
			if(pdPASS == xQueueSendToBack(KeypadMsgToUart_Queue,&au8_receivedKeypadMsg[KeypadBtnCounter2],0))
			{
				KeypadBtnCounter2++;
			}
			else
			{
				/* Do Nothing */
			}

			/* Check if send is complete */
			if(KeypadBtnCounter2 == KeypadBtnCounter1)
			{
				/* Send is complete which means,we can give a (signal) semaphore to UART Send task
				 * to start sending the keypad message.
				 */
				/* Give binary semaphore to UART task to send */
				if(xSemaphoreGive(Uart_BinarySemaphore))
					TOGGLE_BIT(PORTB_DATA,5);

				/* Reset Counters for next keypad message */
				KeypadBtnCounter2=0;
				KeypadBtnCounter1=0;
				b_LocalPressedFlag=FALSE;
			}
			else
			{
				/* Do Nothing */

			}

		}

		vTaskDelayUntil(&xLastWakeTime,xTask_periodicity);
	}
}

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

void Uart_SendTask(void *pvParameter)
{
	extern QueueHandle_t KeypadMsgToUart_Queue;
	extern SemaphoreHandle_t Uart_BinarySemaphore;


	static uint8 u8_receivedMsgCounter1=ZERO_VALUE,u8_receivedMsgCounter2=ZERO_VALUE;
	static uint8 au8_receivedKeypadMsg[MAX_KEYPAD_MESSAGE];
	const TickType_t xTask_periodicity=5;
	/* The xLastWakeTime variable needs to be initialized with the current tick count */
	TickType_t xLastWakeTime=xTaskGetTickCount();
	while(1)
	{
		/* Receive keypad message once the push button is pressed */
		if(pdPASS == xQueueReceive(KeypadMsgToUart_Queue,&au8_receivedKeypadMsg[u8_receivedMsgCounter1],5))
		{
			u8_receivedMsgCounter1++;
		}
		else
		{
			/* Do Nothing */
		}
		/* Take the semaphore if push button is pressed */
		if(xSemaphoreTake( Uart_BinarySemaphore, ( TickType_t ) 5 ))
		{
			if(u8_receivedMsgCounter2 < u8_receivedMsgCounter1)
			{

				/* send the received keypad message */
				UART_SendByte(au8_receivedKeypadMsg[u8_receivedMsgCounter2++]);
			}
			else
			{
				/* so, send is complete.
				 */
				/* Clear First Row of LCD */
				LCD_Clear_ROW(FIRST_ROW);
				/* LED_0 Turned ON for 200 MSE */
				Led_On(LED_0);
				vTaskDelay(200);
				Led_Off(LED_0);

				/* Reset the counters for the next messages */
				u8_receivedMsgCounter1=ZERO_VALUE;
				u8_receivedMsgCounter2=ZERO_VALUE;
			}
		}
		else
		{
			/* Do Nothing */
		}
	}
	vTaskDelayUntil(&xLastWakeTime,xTask_periodicity);
}


void Uart_ReceiveTask(void *pvParameter)
{
	extern QueueHandle_t ReceivedUartKeypadMsg_Queue;

	static uint8 u8_receivedMsgCounter1=ZERO_VALUE;
	static uint8 au8_receivedKeypadMsg[MAX_KEYPAD_MESSAGE];
	const TickType_t xTask_periodicity=5;
	/* The xLastWakeTime variable needs to be initialized with the current tick count */
	TickType_t xLastWakeTime=xTaskGetTickCount();
	while(1)
	{
		/* Receive keypad message once the push button is pressed */
		if(pdPASS == xQueueReceive(ReceivedUartKeypadMsg_Queue,&au8_receivedKeypadMsg[u8_receivedMsgCounter1],5))
		{
			if(au8_receivedKeypadMsg[u8_receivedMsgCounter1]==NULL_CHAR)
			{
				LCD_gotoxy(2,1);
				LCD_Display_Request((char*)au8_receivedKeypadMsg);
			}
			else
			{
				u8_receivedMsgCounter1++;
			}
		}
		else
		{
			/* Do Nothing */
		}
		vTaskDelayUntil(&xLastWakeTime,xTask_periodicity);
	}
}


