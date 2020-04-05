/*
 * Lcd.h
 *
 *  Created on: Jan 21, 2020
 *      Author: Ahemd_Elbadwi 
 */

#ifndef LCD_H_
#define LCD_H_
/*************************************************************************/
/*                         Includes                                      */
/*************************************************************************/
#include "std_types.h"
#include "General_Macros.h"
#include "Utils.h"
#include "DIO.h"
#include "Lcd_cfg.h"

/*************************************************************************/
/*                         #Defines                                      */
/*************************************************************************/
#define DONE          TRUE
#define NOT_DONE      FALSE
#define ACCEPTED      TRUE
#define NOT_ACCEPTED  FALSE
#define FIRST_ROW      1
#define SECOND_ROW     2
/* commands */
#define FOUR_BITS_CONFIGURATION()      do{\
	                                  LCD_WriteCommand(0x33);\
	                                  LCD_WriteCommand(0x32);\
	                                  LCD_WriteCommand(0x28);\
                                      }while(0)

#define CLEAR_COMMAND                (0x01)
#define FOUR_BITS_DATA_MODE          (0x02)
#define TWO_LINE_LCD_Four_BIT_MODE   (0x28)
#define CURSOR_OFF                   (0x0C)
#define CURSOR_ON                    (0x0E)
#define SET_CURSOR_LOCATION          (0x80)


/*************************************************************************/
/*                         Typedefs                                      */
/*************************************************************************/
/* state machine to display data on lcd */
typedef enum Lcd_DisplayData_state_t
{
	Lcd_DisplayData_Idle,
	Lcd_DisplayData_EnableDataMode,
	Lcd_DisplayData_SendHighByte,
	Lcd_DisplayData_EnableLcd,
	Lcd_DisplayData_SendLowByte,
	Lcd_DisplayData_End
}Lcd_DisplayData_state_t;

/* state machine to send command on lcd */
typedef enum Lcd_SendCommand_state_t
{
   Lcd_SendCommand_idle,
   Lcd_SendCommand_EnableCommandMode,
   Lcd_SendCommand_SendHighByte,
   Lcd_SendCommand_EnableLcd,
   Lcd_SendCommand_SendLowByte,
   Lcd_SendCommand_End
}Lcd_SendCommand_state_t;

/*************************************************************************/
/*                          Exports                                      */
/*************************************************************************/

/*******************************************************************************
 * 	Function name: 		LCD_Init
 *
 * 	Description  :     Initialize Global variables and LCD (4 bit mode)
 *
 * 	Arguments:
 * 		-				NA
 * 	Return:
 * 		-				NA
 *******************************************************************************/
extern void LCD_Init(void);
/*******************************************************************************
 * 	Function name: 		LCD_Display_Request
 *
 * 	Description  :     Notify Sos that there is a request to display data
 *
 * 	Arguments:
 * 		-				data  : data to be displayed
 * 		                length: data length
 * 	Return:
 * 		-				b_retstate : LCD_Display_Request is accepted or not
 *******************************************************************************/
extern bool LCD_Display_Request(char* data);
/*******************************************************************************
 * 	Function name: 		LCD_Command_Request
 *
 * 	Description  :      Notify Sos that there is a request to send a command
 *
 * 	Arguments:
 * 		-				command  : command to be sent
 * 	Return:
 * 		-				b_retstate : LCD_Command_Request is accepted or not
 *******************************************************************************/
extern bool LCD_Command_Request(uint8 command);
/*******************************************************************************
 * 	Function name: 		LCD_Cyclic
 *
 * 	Description  :     Check if any request to display data or send command
 * 	                   and Choose which state machine to run through this function.
 * 	                   and this done by Sos Run (scheduler).
 *
 * 	Arguments:
 * 	   -                NA
 * 	Return:
 * 		-				ERROR_STATUS to report success or failure
 *******************************************************************************/
extern ErrorStatus_t LCD_Cyclic(void);

/*************************************************************************************
 * 	Function name: 		LCD_IsSendCommandDone
 *
 * 	Description  :      Notify User by the availability to send another command to be
 * 	                    Handled by getting the state of current request.
 *
 * 	Arguments:
 * 	   -                NA
 * 	Return:
 * 		-				b_retflag : Notification if the command request is done or not
 *************************************************************************************/
extern bool LCD_IsSendCommandDone(void);
/**************************************************************************************
 * 	Function name: 		LCD_IsDisplayDataDone
 *
 * 	Description  :      Notify User by the availability to send another data to be
 * 	                    displayed by getting the state of current request.
 *
 * 	Arguments:
 * 	   -                NA
 * 	Return:
 * 		-				b_retflag : Notification if the display request is done or not
 *************************************************************************************/
extern bool  LCD_IsDisplayDataDone(void);
/*************************************************************************************
 * 	Function name: 		LCD_gotoxy
 *
 * 	Description  :      Move LCD Cursor to certain position (x,y)
 *
 * 	Arguments:
 * 	   -                x:row number(1:2)
 * 	   -                y:col number(1:16)
 * 	Return:
 * 	   -                NA
 *************************************************************************************/
extern void LCD_gotoxy(uint8 x,uint8 y);
extern void LCD_WriteChar(uint8 data);
extern void LCD_PrintData(const char * str_ptr );
extern void LCD_Print_HEX(uint8 hex);
extern void LCD_Clear_ROW(uint8 row_no);
#endif /* LCD_H_ */
