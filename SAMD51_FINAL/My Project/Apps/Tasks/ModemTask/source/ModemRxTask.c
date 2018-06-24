/*
 * ModemRxTask.c
 *
 * Created: 6/3/2018 11:18:52 PM
 *  Author: anilj
 */ 
#include "Apps/SerialDebug/SerialDebug.h"
#include "Apps/Common/Common.h"
#include "apps/Tasks/ModemTask/include/ModemPowerControl.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/projdefs.h"
#include "Apps/Tasks/ModemTask/include/ModemCmdParser.h"

static uint8_t responseDataBuffer[MAX_RESPONSE_SIZE];

void ModemRxTask( void *ModemTaskParam)
{
	const TickType_t xDelayMs = pdMS_TO_TICKS(3000UL);
	const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(500);
	BaseType_t xResult;
	AT_CMD_TYPE atCmd;
	MODEM_CMD_DATA cmdData;

	while(1)
	{
		/* Wait to receive a notification sent directly to this task from the
		interrupt handler. */
		xResult = xTaskNotifyWait(0,ULONG_MAX,&atCmd,xMaxExpectedBlockTime);
		getModemCommandData(atCmd, &cmdData);

		if(xResult == pdPASS)
		{
			DEBUG_PRINT("Notification Received to Rx Task");
			ConsoleDebugPrint("Response length",cmdData.ResponseLength);

			if(false != mdmParser_solicitedCmdParser(cmdData.AtCmd,responseDataBuffer))
			{
				cmdData.respHandler(responseDataBuffer,cmdData.validDataCnt);
				mdmParser_SetLastCmdProcessed(true);
			}
			else
			{
				DEBUG_PRINT("Outside solicited parser");
			}
		}
		else
		{
			/* If this part of the function is reached then an interrupt did not
			arrive within the expected time, and (in a real application) it may
			be necessary to perform some error recovery operations. */
			DEBUG_PRINT("Notification Not Received to Rx Task");
		}
		kickWatchDog();
	}
}
