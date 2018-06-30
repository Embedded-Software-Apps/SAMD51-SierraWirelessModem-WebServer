/*
 * ledConfig.h
 *
 * Created: 4/11/2018 4:29:36 PM
 *  Author: Anil Joseph
 */ 


#ifndef LEDCONFIG_H_
#define LEDCONFIG_H_

/* LED Pins */
#define RED_LED GPIO(GPIO_PORTA, 12)
#define GREEN_LED GPIO(GPIO_PORTB, 14)
#define BLUE_LED GPIO(GPIO_PORTA, 22)
#define SWITCH_IN GPIO(GPIO_PORTB, 15)

typedef enum
{
	E_RED_LED,
	E_GREEN_LED,
	E_BLUE_LED
}LED_TYPE_T;



#endif /* LEDCONFIG_H_ */