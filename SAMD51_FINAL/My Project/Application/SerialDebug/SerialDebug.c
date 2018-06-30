/*
 * SerialDebug.c
 *
 * Created: 4/14/2018 12:54:34 PM
 *  Author: anilj
 */ 
#include "driver_init.h"
#include "utils.h"
#include "Application/SerialDebug/SerialDebug.h"
#include "Application/UartDriver/include/UartDriver.h"
#include "Application/Common/Common.h"
#include <hpl_usart_async.h>
#include <string.h>
#include <stdio.h>
#include "portmacro.h"
/* Initialize the UART Serial Debug interface and register the call backs */

uint8_t printBuf[5];
uint8_t rcvdChar[2];

/* Interrupt Handlers */
void SerialDebugTxByteSentCallBack(struct _usart_async_device *device)
{
	hri_sercomusart_clear_interrupt_TXC_bit(device);
}

void SerialDebugTxDoneCallBack(struct _usart_async_device *device)
{
	SerialDebugPrint((uint8_t*)"Tx serial Debug Done\r\n",22);
	hri_sercomusart_clear_interrupt_DRE_bit(device);
}

void SerialDebugRxDoneCallBack(struct _usart_async_device *device,uint8_t data)
{
	//RxData = data;
	hri_sercomusart_clear_interrupt_RXC_bit(device);
	SerialDebugPrint((uint8_t*)"Rx Done\r\n",9);
}

void SerialDebugErrorCallBack(struct _usart_async_device *device)
{
	hri_sercomusart_clear_INTFLAG_ERROR_bit(device);
}

struct _usart_async_device SERIAL_DEBUG_PRINT =
{
	.hw = SERCOM5,
	.irq.handler = NULL,
	.irq.parameter = 0,
	.usart_cb.tx_done_cb = SerialDebugTxDoneCallBack,
	.usart_cb.tx_byte_sent = SerialDebugTxByteSentCallBack,
	.usart_cb.rx_done_cb = NULL,
	.usart_cb.error_cb = SerialDebugErrorCallBack,
};

void SerialDebugPrintInit(void)
{
	uint32_t initStatus;
	
	initStatus = _usart_async_init(&SERIAL_DEBUG_PRINT,SERCOM5);
	
	if(initStatus != ERR_NONE)
	{
		/* SERCOM5 initialization failed */	
	}
	
	/* Enable only the UART RX Interrupt */
	_usart_async_set_irq_state(&SERIAL_DEBUG_PRINT,USART_ASYNC_RX_DONE,true);
}


void SerialDebugPrint(const uint8_t *const dataToPrint,const uint16_t length)
{
	_usart_async_enable(&SERIAL_DEBUG_PRINT);
	
	usart_async_write(&SERIAL_DEBUG_PRINT, dataToPrint, length);
}

void SerialStringPrint(const uint8_t *const dataToPrint)
{
	#ifdef SERIAL_DEBUG_ON
	
	uint8_t strLength;
	const TickType_t printDelayMs = pdMS_TO_TICKS(500UL);
	uint8_t* dbgBuffer = NULL;
	strLength = strlen(dataToPrint);

    if( xSemaphoreTake( DebugPrintMutex,printDelayMs) == pdTRUE )
    {
		dbgBuffer = (uint8_t*)pvPortMalloc((strLength)*(sizeof(uint8_t)));
		
		if(dbgBuffer != NULL)
		{
	    	memset(dbgBuffer,'\0',strLength);
	    	sprintf((int8_t*)dbgBuffer,"%s",dataToPrint);
	    	SerialDebugPrint(dbgBuffer,strlen(dbgBuffer));
			SerialDebugPrint("\r\n",2);
	    	vPortFree(dbgBuffer);			
		}
    	xSemaphoreGive(DebugPrintMutex);
    }
	
	#endif
}

void ConsoleDebugPrint(const uint8_t *const txt, uint32_t intData)
{
	uint8_t dbgBuffer[100];
	memset(dbgBuffer,'\0',100);

	if(intData != 0)
	{
		sprintf((int8_t*)dbgBuffer,"%s - %d \r\n",txt,intData);
		SerialDebugPrint(dbgBuffer,strlen(dbgBuffer));
	}
}
/* UART Rx Interrupt Handler */ 
/*
 **** Just echo back the char entered on the serial terminal****
 */
void SERCOM5_2_Handler( void )
{
	//SerialDebugPrint((uint8_t*)"Serial Rx Complete CallBack\r\n",19);
	hri_sercomusart_clear_interrupt_RXC_bit(SERCOM5);
	
	while (!_usart_async_is_byte_received(&SERIAL_DEBUG_PRINT));
	
	rcvdChar[0] = _usart_async_read_byte(&SERIAL_DEBUG_PRINT);
	rcvdChar[1] = '\0';
	sprintf((char*)printBuf,"%s",rcvdChar);
	SerialDebugPrint(printBuf,sizeof(printBuf));
}


/*****************************************************************************************************
***********************************Below code, Not used as of now*************************************
******************************************************************************************************/
