/*
 * Keypad.h
 *
 *  Created on: Jan 23, 2020
 *      Author: Ahemd_Elbadwi 
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

/*************************************************************************/
/*                         Includes                                      */
/*************************************************************************/
#include "std_types.h"
#include "Utils.h"
#include "General_Macros.h"
#include "Retval.h"
#include "DIO.h"
#include "Keypad_cfg.h"
/*************************************************************************/
/*                         #defines                                      */
/*************************************************************************/
#define KEYPAD_ROWS_NUMBER        (3U)
#define KEYPAD_COLUMNS_NUMBER     (3U)
#define ROW1                      (0U)
#define ROW2                      (1U)
#define ROW3                      (2U)
#define COL1                      (0U)
#define COL2                      (1U)
#define COL3                      (2U)

#define ANY_BTN                   (GET_BIT_MASK(COL_PIN_PORT,COL1_PIN) && \
		                          GET_BIT_MASK(COL_PIN_PORT,COL2_PIN) && \
		                          GET_BIT_MASK(COL_PIN_PORT,COL3_PIN)) \


#define BTN_PRESSED              (0)
#define BTN_NOT_PRESSED          (1)
#define NOT_VALID_INPUT          (200U)
#define NOT_VALID_ROW            (50U)
#define NOT_VALID_BTN            (20)
#define NOT_VALID_PIN            (8)
#define BTN_INDEX                (0)
#define NULL_INDEX               (1)
/*************************************************************************/
/*                              Typedef                                  */
/*************************************************************************/

/* enum of Keypad cyclic state machine */
typedef enum Keypad_state_t
{
	Keypad_Idle,
	KeyPad_Prepressed_checking,
	Keypad_Pressed_checking,
	Keypad_LongPress_Handling,
	Keypad_end
}Keypad_state_t;
/*******************************************************************************
 * 	Function name: 		keypad_Init
 *
 * 	Description  :      Initialize Global variables and keypad pins
 *
 * 	Arguments:
 * 		-				NA
 * 	Return:
 * 		-				NA
 *******************************************************************************/
extern void keypad_Init(void);
/*******************************************************************************
 * 	Function name: 		keypad_Init
 *
 * 	Description  :      Initialize Global variables and keypad pins
 *
 * 	Arguments:
 * 		-				NA
 * 	Return:
 * 		-				NA
 *******************************************************************************/
extern ErrorStatus_t Keypad_cyclic_Getbutton(uint8* BTN,Keypad_state_t* Keypad_state);



#endif /* KEYPAD_H_ */
