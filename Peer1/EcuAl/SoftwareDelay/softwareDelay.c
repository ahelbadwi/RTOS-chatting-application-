/*
 * softwareDelay.c
 *
 *  Created on: Dec 5, 2019
 *     Author: ahmed_Elbadwi
 */

#include "softwareDelay.h"

#define MS_SWDELAY 735

/**
 * Description: this function to just achieve ~1msec delay
 * and its scope is limited to this file
 */
static void SwDelay_1ms(void)
{
	uint16 u16delay_iter;
	for(u16delay_iter=0;u16delay_iter<MS_SWDELAY;u16delay_iter++);

}

/**
 * Description: this delay function operate in a polling manner
 * 				don't use it with RTOSs
 * @param n: the milli-seconds
 */

void SwDelay_ms(uint32 n)
{
	uint32 u32SwDelay_iter=0;
	while(u32SwDelay_iter<n)
	{
		SwDelay_1ms();
		u32SwDelay_iter++;
	}

}



void SwDelay_us(uint32 delay_us)
{
	uint32 delay_iter;
	for(delay_iter=0;delay_iter<delay_us;delay_iter++){
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}
}
