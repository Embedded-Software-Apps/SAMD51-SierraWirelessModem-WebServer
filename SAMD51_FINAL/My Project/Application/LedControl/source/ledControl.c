/*
 * ledControl.c
 *
 * Created: 4/11/2018 4:29:53 PM
 *  Author: Anil Joseph
 */ 

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "Application/LedControl/include/ledConfig.h"
#include "Application/LedControl/include/ledControl.h"
#include "atmel_start_pins.h"
#include "Application/Common/Common.h"
#include <hal_gpio.h>
#include <hal_delay.h>
/*****************************************************************************
 *                                 Functions                                  *
 ******************************************************************************/
/*============================================================================
**
** Function Name:      led_Initialise
**
** Description:        Initialize the LEDs.
**
**===========================================================================*/
void led_Initialise(void)
{
	/* Disable all the colors on the RGB LED */
	gpio_set_pin_direction(RED_LED, GPIO_DIRECTION_OUT);
	gpio_set_pin_pull_mode(RED_LED, GPIO_PULL_DOWN);
	gpio_set_pin_function (RED_LED, GPIO_PIN_FUNCTION_OFF);
	//gpio_set_pin_level(RED_LED, true);
	
	gpio_set_pin_direction(GREEN_LED, GPIO_DIRECTION_OUT);
	gpio_set_pin_pull_mode(GREEN_LED, GPIO_PULL_DOWN);
	gpio_set_pin_function (GREEN_LED, GPIO_PIN_FUNCTION_OFF);
	//gpio_set_pin_level(GREEN_LED, true);
	
	gpio_set_pin_direction(BLUE_LED, GPIO_DIRECTION_OUT);
	gpio_set_pin_pull_mode(BLUE_LED, GPIO_PULL_DOWN);
	gpio_set_pin_function (BLUE_LED, GPIO_PIN_FUNCTION_OFF);
	//gpio_set_pin_level(BLUE_LED, true);
	
	/* Initialize the Switch */
	gpio_set_pin_direction(SWITCH_IN, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(SWITCH_IN,GPIO_PULL_OFF);
	gpio_set_pin_function(SWITCH_IN, GPIO_PIN_FUNCTION_OFF);
}


/*============================================================================
**
** Function Name:      led_PerformLedSequence
**
** Description:        Perform the Optional LED Test
**
**===========================================================================*/
void led_PeformLedTest(void)
{
	const TickType_t LedDelayMs = pdMS_TO_TICKS(2000UL);

	gpio_set_pin_direction(RED_LED, GPIO_DIRECTION_OUT);
	gpio_set_pin_pull_mode(RED_LED, GPIO_PULL_OFF);
	gpio_set_pin_function(RED_LED, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_level(RED_LED, false);
	gpio_set_pin_level(GREEN_LED, true);
	gpio_set_pin_level(BLUE_LED, true);
	vTaskDelay(LedDelayMs);

	gpio_set_pin_direction(GREEN_LED, GPIO_DIRECTION_OUT);
	gpio_set_pin_pull_mode(GREEN_LED, GPIO_PULL_OFF);
	gpio_set_pin_function(GREEN_LED, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_level(RED_LED, true);
	gpio_set_pin_level(GREEN_LED, false);
	gpio_set_pin_level(BLUE_LED, true);
	vTaskDelay(LedDelayMs);

	gpio_set_pin_direction(BLUE_LED, GPIO_DIRECTION_OUT);
	gpio_set_pin_pull_mode(BLUE_LED, GPIO_PULL_OFF);
	gpio_set_pin_function(BLUE_LED, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_level(RED_LED, true);
	gpio_set_pin_level(GREEN_LED, true);
	gpio_set_pin_level(BLUE_LED, false);
	vTaskDelay(LedDelayMs);
	
	gpio_set_pin_level(RED_LED, true);
	gpio_set_pin_level(GREEN_LED, true);
	gpio_set_pin_level(BLUE_LED, true);
	vTaskDelay(LedDelayMs);
}

/*============================================================================
**
** Function Name:      led_SetConnectionEstablishedIndication
**
** Description:        Turns on the green LED, when the connection is established
**                     successfully with cloud server.
**
**===========================================================================*/
void led_SetConnectionEstablishedIndication(void)
{
	/* Turn on the green LED */
	gpio_set_pin_level(RED_LED, true);
	gpio_set_pin_level(GREEN_LED, false);
	gpio_set_pin_level(BLUE_LED, true);
}

/*============================================================================
**
** Function Name:      led_SetConnectionInProgressIndication
**
** Description:        Turns on the RED LED, when the connection with the cloud
**                     server in progress.
**
**===========================================================================*/
void led_SetConnectionInProgressIndication(void)
{
	/* Turn on the red LED */
	gpio_set_pin_level(RED_LED, false);
	gpio_set_pin_level(GREEN_LED, true);
	gpio_set_pin_level(BLUE_LED, true);
}
