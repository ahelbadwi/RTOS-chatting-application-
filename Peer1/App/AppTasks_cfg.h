/*
 *  AppTasks_cfg.h
 *  Description : this file contains a Precomplie configuration parameters of Tasks.
 *  Created on: Jan 31, 2020
 *  Author: Ahemd_Elbadwi
 */

#ifndef APPTASKS_CFG_H_
#define APPTASKS_CFG_H_
/*************************************************************************/
/*                         #Defines                                      */
/*************************************************************************/
/* Tasks Priorities */
#define Init_Task_PRIORITY       6
#define KEYPAD_PRIORITY          4
#define LCD_CYCLIC_PRIORITY      3
#define PUSH_BUTTON_PRIORITY     5
#define UART_SEND_PRIORITY       4
#define UART_RECEIVE_PRIORITY    4
/* Tasks Peridicities */
#define LCD_CYCLIC_PERIODICITY   1
#define KEYPAD_PERIODICITY       4
#define PUSH_BUTTON1_PERIODICITY 5
#define UART_TX_TASK_PERIODICITY 1
#define UART_RX_TASK_PERIODICITY 1

#endif /* APPTASKS_CFG_H_ */
