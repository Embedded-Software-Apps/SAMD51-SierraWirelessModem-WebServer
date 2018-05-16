/*
 * selectInputs.h
 *
 * Created: 4/20/2018 1:43:11 AM
 *  Author: anilj
 */ 


#ifndef SELECTINPUTS_H_
#define SELECTINPUTS_H_

/* Select Lines */
#define SELECT_0 GPIO(GPIO_PORTB, 13)
#define SELECT_1 GPIO(GPIO_PORTB, 12)
#define SELECT_2 GPIO(GPIO_PORTB, 11)
#define SELECT_3 GPIO(GPIO_PORTB, 10)
#define SELECT_4 GPIO(GPIO_PORTA, 11)
#define SELECT_5 GPIO(GPIO_PORTA, 10)
#define SELECT_6 GPIO(GPIO_PORTA, 9)
#define SELECT_7 GPIO(GPIO_PORTA, 8)

void selectInputsInit(void);



#endif /* SELECTINPUTS_H_ */