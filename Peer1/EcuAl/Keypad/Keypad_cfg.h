/*
 * Keypad_cfg.h
 *
 *  Created on: Jan 23, 2020
 *  Author: Ahemd_Elbadwi
 */

#ifndef KEYPAD_CFG_H_
#define KEYPAD_CFG_H_
/*************************************************************************/
/*                         Includes                                      */
/*************************************************************************/
#include "Keypad.h"
#include "DIO.h"
/*************************************************************************/
/*                         #Defines                                      */
/*************************************************************************/
#define ROWS_DATA_PORT         PORTC_DATA
#define ROWS_GPIO              GPIOC
#define ROWS_PIN_PORT          PORTC_PIN
#define COL_DATA_PORT          PORTC_DATA
#define COL_GPIO               GPIOC
#define COL_PIN_PORT           PORTC_PIN
#define ROW1_PIN               (PIN2)
#define ROW2_PIN               (PIN3)
#define ROW3_PIN               (PIN4)
#define COL1_PIN               (PIN5)
#define COL2_PIN               (PIN6)
#define COL3_PIN               (PIN7)

/*************************************************************************/
/*                         Exports                                       */
/*************************************************************************/
extern const uint8 gau8_KEYPAD_BUTTONS[3][3];

#endif /* KEYPAD_CFG_H_ */
