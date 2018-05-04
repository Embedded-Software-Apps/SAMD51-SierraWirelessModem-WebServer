/*
 * ledControl.h
 *
 * Created: 4/11/2018 4:29:21 PM
 *  Author: Anil Joseph
 */ 


#ifndef LEDCONTROL_H_
#define LEDCONTROL_H_

#include "apps/LedControl/include/ledConfig.h"
#include "apps/LedControl/include/ledControl.h"
#include "driver_init.h"
#include "utils.h"
/*****************************************************************************
*                                 Global Function Prototypes                 *
******************************************************************************/
void led_Initialise(void);
void led_PeformLedTest(void);
void led_SetPwmOutput(LED_TYPE_T ledColor,uint32_t period,uint32_t dutyCycle);

#endif /* LEDCONTROL_H_ */