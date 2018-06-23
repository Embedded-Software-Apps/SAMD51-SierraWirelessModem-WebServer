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

void ModemRxTask( void *ModemTaskParam)
{
	const TickType_t xDelayMs = pdMS_TO_TICKS(3000UL);
	const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(3000);
	BaseType_t xResult;
	uint32_t responseLen;

	while(1)
	{
		/* Wait to receive a notification sent directly to this task from the
		interrupt handler. */
		xResult = xTaskNotifyWait(0,ULONG_MAX,&responseLen,portMAX_DELAY);

		if(xResult == pdPASS)
		{
			DEBUG_PRINT("Notification Received to Rx Task");
			ConsoleDebugPrint("Response length",responseLen);

		}
		else
		{
			/* If this part of the function is reached then an interrupt did not
			arrive within the expected time, and (in a real application) it may
			be necessary to perform some error recovery operations. */
			DEBUG_PRINT("Notification Not Received to Rx Task");
		}

		//DEBUG_PRINT("Running Modem Rx Task successfully");
		//vTaskDelay(xDelayMs);
	}
}
