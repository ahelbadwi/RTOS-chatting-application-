/*
 *  File Named : Keypad.c
 *  Layer      : EcuAl
 *  Created on: Jan 23, 2020
 *  Author: Ahemd_Elbadwi
 */

/*************************************************************************/
/*                         Includes                                      */
/*************************************************************************/
#include "Keypad.h"
#include "Keypad_cfg.h"
#include "softwareDelay.h"
#include "Lcd.h"
/*************************************************************************/
/*                         Private #defines                              */
/*************************************************************************/

#define DEBOUNCING_SYS_TICK_TIME  (2U) /*  (2 system tick * peridicity) */
/*************************************************************************/
/*                         Static Global Variables                       */
/*************************************************************************/
static ErrorStatus_t gErrorStatus_keypad_Init;
static Keypad_state_t gen_Keypad_state;
static uint8 gu8_RowNumber;
static uint8 gu8_ColNumber;
static uint8 gu8_ColPin;
static uint8 gau8_BTN[2];
static uint8 gau8_BtnState[3][3]={
		{BTN_NOT_PRESSED,BTN_NOT_PRESSED,BTN_NOT_PRESSED},
		{BTN_NOT_PRESSED,BTN_NOT_PRESSED,BTN_NOT_PRESSED},
		{BTN_NOT_PRESSED,BTN_NOT_PRESSED,BTN_NOT_PRESSED}
};
/*************************************************************************/
/*                        Api.s  Definitions                             */
/*************************************************************************/
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

void keypad_Init(void)
{
	ErrorStatus_t ErrorStatus_InitRows=DIO_ERROR,ErrorStatus_InitCol=DIO_ERROR;
	/* Initialize Global variables */
	gErrorStatus_keypad_Init=KEYPAD_BASE_ERROR+DIO_ERROR;
	gen_Keypad_state=Keypad_Idle;
	gu8_RowNumber=NOT_VALID_ROW;
	gu8_ColNumber=NOT_VALID_INPUT;
	gu8_ColPin=NOT_VALID_PIN;
	gau8_BTN[BTN_INDEX]=NOT_VALID_BTN;
	gau8_BTN[NULL_INDEX]=NULL_CHAR;
	/* Keypad Initialization */
	/* rows initialization as O/P*/
	DIO_Cfg_s rows_cfg;
	rows_cfg.GPIO=ROWS_GPIO;
	rows_cfg.dir=OUTPUT;
	rows_cfg.pins=(ROW1_PIN|ROW2_PIN|ROW3_PIN);
	ErrorStatus_InitRows=DIO_init(&rows_cfg);
	/* columns initialization as I/P */
	DIO_Cfg_s col_cfg;
	col_cfg.GPIO=COL_GPIO;
	col_cfg.dir=INPUT;
	col_cfg.pins=(COL1_PIN|COL2_PIN|COL3_PIN);
	ErrorStatus_InitCol=DIO_init(&col_cfg);
	/* columns as PullUp */
	DIO_Write(COL_GPIO,col_cfg.pins,HIGH);
	if((SUCCESS == ErrorStatus_InitRows) && (SUCCESS == ErrorStatus_InitCol))
	{
		gErrorStatus_keypad_Init =KEYPAD_BASE_ERROR+SUCCESS;
	}
	else
	{
		/* Do Nothing */
	}
}


/*******************************************************************************
 * 	Function name: 		Keypad_GetBTN
 *
 * 	Description  :      Read keypad buttons
 *
 * 	Arguments:
 * 		-				NA
 * 	Return:
 * 		-				NA
 *******************************************************************************/

ErrorStatus_t Keypad_GetBTN(uint8* BTN)
{
	ErrorStatus_t ErrorStatus_Keypad_GetBTN=SUCCESS;
	uint8 u8_RowNumber;
	uint8 u8_ColNumber=NOT_VALID_INPUT;
	/* check keypad is initialized */
	if(SUCCESS == gErrorStatus_keypad_Init - KEYPAD_BASE_ERROR)
	{
		for(u8_RowNumber=ZERO_VALUE;u8_RowNumber<KEYPAD_ROWS_NUMBER;u8_RowNumber++)
		{
			//SwDelay_us(100);
			/* Set all rows HIGH */
			DIO_Write(ROWS_GPIO,(ROW1_PIN|ROW2_PIN|ROW3_PIN),HIGH);
			switch(u8_RowNumber)
			{
			/* Set Dedicated One Row Low */
			case ROW1:
				DIO_Write(ROWS_GPIO,ROW1_PIN,LOW);
				break;
			case ROW2:
				DIO_Write(ROWS_GPIO,ROW2_PIN,LOW);
				break;
			case ROW3:
				DIO_Write(ROWS_GPIO,ROW3_PIN,LOW);
				break;
			default:
				break;
			}
			/* Check Columns */
			if(GET_BIT_MASK(COL_PIN_PORT,COL1_PIN)==ZERO_VALUE)
			{
				u8_ColNumber=COL1;
			}
			else if(GET_BIT_MASK(COL_PIN_PORT,COL2_PIN)==ZERO_VALUE)
			{
				u8_ColNumber=COL2;
			}
			else if(GET_BIT_MASK(COL_PIN_PORT,COL3_PIN)==ZERO_VALUE)
			{
				u8_ColNumber=COL3;
			}
			else
			{
				/* DO Nothing */
			}

			if(u8_ColNumber != NOT_VALID_INPUT)
			{
				/* Handle Long Press : Busy wait till all BTNS Not Pressed */
				while(ANY_BTN == BTN_PRESSED);
				/* read BTN */
				*BTN=gau8_KEYPAD_BUTTONS[u8_RowNumber][u8_ColNumber];
				break;
			}
		}
	}
	else
	{
		ErrorStatus_Keypad_GetBTN=NOT_INITIALIZED_ERROR;
	}
	return (KEYPAD_BASE_ERROR+ErrorStatus_Keypad_GetBTN);
}


/*******************************************************************************
 * 	Function name: 		Keypad_cyclic
 *
 * 	Description  :      Keypad cyclic function that sos runs it according to
 * 	                    its Periodicity and check if any button pressed or not.
 *
 * 	Arguments:
 * 		-				NA
 * 	Return:
 * 		-				ERROR_STATUS to report success or failure
 *******************************************************************************/
ErrorStatus_t Keypad_cyclic_Getbutton(uint8* BTN,Keypad_state_t* Keypad_state)
{
	ErrorStatus_t ErrorStatus_Keypad_cyclic=SUCCESS;
	static uint8 u8_DebouncingCounter=ZERO_VALUE;
	/* Current state of keypad state machine */
	*Keypad_state=gen_Keypad_state;
	/* check keypad is initialized */
	if(SUCCESS == gErrorStatus_keypad_Init-KEYPAD_BASE_ERROR)
	{
		/* Keypad state machine */
		switch(gen_Keypad_state)
		{
		case Keypad_Idle:
			gu8_RowNumber=NOT_VALID_ROW;
			gu8_ColNumber=NOT_VALID_INPUT;
			gau8_BTN[BTN_INDEX]=NOT_VALID_BTN;
			gen_Keypad_state=KeyPad_Prepressed_checking;
			break;
		case KeyPad_Prepressed_checking:
			for(gu8_RowNumber=ZERO_VALUE;gu8_RowNumber<KEYPAD_ROWS_NUMBER;gu8_RowNumber++)
			{
				/* Set all rows HIGH */
				DIO_Write(ROWS_GPIO,(ROW1_PIN|ROW2_PIN|ROW3_PIN),HIGH);
				switch(gu8_RowNumber)
				{
				/* Set Dedicated One Row Low */
				case ROW1:
					DIO_Write(ROWS_GPIO,ROW1_PIN,LOW);
					break;
				case ROW2:
					DIO_Write(ROWS_GPIO,ROW2_PIN,LOW);
					break;
				case ROW3:
					DIO_Write(ROWS_GPIO,ROW3_PIN,LOW);
					break;
				default:
					break;
				}
				/* Check Columns */
				if(GET_BIT_MASK(COL_PIN_PORT,COL1_PIN)==BTN_PRESSED)
				{
					gu8_ColNumber=COL1;
					gu8_ColPin=COL1_PIN;
				}
				else if(GET_BIT_MASK(COL_PIN_PORT,COL2_PIN)==BTN_PRESSED)
				{
					gu8_ColNumber=COL2;
					gu8_ColPin=COL2_PIN;
				}
				else if(GET_BIT_MASK(COL_PIN_PORT,COL3_PIN)==BTN_PRESSED)
				{
					gu8_ColNumber=COL3;
					gu8_ColPin=COL3_PIN;
				}
				else
				{
					/* DO Nothing */
				}
				if(gu8_ColNumber != NOT_VALID_INPUT)
				{
					gau8_BtnState[gu8_RowNumber][gu8_ColNumber]=BTN_PRESSED;
					gen_Keypad_state=Keypad_Pressed_checking;
					break;
				}
				else
				{
					//gen_Keypad_state=Keypad_Idle;
				}
			}
			break;

		case Keypad_Pressed_checking:
			/* handle denouncing : Re check the the button state after denouncing time */
			u8_DebouncingCounter++;
			if(u8_DebouncingCounter < DEBOUNCING_SYS_TICK_TIME)
			{
				gen_Keypad_state=Keypad_Pressed_checking;
			}
			else
			{
				u8_DebouncingCounter=ZERO_VALUE;
				if(GET_BIT_MASK(COL_PIN_PORT,gu8_ColPin)==BTN_PRESSED)
				{
					/* So we can decide this button is in pressed state however we can read it now */
					gau8_BTN[BTN_INDEX]=gau8_KEYPAD_BUTTONS[gu8_RowNumber][gu8_ColNumber];
					gen_Keypad_state=Keypad_LongPress_Handling;
				}
				else
				{
					gen_Keypad_state=Keypad_Idle;
				}
			}
			break;

		case Keypad_LongPress_Handling:
			if(ANY_BTN==BTN_PRESSED)
			{
				gen_Keypad_state=Keypad_LongPress_Handling;
			}
			else
			{
				gen_Keypad_state=Keypad_end;
			}
			break;

		case Keypad_end:
			if(gau8_BTN[BTN_INDEX] != NOT_VALID_BTN)
			{
				/* Make LCD request to display this button */
				//LCD_Display_Request((char*)gau8_BTN);
				*BTN=gau8_BTN[0];
				gen_Keypad_state=Keypad_Idle;
			}
			else
			{
				/* Do Nothing */
			}
			break;
		}
	}
	else
	{
		ErrorStatus_Keypad_cyclic=NOT_INITIALIZED_ERROR;
	}
	return (KEYPAD_BASE_ERROR+ErrorStatus_Keypad_cyclic);
}
