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
	uint32_t ulEventsToProcess;
	uint32_t totalEvents = 0;

	while(1)
	{
		/* Wait to receive a notification sent directly to this task from the
		interrupt handler. */
		ulEventsToProcess = ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
		if( ulEventsToProcess != 0 )
		{
			/* To get here at least one event must have occurred.  Loop here
			until all the pending events have been processed (in this case, just
			print out a message for each event). */
			totalEvents = ulEventsToProcess;
			ConsoleDebugPrint("rx Count is",ulEventsToProcess);
			while( ulEventsToProcess > 0 )
			{
				//DEBUG_PRINT( "Modem Rx Task - Processing event.\r\n" );
				ulEventsToProcess--;
			}
			
		}
		else
		{
			/* If this part of the function is reached then an interrupt did not
			arrive within the expected time, and (in a real application) it may
			be necessary to perform some error recovery operations. */
		}

		DEBUG_PRINT("Running Modem Rx Task successfully");
		//vTaskDelay(xDelayMs);
	}
}
