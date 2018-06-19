/*=======================================================================================
 * ModemPowerControl.c
 *
 * Implements the power strategies for Sierra wireless modem
 *======================================================================================*/
#include <hal_delay.h>
#include <stdio.h>
#include <string.h>
#include "Apps/SerialDebug/SerialDebug.h"
#include "Apps/Common/Common.h"
#include "apps/Tasks/ModemTask/include/ModemPowerControl.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/projdefs.h"
#include "Apps/Tasks/ModemTask/include/ModemDiagTask.h"

/******************************************************************************************
************************************STATIC VARIABLES***************************************
*******************************************************************************************/
MODEM_DIAG_STATES_T ModemDiagState;


static void ModemDiagInit(void);
static void ModemDiagSchedule(void);
/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: Gets the current Modem Power State.
*
********************************************************************************/
void ModemDiagTask( void *ModemTaskParam)
{
	const TickType_t xDelayMs = pdMS_TO_TICKS(3500UL);
	ModemDiagInit();

	while(1)
	{
		if(getModemPowerStatus() == MDM_PWR_OPERATIONAL_READY_FOR_AT_CMDS)
		{
			ModemDiagSchedule();
			DEBUG_PRINT("Running Diag Process Task successfully");
			vTaskDelay(xDelayMs);
		}
	}
}

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: Gets the current Modem Power State.
*
********************************************************************************/
static void ModemDiagInit(void)
{
	ModemDiagState = MODEM_DIAG_TEST_AT;
}

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: Gets the current Modem Power State.
*
********************************************************************************/
static void ModemDiagSchedule(void)
{
	const TickType_t QueuePushDelayMs = pdMS_TO_TICKS(100UL);
	const TickType_t DiagDelayMs = pdMS_TO_TICKS(500UL);
	BaseType_t TxQueuePushStatus;
	AtTxMsgType TxMsgQueueData;

	switch(ModemDiagState)
	{
		case MODEM_DIAG_TEST_AT:
		{
			if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
			{
				if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
				{
					TxMsgQueueData.atCmd = CMD_AT_CGSN;
					TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

					if(TxQueuePushStatus == pdPASS)
					{
						DEBUG_PRINT("Sent the Diag data to Tx Task");
						xSemaphoreGive(AtTxQueueLoadSemaphore);
						vTaskDelay(DiagDelayMs);
						ModemDiagState = MODEM_DIAG_GET_IMEI;
					}
					else
					{
						DEBUG_PRINT("Failed to sent the Diag data to Tx Task");
						vTaskDelay(DiagDelayMs);
					}
				}
				else
				{
					DEBUG_PRINT("Couldn't obtain the semaphore");
				}
			}

		}
		break;

		case MODEM_DIAG_GET_IMEI:
		{

		}
		break;

		case MODEM_DIAG_GET_SERIAL:
		{

		}
		break;

		case MODEM_DIAG_GET_CARRIER:
		{

		}
		break;

	}
}
