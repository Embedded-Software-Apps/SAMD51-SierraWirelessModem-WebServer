/*
 * UartDriver.c
 *
 * Created: 5/1/2018 10:20:11 AM
 *  Author: anilj
 */ 
#include "Apps/UartDriver/include/UartDriver.h"
uint8_t TxBuf[10] = {0};
int32_t usart_async_write(struct _usart_async_device *const device,const uint8_t *const buf, const uint16_t length)
{
	uint32_t offset = 0;
	
	while (!_usart_async_is_byte_sent(device));
	
	do {
		TxBuf[offset] = buf[offset];
		_usart_async_write_byte(device, buf[offset]);
		while (!_usart_async_is_byte_sent(device))
		;
	} while (++offset < length);

	return (int32_t)offset;
}


int32_t usart_async_read(struct _usart_async_device *const device,uint8_t *const buf, const uint16_t length)
{
	uint32_t offset = 0;
	
	do {
		while (!_usart_async_is_byte_received(device))
		;
		buf[offset] = _usart_async_read_byte(device);
	} while (++offset < length);

	return (int32_t)offset;
}
