/*
 * ledPwmControl.c
 *
 * Created: 4/14/2018 9:43:39 PM
 *  Author: anilj
 */ 
#include "driver_init.h"
#include "utils.h"
#include "apps/LedControl/include/ledConfig.h"
#include "apps/LedControl/include/ledControl.h"
#include "Apps/SerialDebug/SerialDebug.h"

#define MAX_LED (3)

void led_SetPwmOutput(LED_TYPE_T ledColor,uint32_t period,uint32_t dutyCycle)
{
	const  uint32_t pin[MAX_LED] = {RED_LED, GREEN_LED, BLUE_LED};
	const  uint32_t pinmode[MAX_LED] = {PINMUX_PA12E_TC2_WO0, PINMUX_PB14E_TC5_WO0, PINMUX_PA22E_TC4_WO0};
	
	gpio_set_pin_function(pin[ledColor], pinmode[ledColor]);
	
	switch(ledColor)
	{
		case E_RED_LED:
		{
			//pwm_set_parameters(&PWM_2, period, dutyCycle);
			//pwm_enable(&PWM_2);
			//SerialDebugPrint((uint8_t*)"RED PWM ENABLED\n",16);
		}
		break;
		
		case E_GREEN_LED:
		{
			//pwm_set_parameters(&PWM_5, period, dutyCycle);
			//pwm_enable(&PWM_5);	
			//SerialDebugPrint((uint8_t*)"GREEN PWM ENABLED\n",18);
		}
		break;
		
		case E_BLUE_LED:
		{		
			//pwm_set_parameters(&PWM_4, period, dutyCycle);
			//pwm_enable(&PWM_4);
			//SerialDebugPrint((uint8_t*)"BLUE PWM ENABLED\n",17);			
		}
		break;
		
		default:
		break;
	}

	
}