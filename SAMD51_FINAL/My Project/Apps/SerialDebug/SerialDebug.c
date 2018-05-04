/*
 * SerialDebug.c
 *
 * Created: 4/14/2018 12:54:34 PM
 *  Author: anilj
 */ 
#include "driver_init.h"
#include "utils.h"
#include "Apps/SerialDebug/SerialDebug.h"
#include "Apps/UartDriver/include/UartDriver.h"
#include <hpl_usart_async.h>
#include <string.h>
#include <stdio.h>

/* Initialize the UART Serial Debug interface and register the call backs */
#if 0
uint8_t ReadBuff[30];
uint8_t RxData;
static SERIAL_TEST serTest;
#endif


uint8_t printBuf[5];
uint8_t rcvdChar[2];

struct _usart_async_device DEBUG_PRINT = 
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
	
	initStatus = _usart_async_init(&DEBUG_PRINT,SERCOM5);
	
	if(initStatus != ERR_NONE)
	{
		/* SERCOM5 initialization failed */	
	}
	else
	{

	}
	
	//serTest = SERIAL_DEBUG_TEST_WRITE;
	
	/* Enabled the UART Interrupts */
	_usart_async_set_irq_state(&DEBUG_PRINT,USART_ASYNC_RX_DONE,true);
#if 0
	//_usart_async_set_irq_state(&DEBUG_PRINT,USART_ASYNC_BYTE_SENT,true);
	_usart_async_set_irq_state(&DEBUG_PRINT,USART_ASYNC_TX_DONE,true);
	_usart_async_set_irq_state(&DEBUG_PRINT,USART_ASYNC_ERROR,true);
	_usart_async_enable_tx_done_irq(&DEBUG_PRINT);
#endif
	
	//_usart_async_enable(&DEBUG_PRINT);

}



#if 0
void SerialDebugStateMachine(void)
{
	uint8_t cnt;
	
	switch(serTest)
	{
		case SERIAL_DEBUG_TEST_WRITE:
		{
			SerialDebugPrint((uint8_t*)"Enter your Name\r\n",20);
			delay_ms(3000);
			//SerialDebugPrint((uint8_t*)"Enter your Name\r\n",20);
			serTest = SERIAL_DEBUG_IDLE;
		}
		break;

		case SERIAL_DEBUG_TEST_READ:
		{
			cnt = usart_async_read(&DEBUG_PRINT,ReadBuff,5);
			serTest = SERIAL_DEBUG_IDLE;
			
			if(cnt == 5)
			{
				SerialDebugPrint((uint8_t*)"Received 5 chars\r\n",18);
				
				if((0==memcmp(ReadBuff,"Anil1",5)))
				{
					SerialDebugPrint((uint8_t*)"Received Anil1\r\n",16);
				}
				else
				{
					SerialDebugPrint((uint8_t*)"Not Received Anil1\r\n",20);
				}
			}
			else
			{
				SerialDebugPrint((uint8_t*)"Not Received 5 chars\r\n",22);
			}
			
		}
		break;
		
		case SERIAL_DEBUG_IDLE:
		{
			delay_ms(15000);
			serTest = SERIAL_DEBUG_TEST_WRITE;
		}
		break;
		default:
		break;
	}
}
#endif

void SerialDebugPrint(const uint8_t *const dataToPrint,const uint16_t length)
{
	_usart_async_enable(&DEBUG_PRINT);
	
	usart_async_write(&DEBUG_PRINT, dataToPrint, length);
}



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

void SERCOM5_2_Handler( void )
{
	//SerialDebugPrint((uint8_t*)"Serial Rx Complete CallBack\r\n",19);
	hri_sercomusart_clear_interrupt_RXC_bit(SERCOM5);
	
	while (!_usart_async_is_byte_received(&DEBUG_PRINT));
	
	rcvdChar[0] = _usart_async_read_byte(&DEBUG_PRINT);
	rcvdChar[1] = '\0';
	sprintf(printBuf,"%s",rcvdChar);
	SerialDebugPrint(printBuf,sizeof(printBuf));
}