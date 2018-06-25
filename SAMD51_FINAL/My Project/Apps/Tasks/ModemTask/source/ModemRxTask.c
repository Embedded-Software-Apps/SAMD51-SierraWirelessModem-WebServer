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

void ModemRxTask( void *ModemTaskParam)
{
	const TickType_t xDelayMs = pdMS_TO_TICKS(3000UL);
	const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(2000);
	BaseType_t xResult;
	AT_CMD_TYPE atCmd;
	MODEM_CMD_DATA cmdData;
	uint8_t* responseBuffer = NULL;
	AtRxMsgType AtRxQueueReceivedData;
	AtRxQueueReceivedData.atCmd = CMD_AT_MAX;

	while(1)
	{
		/* Wait to receive a notification sent directly to this task from the
		interrupt handler. */
		xResult = xQueueReceive( AtReceiveQueue, &AtRxQueueReceivedData, xMaxExpectedBlockTime );
		getModemCommandData(AtRxQueueReceivedData.atCmd, &cmdData);

		if(xResult == pdPASS)
		{
			responseBuffer = (uint8_t*)pvPortMalloc((cmdData.validDataCnt)*(sizeof(uint8_t)));

			if(responseBuffer != NULL)
			{
				DEBUG_PRINT("Notification Received to Rx Task. Memory Allocated");

				if(false != mdmParser_solicitedCmdParser(cmdData.AtCmd,responseBuffer))
				{
					cmdData.respHandler(responseBuffer,cmdData.validDataCnt);
					DEBUG_PRINT("Called Resp Handler");
					mdmParser_SetLastCmdProcessed(true);
				}
				else
				{
					DEBUG_PRINT("ERROR: Solicited Command Parsing Failed");
				}

				vPortFree(responseBuffer);
			}
			else
			{
				DEBUG_PRINT("Error: Heap Allocation Failed");
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
