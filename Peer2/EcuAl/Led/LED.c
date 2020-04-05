/*
 * LED.c
 *
 *  Created on: 12/1/2019
 *      Author: Sherif Adel
 */
#include "DIO.h"
#include "ledConfig.h"
#include "LED.h"

/**
 * Description: Initialize the LED_x (where x 0, 1, 2, 3) as output
 * @param led_id: The led to be initialized .
 *
 */
void Led_Init(uint8 led_id)
{
	DIO_Cfg_s LedCfg;
	switch(led_id)
	{
	case LED_0:
		LedCfg.GPIO=LED_0_GPIO;
		LedCfg.dir=OUTPUT;
		LedCfg.pins=LED_0_BIT;
		DIO_init(&LedCfg);
		break;
	case LED_1:
		LedCfg.GPIO=LED_1_GPIO;
		LedCfg.dir=OUTPUT;
		LedCfg.pins=LED_1_BIT;
		DIO_init(&LedCfg);
		break;
	case LED_2:
		LedCfg.GPIO=LED_2_GPIO;
		LedCfg.dir=OUTPUT;
		LedCfg.pins=LED_2_BIT;
		DIO_init(&LedCfg);
		break;
	case LED_3:
		LedCfg.GPIO=LED_3_GPIO;
		LedCfg.dir=OUTPUT;
		LedCfg.pins=LED_3_BIT;
		DIO_init(&LedCfg);
		break;
	}

}

/**
 * Description: Change the LED_x state (where x 0, 1, 2, 3) to be on
 * @param led_id: The led to be ON.
 *
 */
void Led_On(uint8 led_id)
{
	switch(led_id)
	{
	case LED_0:
		DIO_Write(LED_0_GPIO,LED_0_BIT,HIGH);
		break;
	case LED_1:
		DIO_Write(LED_1_GPIO,LED_1_BIT,HIGH);
		break;
	case LED_2:
		DIO_Write(LED_2_GPIO,LED_2_BIT,HIGH);
		break;
	case LED_3:
		DIO_Write(LED_3_GPIO,LED_3_BIT,HIGH);
		break;
	}
}


/**
 * Description: Change the LED_x state (where x 0, 1, 2, 3) to be off
 * @param led_id: The led to OFF.
 *
 */
void Led_Off(uint8 led_id)
{
	switch(led_id)
	{
	case LED_0:
		DIO_Write(LED_0_GPIO,LED_0_BIT,LOW);
		break;
	case LED_1:
		DIO_Write(LED_1_GPIO,LED_1_BIT,LOW);
		break;
	case LED_2:
		DIO_Write(LED_2_GPIO,LED_2_BIT,LOW);
		break;
	case LED_3:
		DIO_Write(LED_3_GPIO,LED_3_BIT,LOW);
		break;
	}
}


/**
 * Description: Toggle the LED_x state (where x 0, 1, 2, 3)
 * @param led_id: The led to be Toggled
 *
 */
void Led_Toggle(uint8 led_id)
{
	switch(led_id)
	{
	case LED_0:
		DIO_Toggle(LED_0_GPIO,LED_0_BIT);
		break;
	case LED_1:
		DIO_Toggle(LED_1_GPIO,LED_1_BIT);
		break;
	case LED_2:
		DIO_Toggle(LED_2_GPIO,LED_2_BIT);
		break;
	case LED_3:
		DIO_Toggle(LED_3_GPIO,LED_3_BIT);
		break;
	}
}


