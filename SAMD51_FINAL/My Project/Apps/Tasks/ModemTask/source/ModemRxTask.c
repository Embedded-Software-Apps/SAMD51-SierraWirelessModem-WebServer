/*
 * ModemRxTask.c
 *
 * Created: 6/3/2018 11:18:52 PM
 *  Author: anilj
 */ 
#include "Apps/Common/Common.h"
#include "Apps/SerialDebug/SerialDebug.h"

void modemRxTaskSchedule(void)
{
	const TickType_t xDelayMs = pdMS_TO_TICKS(1100UL);

	DEBUG_PRINT("Entering Modem Rx Task");
	while(1)
	{
		DEBUG_PRINT("Running Modem Rx Task");
		vTaskDelay(xDelayMs);
	}
}
