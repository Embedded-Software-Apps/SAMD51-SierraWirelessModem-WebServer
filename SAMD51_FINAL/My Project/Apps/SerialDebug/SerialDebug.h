/*
 * SerialDebug.h
 *
 * Created: 4/14/2018 12:54:50 PM
 *  Author: anilj
 */ 


#ifndef SERIALDEBUG_H_
#define SERIALDEBUG_H_

typedef enum
{
	SERIAL_DEBUG_TEST_READ,
	SERIAL_DEBUG_TEST_WRITE,
	SERIAL_DEBUG_IDLE	
}SERIAL_TEST;

void SerialDebugPrintInit(void);
void SerialDebugStateMachine(void);
#if 0
void SerialDebugTxByteSentCallBack(struct _usart_async_device *device);
void SerialDebugTxDoneCallBack(struct _usart_async_device *device);
void SerialDebugRxDoneCallBack(struct _usart_async_device *device,uint8_t data);
void SerialDebugErrorCallBack(struct _usart_async_device *device);
#endif
void SerialDebugPrint(const uint8_t *const dataToPrint,const uint16_t length);
#endif /* SERIALDEBUG_H_ */