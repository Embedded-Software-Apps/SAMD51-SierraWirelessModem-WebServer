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

#define SIZE_MODEM_RX_DATA_BUF (2048)
static uint8_t RxDataBuff[2];
uint8_t printBuff[40];
uint8_t rxEcho[2];
uint8_t  ModemRxDatabuffer[SIZE_MODEM_RX_DATA_BUF];

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

struct ringbuffer RxRingBuffer;
/*============================================================================
**
** Function Name:      ModemDataCommInit
**
** Description:        Initializes the SERCOM3 UART Module for Modem Data.
**
**===========================================================================*/
void mdmCtrlr_DataCommInit(void)
{
	uint32_t initStatus;
	
	initStatus = _usart_async_init(&MODEM_DATA,SERCOM3);
	
	if(initStatus == ERR_NONE)
	{
		initStatus = ringbuffer_init(&RxRingBuffer, ModemRxDatabuffer, SIZE_MODEM_RX_DATA_BUF);
	}
	
	if(initStatus == ERR_NONE)
	{
		/* Enable all of the UART interrupts for SERCOM3 */
		_usart_async_set_irq_state(&MODEM_DATA,USART_ASYNC_RX_DONE,true);
		_usart_async_enable(&MODEM_DATA);
		sprintf(printBuff,"MODEM DATA UART (SERCOM3) initialized\r\n");
		SerialDebugPrint(printBuff,sizeof(printBuff));
	}
	else
	{
		sprintf(printBuff,"SERCOM3 not initialized\r\n");
		SerialDebugPrint(printBuff,sizeof(printBuff));
	}
}

/*============================================================================
**
** Function Name:      SERCOM3_0_Handler
**
** Description:        DRE: TX Data Register Empty Interrupt
**
**===========================================================================*/
void SERCOM3_0_Handler( void )
{
	/* DRE: TX Data Register Empty */
	sprintf(printBuff,"Modem Data Tx Data Reg Empty CallBack\r\n");
	SerialDebugPrint(printBuff,sizeof(printBuff));
	hri_sercomusart_clear_interrupt_DRE_bit(SERCOM3);
}

/*============================================================================
**
** Function Name:      SERCOM3_1_Handler
**
** Description:        TXC : Transmit Complete Interrupt
**
**===========================================================================*/
void SERCOM3_1_Handler(void )
{
	/* TXC : Transmit Complete */
	sprintf(printBuff,"Modem Data Tx byte Complete Callback\r\n");
	SerialDebugPrint(printBuff,sizeof(printBuff));
	hri_sercomusart_clear_interrupt_TXC_bit(SERCOM3);
}

/*============================================================================
**
** Function Name:      SERCOM3_2_Handler
**
** Description:        RXC : Receive Complete Interrupt
**
**===========================================================================*/
void SERCOM3_2_Handler( void )
{
	uint8_t rcvdChar = 0;
	while (!_usart_async_is_byte_received(&MODEM_DATA));
	rcvdChar = _usart_async_read_byte(&MODEM_DATA);
	ringbuffer_put(&RxRingBuffer, rcvdChar);
}

/*============================================================================
**
** Function Name:      sendCommandToModem
**
** Description:        Transmits Data to Modem
**
**===========================================================================*/
uint32_t mdmCtrlr_SendDataToModem(const uint8_t *const TxData,const uint16_t length)
{
	_usart_async_enable(&MODEM_DATA);
	return usart_async_write(&MODEM_DATA, TxData, length);
}

/*============================================================================
**
** Function Name:      sendCommandToModem
**
** Description:        Transmits Data to Modem
**
**===========================================================================*/
void mdmCtrlr_ReadResponseFromModem(uint8_t *const buf, const uint16_t length)
{
	uint8_t readCnt = 0;
	if (ringbuffer_num(&RxRingBuffer) >= length)
	{
		CRITICAL_SECTION_ENTER()
		while (readCnt < length)
		{
			ringbuffer_get(&RxRingBuffer, &buf[readCnt++]);
		}
		CRITICAL_SECTION_LEAVE()
	}	
}
