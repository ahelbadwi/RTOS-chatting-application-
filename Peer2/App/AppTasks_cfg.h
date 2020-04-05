/*
 * AppTasks_cfg.h
 *
 *  Created on: Feb 1, 2020
 *      Author: Ahemd_Elbadwi 
 */

#ifndef APPTASKS_CFG_H_
#define APPTASKS_CFG_H_

#define Init_Task_PRIORITY  5
#define KEYPAD_PRIORITY     4
#define LCD_CYCLIC_PRIORITY 3
#define PUSH_BUTTON_PRIORITY 2
#define UART_SEND_PRIORITY   4

#define LCD_CYCLIC_PERIODICITY 1
#define KEYPAD_PERIODICITY     4
#define PUSH_BUTTON1_PERIODICITY 5
#define LED_200_PERIODICITY   (200UL)
#endif /* APPTASKS_CFG_H_ */
