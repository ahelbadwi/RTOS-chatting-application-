/*
 * Lcd.c
 *
 *  Created on: Jan 21, 2020
 *  Author: Ahemd_Elbadwi
 */
/*************************************************************************/
/*                         Includes                                      */
/*************************************************************************/
#include "Lcd.h"
#include "softwareDelay.h"


/*************************************************************************/
/*                         #Defines                                      */
/*************************************************************************/
#define FIRST_ROW_ADDRESS   (0x80)
#define SECOND_ROW_ADDRESS  (0xC0)

/*************************************************************************/
/*                         Static Global Variables                       */
/*************************************************************************/
static char* gpu8_Data;
static uint8 gu8_DataLength,gu8_Command;
static bool gb_LcdDataRequest_IsRunning;
static bool gb_LcdCommandRequest_IsRunning;
static bool gb_SendCommand_HighByte_Flag,gb_SendCommand_LowByte_Flag;
static bool gb_SendData_HighByte_Flag,gb_SendData_LowByte_Flag;
static bool gb_DisplayRequestIsDone,gb_CommandRequestIsDone;
static ErrorStatus_t gErrorStatus_LcdInit;
static Lcd_DisplayData_state_t gen_DisplayData_state;
static Lcd_SendCommand_state_t gen_SendCommand_state;
/*************************************************************************/
/*                       Static Function Definitions                     */
/*************************************************************************/

/*******************************************************************************
 * 	Function name: 		Get_DataLength
 *
 * 	Description  :      calculate length of data to be displayed
 *
 * 	Arguments:
 * 	   -               data:pointer to data
 * 	Return:
 * 		-			   u8_DataIndex:length of data to be displayed
 *******************************************************************************/

static uint8 Get_DataLength(char* data)
{
	uint8 u8_DataIndex=0;
	while(data[u8_DataIndex] != NULL_CHAR)
	{
		u8_DataIndex++;
	}
	return u8_DataIndex;
}
/*******************************************************************************
 * 	Function name: 		LCD_Enable
 *
 * 	Description  :      LCD Enable Mecanism
 *
 * 	Arguments:
 * 	   -               NA
 * 	Return:
 * 		-			   NA
 *******************************************************************************/

static void LCD_Enable(void)
{
	SET_BIT(LCD_PORT_COMMAND,E);    /*E=1*/
	SwDelay_us(100);
	CLEAR_BIT(LCD_PORT_COMMAND,E); /*E=0*/
	SwDelay_us(100);
}

/*******************************************************************************
 * 	Function name: 		LCD_WriteCommand
 *
 * 	Description  :      mechanism for sending command
 *
 * 	Arguments:
 * 	   -               command
 * 	Return:
 * 		-			   NA
 *******************************************************************************/

static void LCD_WriteCommand(uint8 command)
{
	CLEAR_BIT(LCD_PORT_COMMAND,RS);   /*Rs=0*/
	CLEAR_BIT(LCD_PORT_COMMAND,RW);  /*RW=0*/
#ifdef UPPER_PORT_PINS
	/* Send High byte */
	LCD_PORT_DATA=(LCD_PORT_DATA&0x0F) | (command&0xF0);
	LCD_Enable();
	//SwDelay_ms(2);
	/* Send Low byte */
	LCD_PORT_DATA=(LCD_PORT_DATA&0x0F)|((command&0x0F)<<4);
	LCD_Enable();
#else
	/* Send High byte */
	LCD_PORT_DATA=(LCD_PORT_DATA&0xF0) | ((command&0xF0)>>4);
	LCD_Enable();
	SwDelay_ms(2);
	/* Send Low byte */
	LCD_PORT_DATA = (LCD_PORT_DATA & 0xF0) |(command & 0x0F) ;
	LCD_Enable();
#endif
}

/*******************************************************************************
 * 	Function name: 		LCD_SendCommand_StateMachine
 *
 * 	Description  :      LCD state machine to send a command
 *
 * 	Arguments:
 * 	   -                NA
 * 	Return:
 * 		-				ERROR_STATUS to report success or failure
 *******************************************************************************/

static ErrorStatus_t LCD_SendCommand_StateMachine(void)
{
	ErrorStatus_t ErrorStatus_LCD_SendCommand_StateMachine=SUCCESS;
	switch(gen_SendCommand_state)
	{
	case Lcd_SendCommand_idle:
		gen_SendCommand_state=Lcd_SendCommand_EnableCommandMode;
		break;

	case Lcd_SendCommand_EnableCommandMode:
		/*Rs=0*/
		CLEAR_BIT(LCD_PORT_COMMAND,RS);
		/*RW=0*/
		CLEAR_BIT(LCD_PORT_COMMAND,RW);
		gen_SendCommand_state=Lcd_SendCommand_SendHighByte;
		break;

	case Lcd_SendCommand_SendHighByte:
#ifdef UPPER_PORT_PINS
		/* Send High byte */
		LCD_PORT_DATA=(LCD_PORT_DATA & 0x0F) | (gu8_Command & 0xF0);
#else
		/* Send High byte */
		LCD_PORT_DATA=(LCD_PORT_DATA & 0xF0) | ((gu8_Command & 0xF0)>>4);
#endif
		gb_SendCommand_HighByte_Flag=TRUE;
		gen_SendCommand_state=Lcd_SendCommand_EnableLcd;
		break;

	case Lcd_SendCommand_EnableLcd:
		LCD_Enable();
		if((TRUE==gb_SendCommand_HighByte_Flag) && (FALSE==gb_SendCommand_LowByte_Flag))
		{
			gen_SendCommand_state=Lcd_SendCommand_SendLowByte;
		}
		else if((TRUE==gb_SendCommand_HighByte_Flag) && (TRUE==gb_SendCommand_LowByte_Flag))
		{
			gen_SendCommand_state=Lcd_SendCommand_End;
		}
		else
		{
			/* Do Nothing */
		}
		break;

	case Lcd_SendCommand_SendLowByte:
#ifdef UPPER_PORT_PINS
		/* Send Low byte */
		LCD_PORT_DATA=(LCD_PORT_DATA & 0x0F)|((gu8_Command & 0x0F)<<4);
#else
		/* Send Low byte */
		LCD_PORT_DATA = (LCD_PORT_DATA & 0xF0) |(gu8_Command & 0x0F) ;
#endif
		gb_SendCommand_LowByte_Flag=TRUE;
		gen_SendCommand_state=Lcd_SendCommand_EnableLcd;
		break;

	case Lcd_SendCommand_End:
		gb_SendCommand_HighByte_Flag=FALSE;
		gb_SendCommand_LowByte_Flag=FALSE;
		gb_LcdCommandRequest_IsRunning=FALSE;
		/* Notify user sending command is done */
		gb_CommandRequestIsDone=DONE;
		gen_SendCommand_state=Lcd_SendCommand_idle;
		break;

	default:
		gen_SendCommand_state =Lcd_SendCommand_idle;
		gb_LcdCommandRequest_IsRunning=FALSE;
		break;
	}
	return (LCD_BASE_ERROR + ErrorStatus_LCD_SendCommand_StateMachine);
}

/*******************************************************************************
 * 	Function name: 		LCD_DisplayData_StateMachine
 *
 * 	Description  :      LCD state machine to Display data on LCD
 *
 * 	Arguments:
 * 	   -                NA
 * 	Return:
 * 		-				ERROR_STATUS to report success or failure
 *******************************************************************************/

static ErrorStatus_t LCD_DisplayData_StateMachine(void)
{
	ErrorStatus_t ErrorStatus_LCD_DisplayData_StateMachine=SUCCESS;
	static uint8 u8_DataIndex=0;
	switch(gen_DisplayData_state)
	{
	case Lcd_DisplayData_Idle:
		gen_DisplayData_state=Lcd_DisplayData_EnableDataMode;
		break;

	case Lcd_DisplayData_EnableDataMode:
		SET_BIT(LCD_PORT_COMMAND,RS);   /*Rs=1*/
		CLEAR_BIT(LCD_PORT_COMMAND,RW);  /*RW=0*/
		gen_DisplayData_state=Lcd_DisplayData_SendHighByte;
		break;

	case Lcd_DisplayData_SendHighByte:
#ifdef UPPER_PORT_PINS
		/* Send High byte */
		LCD_PORT_DATA=(LCD_PORT_DATA & 0x0F) | (gpu8_Data[u8_DataIndex] & 0xF0);
#else
		/* Send High byte */
		LCD_PORT_DATA=(LCD_PORT_DATA & 0xF0) | ((gpu8_Data[u8_DataIndex] & 0xF0)>>4);
#endif
		gb_SendData_HighByte_Flag=TRUE;
		gen_DisplayData_state=Lcd_DisplayData_EnableLcd;
		break;

	case Lcd_DisplayData_EnableLcd:
		LCD_Enable();
		if((TRUE==gb_SendData_HighByte_Flag) && (FALSE==gb_SendData_LowByte_Flag))
		{
			gen_DisplayData_state=Lcd_DisplayData_SendLowByte;
		}
		else if((TRUE==gb_SendData_HighByte_Flag) && (TRUE==gb_SendData_LowByte_Flag))
		{
			gen_DisplayData_state=Lcd_DisplayData_End;
		}
		else
		{
			/* Do Nothing */
		}
		break;

	case Lcd_DisplayData_SendLowByte:
#ifdef UPPER_PORT_PINS
		/* Send Low byte */
		LCD_PORT_DATA=(LCD_PORT_DATA&0x0F)|((gpu8_Data[u8_DataIndex]&0x0F)<<4);
#else
		/* Send Low byte */
		LCD_PORT_DATA = (LCD_PORT_DATA & 0xF0) |(gpu8_Data[u8_DataIndex] & 0x0F) ;
#endif
		gb_SendData_LowByte_Flag=TRUE;
		gen_DisplayData_state=Lcd_SendCommand_EnableLcd;
		break;

	case Lcd_DisplayData_End:
		/* Increment u8_DataIndex to send next byte if found */
		u8_DataIndex++;
		/* check if there is more than one byte of data */
		if(u8_DataIndex >= gu8_DataLength)
		{
			u8_DataIndex=ZERO_VALUE;
			gb_LcdDataRequest_IsRunning=FALSE;
			/* Notify User the Display Request Is Done*/
			gb_DisplayRequestIsDone=DONE;
			gen_DisplayData_state=Lcd_DisplayData_Idle;
		}
		else
		{
			/* Send Next Byte of data */
			gen_DisplayData_state=Lcd_DisplayData_SendHighByte;
		}
		gb_SendData_HighByte_Flag=FALSE;
		gb_SendData_LowByte_Flag=FALSE;
		break;

	default:
		gen_DisplayData_state=Lcd_DisplayData_Idle;
		gb_LcdDataRequest_IsRunning=FALSE;
		break;
	}

	return (LCD_BASE_ERROR +ErrorStatus_LCD_DisplayData_StateMachine);
}


/*************************************************************************/
/*                        Api.s  Definitions                             */
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

void LCD_Init(void)
{
	/* Initialize Global Variables */
	gpu8_Data=NULL;
	gu8_Command=ZERO_VALUE;
	gu8_DataLength=ZERO_VALUE;
	gb_LcdDataRequest_IsRunning=FALSE;
	gb_LcdCommandRequest_IsRunning=FALSE;
	gb_DisplayRequestIsDone=NOT_DONE;
	gb_CommandRequestIsDone=NOT_DONE;
	gb_SendCommand_HighByte_Flag=FALSE;
	gb_SendCommand_LowByte_Flag=FALSE;
	gb_SendData_HighByte_Flag=FALSE;
	gb_SendData_LowByte_Flag=FALSE;
	gErrorStatus_LcdInit=LCD_BASE_ERROR+SUCCESS;
	gen_DisplayData_state=Lcd_DisplayData_Idle;
	gen_SendCommand_state=Lcd_SendCommand_idle;
	/* Init Lcd Pin Configuration  */
	SET_BIT(LCD_DDR_COMMAND,RS);   /*Rs pin o/p */
	SET_BIT(LCD_DDR_COMMAND,RW);  /*RW pin o/p */
	SET_BIT(LCD_DDR_COMMAND,E);  /*E pin */
#ifdef UPPER_PORT_PINS
	LCD_DDR_DATA=(LCD_DDR_DATA&0x0F)|(0xF0);           /*D7:D4 DATA BUS which i care */
#else
	LCD_DDR_DATA=(LCD_DDR_DATA&0xF0)|(0x0F);
#endif
	/* LCD Initialization Commands */
	FOUR_BITS_CONFIGURATION();
	LCD_WriteCommand(CLEAR_COMMAND); /*CLEAR_COMMAND */
	SwDelay_ms(2);
	LCD_WriteCommand(CURSOR_OFF); /* CURSOR_OFF */
}

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
bool LCD_Display_Request(char* data)
{
	bool b_retstate=NOT_ACCEPTED;
	/* don't accept request till make sure there is any request running */
	if((FALSE == gb_LcdDataRequest_IsRunning)&&(FALSE == gb_LcdCommandRequest_IsRunning))
	{
		b_retstate=ACCEPTED;
		gb_LcdDataRequest_IsRunning=TRUE;
		/* Enter display data state machine */
		gen_DisplayData_state=Lcd_DisplayData_EnableDataMode;
		gpu8_Data=data;
		gu8_DataLength=Get_DataLength(data);
		gb_DisplayRequestIsDone=NOT_DONE;
	}
	else
	{
		/* Do Nothing */
	}

	return b_retstate;
}

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

bool LCD_Command_Request(uint8 command)
{
	bool b_retstate=NOT_ACCEPTED;
	/* check if command is running */
	if((FALSE == gb_LcdCommandRequest_IsRunning)&&(FALSE==gb_LcdDataRequest_IsRunning))
	{
		b_retstate=ACCEPTED;
		gb_LcdCommandRequest_IsRunning=TRUE;
		/* Enter Send Command state machine */
		gen_SendCommand_state=Lcd_SendCommand_EnableCommandMode;
		gu8_Command=command;
		gb_CommandRequestIsDone=NOT_DONE;
	}
	else
	{
		/* Do Nothing */
	}

	return b_retstate;
}


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
ErrorStatus_t LCD_Cyclic(void)
{
	ErrorStatus_t ErrorStatus_LCD_Cyclic=SUCCESS;
	if(SUCCESS == gErrorStatus_LcdInit-LCD_BASE_ERROR)
	{
		/* check which request is set */
		if(TRUE == gb_LcdCommandRequest_IsRunning)
		{
			/* LCD Command state machine */
			LCD_SendCommand_StateMachine();
		}

		else if(TRUE == gb_LcdDataRequest_IsRunning)
		{
			/* LCD Display data state machine */
			LCD_DisplayData_StateMachine();
		}
		else
		{
			/* Do Nothing */
		}
	}
	else
	{
		ErrorStatus_LCD_Cyclic=NOT_INITIALIZED_ERROR;
	}
	return (LCD_BASE_ERROR+ErrorStatus_LCD_Cyclic);
}




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

bool LCD_IsSendCommandDone(void)
{
	bool b_retflag=gb_CommandRequestIsDone;
	return b_retflag;
}

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
bool  LCD_IsDisplayDataDone(void)
{
	bool b_retflag=gb_DisplayRequestIsDone;
	return b_retflag;
}

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
void LCD_gotoxy(uint8 x,uint8 y)  /* x=no of row ___ y=no of column */
{
	uint8 au8_RowAdd[]={FIRST_ROW_ADDRESS,SECOND_ROW_ADDRESS};   /*first row 0x80:0x8f ___ second row 0xc0:0xcf*/
	//LCD_Command_Request(au8_RowAdd[x-1]+(y-1));  /* rows=1:2 */
	LCD_WriteCommand(au8_RowAdd[x-1]+(y-1));  /* rows=1:2 */

}


/* Test */
void LCD_WriteChar(uint8 data)
{
	SET_BIT(LCD_PORT_COMMAND,RS);   /*Rs=1*/
	CLEAR_BIT(LCD_PORT_COMMAND,RW);  /*RW=0*/
#ifdef UPPER_PORT_PINS
	/* Send High byte */
	LCD_PORT_DATA=(LCD_PORT_DATA&0x0F) | (data&0xF0);
	LCD_Enable();
	//SwDelay_ms(2);
	/* Send Low byte */
	LCD_PORT_DATA=(LCD_PORT_DATA&0x0F)|((data&0x0F)<<4);
	LCD_Enable();
#else
	/* Send High byte */
	LCD_PORT_DATA=(LCD_PORT_DATA&0xF0) | ((data&0xF0)>>4);
	LCD_Enable();
	_delay_ms(2);
	/* Send Low byte */
	LCD_PORT_DATA = (LCD_PORT_DATA & 0xF0) |(data & 0x0F) ;
	LCD_Enable();
#endif
}

void LCD_PrintData(const char * str_ptr )
{
	uint8 i=0;
	while(str_ptr[i] !='\0')
	{
		LCD_WriteChar((uint8)str_ptr[i]);
		i++;
	}
}

void LCD_Clear_ROW(uint8 row_no)
{
	switch(row_no)
	{
	case FIRST_ROW:
		LCD_WriteCommand(0x80);
		LCD_PrintData("                ");
		LCD_WriteCommand(0x80);
		break;
	case SECOND_ROW:
		LCD_WriteCommand(0xC0);
		LCD_PrintData("                ");
		LCD_WriteCommand(0xC0);
		break;
	default:
		break;
	}
}

void LCD_Print_HEX(uint8 hex)
{
	uint8 second_digit;
	uint8 first_digit;

	second_digit =GET_HIGH_NIBBLE(hex) ;
	LCD_gotoxy(2,9);

	if(second_digit<=9)
		LCD_WriteChar(48+second_digit); //transform digit to its ascii
	else   //(A:F)
		LCD_WriteChar(55+second_digit); //transform digit to its ascii

	first_digit=GET_LOW_NIBBLE(hex);
	LCD_gotoxy(2,10);

	if(first_digit<=9)
		LCD_WriteChar(48+first_digit);
	else
		LCD_WriteChar(55+first_digit);
}

