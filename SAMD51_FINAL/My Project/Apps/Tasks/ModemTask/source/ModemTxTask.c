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

/******************************************************************************************
************************************STATIC VARIABLES***************************************
*******************************************************************************************/
static AtTxMsgType AtTxQueueReceivedData;

static void ModemTxTaskSchedule(void);
static void ModemTx_SendCommandToModem(AT_CMD_TYPE atCmd);
/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: Gets the current Modem Power State.
*
********************************************************************************/
void ModemTxTask( void *ModemTaskParam)
{
	const TickType_t xDelayMs = pdMS_TO_TICKS(1000UL);

	while(1)
	{
		if(getModemPowerStatus() == MDM_PWR_OPERATIONAL_READY_FOR_AT_CMDS)
		{
			ModemTxTaskSchedule();
			kickWatchDog();
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
static void ModemTxTaskSchedule(void)
{
	xQueueReceive( AtTransmitQueue, &AtTxQueueReceivedData, portMAX_DELAY );
	ModemTx_SendCommandToModem(AtTxQueueReceivedData.atCmd);
	DEBUG_PRINT("Transmitted a command to Modem");
}

/*============================================================================
**
** Function Name:      sendCommandToModem
**
** Description:        Transmits Data to Modem
**
**===========================================================================*/
static void ModemTx_SendCommandToModem(AT_CMD_TYPE atCmd)
{
	MODEM_CMD_DATA ModemCmdData;
	getModemCommandData(atCmd, &ModemCmdData);
	mdmCtrlr_FlushRxBuffer();
	mdmCtrlr_SendDataToModem(ModemCmdData.AtString,ModemCmdData.CmdLength);
	mdmParser_SetLastSentAtCommand(atCmd);
	mdmParser_SetLastCmdProcessed(false);
}
