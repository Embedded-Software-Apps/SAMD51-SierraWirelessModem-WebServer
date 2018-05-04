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
static uint8_t RxCnt;
uint8_t printBuff[30];
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
	
	/* Enabled the UART Interrupts */
	_usart_async_set_irq_state(&DEBUG_PRINT,USART_ASYNC_RX_DONE,true);
	#if 0
	//_usart_async_set_irq_state(&DEBUG_PRINT,USART_ASYNC_BYTE_SENT,true);
	_usart_async_set_irq_state(&DEBUG_PRINT,USART_ASYNC_TX_DONE,true);
	_usart_async_set_irq_state(&DEBUG_PRINT,USART_ASYNC_ERROR,true);
	_usart_async_enable_tx_done_irq(&DEBUG_PRINT);
	#endif
	
	if(initStatus == ERR_NONE)
	{
		sprintf(printBuff,"SERCOM3 initialized\r\n");
		SerialDebugPrint(printBuff,sizeof(printBuff));
	}
	else
	{
		sprintf(printBuff,"SERCOM3 not initialized\r\n");
		SerialDebugPrint(printBuff,sizeof(printBuff));
	}
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

void ModemControllerSchedule(void)
{
	uint8_t Response[30];
	uint32_t writeCnt;
	uint32_t readCnt;
	uint8_t printBuff[50];
	
	if(getModemPowerStatus() == MDM_PWR_FULLY_OPERATIONAL)
	{		
		switch(mdmTestState)
		{
			case MDM_AT_TEST:
			{
#if 1
				writeCnt = sendCommandToModem((uint8_t*)"AT\r\n",4);
				//sprintf(printBuff,"Transmitted %d char to Modem \r\n",writeCnt);
				//SerialDebugPrint((uint8_t*)printBuff,sizeof(printBuff));
				//mdmTestState = MDM_AT_IDLE;
				vTaskDelay(5000);
#endif
			}
			break;
			
			case MDM_AT_IDLE:
			{
				
			}
			break;
			
			case MDM_SET_SLEEP_SETTINGS:
			{
				//sprintf(printBuff,"Inside MDM_SET_SLEEP_SETTINGS\r\n");
				//SerialDebugPrint((uint8_t*)printBuff,sizeof(printBuff));
				
				writeCnt = sendCommandToModem((uint8_t*)"AT+KSLEEP=2\r\n",13);
				sprintf(printBuff,"Transmitted %d char to Modem for sleep settings \r\n",writeCnt);
				SerialDebugPrint((uint8_t*)printBuff,sizeof(printBuff));
				vTaskDelay(100);
				readCnt = ReadResponseFromModem(Response,3);
				
				if(readCnt > 0)
				{
					sprintf(printBuff,"Read %d char from Modem.\r\n",readCnt);
					SerialDebugPrint(printBuff,sizeof(printBuff));
					
					if((Response[0] == 'O') &&
					(Response[1] == 'K'))
					{
						sprintf(printBuff,"OK Response received\r\n");
						SerialDebugPrint(printBuff,sizeof(printBuff));
						mdmTestState = MDM_WRITE_MODE;
					}
					else
					{
						sprintf(printBuff,"No Proper Response.\r\n");
						SerialDebugPrint(printBuff,sizeof(printBuff));
					}
				}
				else
				{
					SerialDebugPrint("Response not read\r\n",19);
				}
			}
			break;
			
			case MDM_WRITE_MODE:
			{
				sprintf(printBuff,"Inside MDM_SET_SLEEP_SETTINGS\r\n");
				SerialDebugPrint((uint8_t*)printBuff,sizeof(printBuff));
								
				writeCnt = sendCommandToModem((uint8_t*)"AT+CGSN=?\r\n",11);
				sprintf(printBuff,"Transmitted %d char to Modem \r\n",writeCnt);
				SerialDebugPrint((uint8_t*)printBuff,sizeof(printBuff));
				vTaskDelay(50);
				mdmTestState = MDM_READ_MODE;
			}
			break;

			case MDM_READ_MODE:
			{
				sprintf(printBuff,"Inside MDM_SET_SLEEP_SETTINGS\r\n");
				SerialDebugPrint((uint8_t*)printBuff,sizeof(printBuff));
								
				readCnt = ReadResponseFromModem(Response,3);
				
				if(readCnt > 0)
				{
					sprintf(printBuff,"Read %d char from Modem.\r\n",readCnt);
					SerialDebugPrint(printBuff,sizeof(printBuff));
					
					if((Response[0] == 'O') &&
					(Response[0] == 'K'))
					{
						sprintf(printBuff,"OK Response received\r\n");
						SerialDebugPrint(printBuff,sizeof(printBuff));
					}
					else
					{
						sprintf(printBuff,"No Proper Response.\r\n");
						SerialDebugPrint(printBuff,sizeof(printBuff));
					}
					
					mdmTestState = MDM_WRITE_MODE;
				}
				else
				{
					SerialDebugPrint("Response not read\r\n",19);
					mdmTestState = MDM_WRITE_MODE;
				}
				
				vTaskDelay(500);
			}
			break;
		}
	}
	else
	{
		/* Do Nothing */
	}
	
}


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

void SERCOM3_0_Handler( void )
{
	/* DRE: Data Register Empty */
	SerialDebugPrint((uint8_t*)"Modem Tx Data Reg Empty CallBack\r\n",33);
	//SERCOM3->USART.INTENCLR.bit.DRE = 1;
	hri_sercomusart_clear_interrupt_DRE_bit(SERCOM3);
}

void SERCOM3_1_Handler(void )
{
	SerialDebugPrint((uint8_t*)"Modem Tx Complete Callback\r\n",19);
	hri_sercomusart_clear_interrupt_TXC_bit(SERCOM3);
}

void SERCOM3_2_Handler( void )
{
	SerialDebugPrint((uint8_t*)"Modem Rx Complete CallBack\r\n",19);
	hri_sercomusart_clear_interrupt_RXC_bit(SERCOM3);
	
	while (!_usart_async_is_byte_received(&MODEM_DATA));
	
	RxDataBuff[RxCnt] = _usart_async_read_byte(&MODEM_DATA);
	RxCnt++;
	
	if(RxCnt > 1)
	{
		RxCnt = 0;
	}
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
