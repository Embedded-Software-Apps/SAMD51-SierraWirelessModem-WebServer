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
#include "Apps/Common/Common.h"
#include "Apps/Tasks/ModemTask/include/ModemAtCommandSet.h"

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define SIZE_MODEM_RX_DATA_BUF (2048)
#define ENABLE_SERCOM3_RX_DEBUG (0)

static uint8_t RxDataBuff[2];
uint8_t rxEcho[2];
uint8_t  ModemRxDatabuffer[SIZE_MODEM_RX_DATA_BUF];
static uint32_t prior = 0;
struct _usart_async_device MODEM_SERCOM3_UART =
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
	initStatus = _usart_async_init(&MODEM_SERCOM3_UART,SERCOM3);
	
	if(initStatus == ERR_NONE)
	{
		initStatus = ringbuffer_init(&RxRingBuffer, ModemRxDatabuffer, SIZE_MODEM_RX_DATA_BUF);
	}
	
	if(initStatus == ERR_NONE)
	{
		/* Enable all of the UART interrupts for SERCOM3 */
		_usart_async_set_irq_state(&MODEM_SERCOM3_UART,USART_ASYNC_RX_DONE,true);
		_usart_async_enable(&MODEM_SERCOM3_UART);
		DEBUG_PRINT("MODEM DATA UART (SERCOM3) initialized");
		prior = NVIC_GetPriority(60);
		ConsoleDebugPrint("SERCOM3 PRIORITY", prior);
		
	}
	else
	{
		DEBUG_PRINT("Failed to initialize the MODEM DATA UART");
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
	uint8_t rcvdChar[2];
	uint8_t rxPrint[2];
	BaseType_t xHigherPriorityTaskWoken;
	uint32_t ulValue;
	MODEM_CMD_DATA cmdData;
	AT_CMD_TYPE lastCmd;
	
	while (!_usart_async_is_byte_received(&MODEM_SERCOM3_UART));
	rcvdChar[0] = _usart_async_read_byte(&MODEM_SERCOM3_UART);
	
#if ENABLE_SERCOM3_RX_DEBUG
	rcvdChar[1] = '\0';
	sprintf((char*)rxPrint,"%s",rcvdChar);
	SerialDebugPrint(rxPrint,sizeof(rxPrint));
#endif
	
	ringbuffer_put(&RxRingBuffer, rcvdChar[0]);
	lastCmd = mdmParser_GetLastSentAtCommand();

	if(lastCmd != CMD_AT_MAX)
	{
		getModemCommandData(lastCmd, &cmdData);

		if(ringbuffer_num(&RxRingBuffer) >= cmdData.ResponseLength)
		{
			/* Send a notification directly to the handler task. */
		    xTaskNotifyFromISR( xModemRxTaskHandle, lastCmd, eSetValueWithOverwrite, &xHigherPriorityTaskWoken );

			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

			mdmParser_SetLastSentAtCommand(CMD_AT_MAX);
		}
	}

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
	_usart_async_enable(&MODEM_SERCOM3_UART);
	return usart_async_write(&MODEM_SERCOM3_UART, TxData, length);
}

/*============================================================================
**
** Function Name:      sendCommandToModem
**
** Description:        Transmits Data to Modem
**
**===========================================================================*/
bool mdmCtrlr_ReadResponseFromModem(uint8_t *const buf, const uint16_t length)
{
	bool status = false;
	
	uint16_t readCnt = 0;
	if (ringbuffer_num(&RxRingBuffer) >= length)
	{
		CRITICAL_SECTION_ENTER()
		while (readCnt < length)
		{
			ringbuffer_get(&RxRingBuffer, &buf[readCnt++]);
		}
		status = true;
		CRITICAL_SECTION_LEAVE()
	}
	else
	{
		/* Data is not available at Rx Buffer */
		status = false;
	}	
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
bool mdmCtrlr_CheckForUnSolicitedResponses(void)
{
/*	uint8_t readCnt = 0;
	uint8_t

	if (ringbuffer_num(&RxRingBuffer) >= 0)
	{
		while(readCnt < (RxRingBuffer.write_index-RxRingBuffer.write_index)
	}*/
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
void mdmCtrlr_FlushRxBuffer(void)
{
	ringbuffer_flush(&RxRingBuffer);
}
