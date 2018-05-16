/*
 * UartDriver.h
 *
 * Created: 5/1/2018 10:19:55 AM
 *  Author: anilj
 */ 


#ifndef UARTDRIVER_H_
#define UARTDRIVER_H_

#include <hpl_usart_async.h>

int32_t usart_async_write(struct _usart_async_device *const device,const uint8_t *const buf, const uint16_t length);

int32_t usart_async_read(struct _usart_async_device *const device,uint8_t *const buf, const uint16_t length);



#endif /* UARTDRIVER_H_ */