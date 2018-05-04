/*
 * selectInputs.c
 *
 * Created: 4/20/2018 1:42:52 AM
 *  Author: anilj
 */ 
#include "atmel_start_pins.h"
#include "Apps/SelectInputs/include/selectInputs.h"
#include <hal_gpio.h>

void selectInputsInit(void)
{
	gpio_set_pin_direction(SELECT_0, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(SELECT_0,GPIO_PULL_UP);
	gpio_set_pin_function(SELECT_0, GPIO_PIN_FUNCTION_OFF);
	
	gpio_set_pin_direction(SELECT_1, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(SELECT_1,GPIO_PULL_UP);
	gpio_set_pin_function(SELECT_1, GPIO_PIN_FUNCTION_OFF);
		
	gpio_set_pin_direction(SELECT_2, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(SELECT_2,GPIO_PULL_UP);
	gpio_set_pin_function(SELECT_2, GPIO_PIN_FUNCTION_OFF);
	
	gpio_set_pin_direction(SELECT_3, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(SELECT_3,GPIO_PULL_UP);
	gpio_set_pin_function(SELECT_3, GPIO_PIN_FUNCTION_OFF);
	
	gpio_set_pin_direction(SELECT_4, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(SELECT_4,GPIO_PULL_UP);
	gpio_set_pin_function(SELECT_4, GPIO_PIN_FUNCTION_OFF);
		
	gpio_set_pin_direction(SELECT_5, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(SELECT_5,GPIO_PULL_UP);
	gpio_set_pin_function(SELECT_5, GPIO_PIN_FUNCTION_OFF);
		
	gpio_set_pin_direction(SELECT_6, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(SELECT_6,GPIO_PULL_UP);
	gpio_set_pin_function(SELECT_6, GPIO_PIN_FUNCTION_OFF);		
	
	gpio_set_pin_direction(SELECT_7, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(SELECT_7,GPIO_PULL_UP);
	gpio_set_pin_function(SELECT_7, GPIO_PIN_FUNCTION_OFF);	
}
