/*
 * ModemController.c
 *
 * Created: 4/21/2018 8:43:53 PM
 *  Author: toani
 */ 
#include <hal_usart_sync.h>
#include <stdio.h>
#include <hal_usart_os.h>

#include "Apps\Tasks\ModemTask\include\ModemController.h"
#include "Apps\Tasks\ModemTask\include\ModemPowerControl.h"
#include "Apps\SerialDebug\SerialDebug.h"
#include "Apps\UartDriver\include\UartDriver.h"


static MDM_TEST_STATES mdmTestState = MDM_AT_TEST;
static uint8_t RxDataBuff[2];
uint8_t printBuff[40];
uint8_t rxEcho[2];
struct usart_os_descriptor MODEM_USART_DATA;
uint8_t                    MODEM_USART_DATA_buffer[2048];

struct _usart_async_device MODEM_DATA =
{
	.hw = SERCOM3,
	.irq.handler = NULL,
	.irq.parameter = 0,

	.usart_cb.tx_done_cb = NULL,
	.usart_cb.tx_byte_sent = NULL,
	.usart_cb.rx_done_cb = NULL,
	.usart_cb.error_cb = NULL

};

void ModemDataCommInit(void)
{
	uint32_t initStatus;
	
	initStatus = _usart_async_init(&MODEM_DATA,SERCOM3);
	
	/* Enable all of the UART interrupts for SERCOM3 */
	#if 1
	_usart_async_set_irq_state(&MODEM_DATA,USART_ASYNC_BYTE_SENT,true);
	_usart_async_set_irq_state(&MODEM_DATA,USART_ASYNC_TX_DONE,true);
	_usart_async_set_irq_state(&MODEM_DATA,USART_ASYNC_ERROR,true);
	_usart_async_enable_tx_done_irq(&MODEM_DATA);
	#endif
	
	if(initStatus == ERR_NONE)
	{
		sprintf(printBuff,"MODEM DATA UART (SERCOM3) initialized\r\n");
		SerialDebugPrint(printBuff,sizeof(printBuff));
	}
	else
	{
		sprintf(printBuff,"SERCOM3 not initialized\r\n");
		SerialDebugPrint(printBuff,sizeof(printBuff));
	}
}


void SERCOM3_0_Handler( void )
{
	/* DRE: TX Data Register Empty */
	sprintf(printBuff,"Modem Data Tx Data Reg Empty CallBack\r\n");
	SerialDebugPrint(printBuff,sizeof(printBuff));
	
	hri_sercomusart_clear_interrupt_DRE_bit(SERCOM3);
	/* Disable the TX Data reg empty interrupt after transmitting the first set of data */
	SERCOM3->USART.INTENCLR.bit.DRE = 1;
}

void SERCOM3_1_Handler(void )
{
	/* TXC : Transmit Complete */
	sprintf(printBuff,"Modem Data Tx byte Complete Callback\r\n");
	SerialDebugPrint(printBuff,sizeof(printBuff));
	hri_sercomusart_clear_interrupt_TXC_bit(SERCOM3);
}

/* This interrupt will be called if SAMD51 receives any data from Modem 
 * through SERCOM3 UART Interafce.
 */
void SERCOM3_2_Handler( void )
{
	/* RXC : Receive Complete */
	sprintf(printBuff,"Modem Data Rx Complete Callback\r\n");
	SerialDebugPrint(printBuff,sizeof(printBuff));

	sprintf(printBuff,"Successfully Received a char from Modem\r\n");
	SerialDebugPrint(printBuff,sizeof(printBuff));
	
	hri_sercomusart_clear_interrupt_RXC_bit(SERCOM3);
	
	while (!_usart_async_is_byte_received(&MODEM_DATA));
	
	RxDataBuff[0] = _usart_async_read_byte(&MODEM_DATA);
	SerialDebugPrint(RxDataBuff[0],1);
}


#if 1
uint32_t sendCommandToModem(const uint8_t *const TxData,const uint16_t length)
{
	_usart_async_enable(&MODEM_DATA);
	return usart_async_write(&MODEM_DATA, TxData, length);
}

int32_t ReadResponseFromModem(uint8_t *RecvdData,const int32_t length)
{
	_usart_async_enable(&MODEM_DATA);
	return usart_async_read(&MODEM_DATA, RecvdData,length);
}
#endif



/*****************************************************************************************************
***********************************Below code, Not used as of now*************************************
******************************************************************************************************/

/* Interrupt Handlers */
void ModemDataTxByteSentCallBack(struct _usart_async_device *device)
{
	
}

void ModemDataTxDoneCallBack(struct _usart_async_device *device)
{
	SerialDebugPrint((uint8_t*)"Modem Tx Done CallBack\r\n",19);
	hri_sercomusart_clear_interrupt_DRE_bit(device);
	SERCOM3->USART.INTENCLR.bit.DRE = 1;
}

void ModemDataRxDoneCallBack(struct _usart_async_device *device,uint8_t data)
{
	//RxData = data;
	SerialDebugPrint((uint8_t*)"Modem Rx Callback\r\n",19);
	hri_sercomusart_clear_interrupt_RXC_bit(device);
}

void ModemDataErrorCallBack(struct _usart_async_device *device)
{
	SerialDebugPrint((uint8_t*)"Modem Error Callback\r\n",22);
	hri_sercomusart_clear_INTFLAG_ERROR_bit(device);
}



void UsartsendCommandToModem(const uint8_t *const TxData,const uint16_t length)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_3, &io);
	usart_sync_enable(&USART_3);
	
	io_write(io, TxData, length);
}

int32_t UsartReadResponseFromModem(uint8_t *RecvdData,const int32_t length)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_3, &io);
	usart_sync_enable(&USART_3);
	
	return io_read(io, &RecvdData, length);
}

void ModemUsartOsInit(void)
{
	bool initStatus = false;
	
	initStatus = usart_os_init(&MODEM_USART_DATA, SERCOM3, MODEM_USART_DATA_buffer, 2048, (void *)NULL);
	usart_os_enable(&MODEM_USART_DATA);
	
	if(initStatus != 0)
	{
		sprintf(printBuff,"SERCOM3 Init Failed. Error %d.\r\n",initStatus);
		SerialDebugPrint(printBuff,sizeof(printBuff));
	}
	else
	{
		sprintf(printBuff,"SERCOM3 Init Success.\r\n");
		SerialDebugPrint(printBuff,sizeof(printBuff));
	}	
}


void ModemUsartOsTest(void)
{
	struct io_descriptor *io;
	uint8_t readResp[10];
	uint8_t readCnt;
	uint8_t writeStatus;
	 
	usart_os_get_io(&MODEM_USART_DATA, &io);
	writeStatus = io->write(io,"AT\r\n",4);
	
	if(writeStatus != 0)
	{
		sprintf(printBuff,"Tx to modem Failed Error %d\r\n",writeStatus);
		SerialDebugPrint(printBuff,sizeof(printBuff));
	}
	else
	{
		SerialDebugPrint("Tx to modem Success",21);
	}
	
	delay_ms(1000);
	readCnt = io->read(io,&readResp,3);
	
	if(readCnt > 0)
	{
		sprintf(printBuff,"Read %d chars. Data is %s\r\n",&readResp);
		SerialDebugPrint(printBuff,sizeof(printBuff));
	}
	else
	{
		sprintf(printBuff,"Read Failed.\r\n");
		SerialDebugPrint(printBuff,sizeof(printBuff));
	}
	
	delay_ms(5000);	
	
} 
