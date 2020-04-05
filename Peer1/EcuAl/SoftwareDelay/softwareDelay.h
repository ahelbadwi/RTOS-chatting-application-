/*
 * softwareDelay.h
 *
 *  Created on: Oct 21, 2019
 *      Author: Sprints
 */

#ifndef SOFTWAREDELAY_H_
#define SOFTWAREDELAY_H_
#include "std_types.h"

/**
 * Description:this MS_DELAY Macro defines the number of counts to achieve ~1msec delay
 */


/**
 * Description: this delay function operate in a polling manner
 * 				don't use it with RTOSs
 * @param n: the milli-seconds
 */
extern void SwDelay_ms(uint32 n);

extern void SwDelay_us(uint32 delay_us);

#endif /* SOFTWAREDELAY_H_ */
