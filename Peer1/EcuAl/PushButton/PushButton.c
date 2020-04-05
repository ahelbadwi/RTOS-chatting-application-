/*
 * PushButton.c
 *
 *  Created on: Dec 29, 2019
 *      Author: Ahemd_Elbadwi 
 */


/********************************************************************/
/*                        Includes                                  */
/********************************************************************/
#include "pushButtonConfig.h"
#include "PushButton.h"

/***********************************************************************/
/*                        Global Variables                             */
/***********************************************************************/
static uint8 gu8_btn_id=0;
static uint8 gu8_btnValue=LOW;
/**
 * Description: Initialize the BTN_x Pin state (where x 0, 1, 2, 3) to Input
 * @param btn_id: The btn to be initialized and it takes
 * 				  one of the enum (En_buttonId) parameters
 *
 */
ErrorStatus_t pushButton_Init(uint8 btn_id)
{
	ErrorStatus_t ERROR_STATUS_pushButton_Init=E_OK;
	DIO_Cfg_s DIO_BTN_Cfg;
	switch(btn_id)
	{
	case BTN_0:
		DIO_BTN_Cfg.GPIO=BTN_0_GPIO;
		DIO_BTN_Cfg.dir=INPUT;
		DIO_BTN_Cfg.pins=BTN_0_BIT;
		DIO_init(&DIO_BTN_Cfg);
		break;
	case BTN_1:
		DIO_BTN_Cfg.GPIO=BTN_1_GPIO;
		DIO_BTN_Cfg.dir=INPUT;
		DIO_BTN_Cfg.pins=BTN_1_BIT;
		DIO_init(&DIO_BTN_Cfg);
		break;
	default:
		ERROR_STATUS_pushButton_Init=E_NOK;
		break;
	}
	return ERROR_STATUS_pushButton_Init;
}


/**
 * Description: read all BTN_x (where x 0, 1, 2, 3) states and store it in the program
 *
 * @note : this function must be called in the system tick handler or in the super loop handler
 */

ErrorStatus_t pushButton_Update(void)
{
	ErrorStatus_t ERROR_STATUS_pushButton_Update=E_OK;
	switch(gu8_btn_id)
	{
	case BTN_0:
		DIO_Read(BTN_0_GPIO,BTN_0_BIT,&gu8_btnValue);
		break;
	case BTN_1:
		DIO_Read(BTN_1_GPIO,BTN_1_BIT,&gu8_btnValue);
		break;
	default:
		ERROR_STATUS_pushButton_Update=E_NOK;
		break;
	}
	return ERROR_STATUS_pushButton_Update;
}


ErrorStatus_t pushButton_GetStatus(uint8 btn_id,En_buttonStatus_t* En_buttonStatus)
{
	ErrorStatus_t ERROR_STATUS_pushButton_GetStatus=E_OK;
	gu8_btn_id=btn_id;
	/* button denouncing state machine */
	switch((*En_buttonStatus))
	{
	case Pressed:
		pushButton_Update();
		if(gu8_btnValue == LOW)
			*En_buttonStatus = Prereleased;
		break;

	case Prereleased:
		/* this task periodicity = 50msec so, we can handle bouncing by getting reading of
		 * button x and make sure it's  reading == released reading
		 */
		/* update buttons readings */
		pushButton_Update();

		if(gu8_btnValue == LOW)
		{
			*En_buttonStatus = Released;
		}
		else
		{
			/* take another sample in next task execution */
			*En_buttonStatus = Prereleased;
		}

		break;

	case Released:
		pushButton_Update();
		if(gu8_btnValue==HIGH)
			*En_buttonStatus = Prepressed;
		break;

	case Prepressed:
		/* this task periodicity = 50msec so, we can handle bouncing by getting reading of
		 * button x and make sure it's reading == pressed reading
		 */
		/* update buttons readings */
		pushButton_Update();
		if(gu8_btnValue == HIGH)
		{
			*En_buttonStatus = Pressed;
		}
		else
		{
			/* still button bouncing so take another sample in next task execution */
			*En_buttonStatus = Prepressed;
		}
		break;

	default:
		ERROR_STATUS_pushButton_GetStatus=E_NOK;
		break;
	}


	return ERROR_STATUS_pushButton_GetStatus;
}
