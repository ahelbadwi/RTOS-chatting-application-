/*
 * Lcd_cfg.h
 *
 *  Created on: Jan 21, 2020
 *      Author: Ahemd_Elbadwi 
 */

#ifndef LCD_CFG_H_
#define LCD_CFG_H_
/*************************************************************************/
/*                         Includes                                      */
/*************************************************************************/
#include "registers.h"
/*************************************************************************/
/*                         #Defines                                      */
/*************************************************************************/

#define UPPER_PORT_PINS

#define LCD_DDR_COMMAND   PORTA_DIR
#define LCD_PORT_COMMAND  PORTA_DATA

#define LCD_DDR_DATA      PORTA_DIR
#define LCD_PORT_DATA     PORTA_DATA

#define RS                (1U)
#define RW                (2U)
#define E                 (3U)


#endif /* LCD_CFG_H_ */
