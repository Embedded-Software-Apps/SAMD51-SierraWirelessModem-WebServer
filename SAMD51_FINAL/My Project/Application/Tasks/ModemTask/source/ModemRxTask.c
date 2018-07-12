/*
 * ModemRxTask.c
 *
 * Created: 6/3/2018 11:18:52 PM
 *  Author: anilj
 */ 
#include "Application/SerialDebug/SerialDebug.h"
#include "Application/Common/Common.h"
#include "Application/Tasks/ModemTask/include/ModemPowerControl.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/projdefs.h"
#include "Application/Tasks/ModemTask/include/ModemCmdParser.h"

void ModemRxTask( void *ModemTaskParam)
{
	const TickType_t xDelayMs = pdMS_TO_TICKS(3000UL);
	const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(1000);
	BaseType_t xResult;
	AT_CMD_TYPE atCmd;
	MODEM_CMD_DATA cmdData;

	while(1)
	{
		/* Wait to receive a notification sent directly to this task from the
		interrupt handler. */
		xResult = xTaskNotifyWait(0,ULONG_MAX,&atCmd,portMAX_DELAY);
		
		if(xResult == pdPASS)
		{
			getModemCommandData(atCmd, &cmdData);
			//DEBUG_PRINT("Notification Received to Rx Task from ISR");

			if(false != mdmParser_solicitedCmdParser(cmdData.AtCmd))
			{
				/* Last Command is parsed successfully */
			}
			else
			{
				DEBUG_PRINT("ERROR: Command Parsing Failed");
				requestWatchDogForcedReset();
			}
		}
		else
		{
			/* If this part of the function is reached then an interrupt did not
			arrive within the expected time, and (in a real application) it may
			be necessary to perform some error recovery operations. */
		}
		kickWatchDog();
	}
}
