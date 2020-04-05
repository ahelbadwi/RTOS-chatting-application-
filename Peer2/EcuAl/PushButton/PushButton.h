/*
 * PushButton.h
 *
 *  Created on: Dec 29, 2019
 *      Author: Ahemd_Elbadwi 
 */

/********************************************************************/
/*                        Includes                                  */
/********************************************************************/
#include "std_types.h"
#include "Utils.h"
#include "General_Macros.h"
#include "DIO.h"
#include "softwareDelay.h"

#ifndef PUSHBUTTON_H_
#define PUSHBUTTON_H_

#define BTN_0     (0)
#define BTN_1     (1)

/***********************************************************************/
/*                        Typedefs                                     */
/***********************************************************************/
typedef enum En_buttonStatus_t{
	Released,
	Pressed,
	Prepressed,
	Prereleased

}En_buttonStatus_t;

#define BOUNCING_TIMEms 50
#define SAMPLING_DELAYms 10
#define MAX_NO_SAMPLES 5


/**
 * Description: Initialize the BTN_x Pin state (where x 0, 1, 2, 3) to Input
 * @param btn_id: The btn to be initialized and it takes
 * 				  button id
 *
 */
extern ErrorStatus_t pushButton_Init(uint8 btn_id);


extern ErrorStatus_t pushButton_Update(void);


extern ErrorStatus_t pushButton_GetStatus(uint8 btn_id,En_buttonStatus_t* En_buttonStatus);



#endif /* PUSHBUTTON_H_ */
