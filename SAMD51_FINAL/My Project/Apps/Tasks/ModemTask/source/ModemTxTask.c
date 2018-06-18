/*
 * ModemTask.c
 *
 * Created: 4/19/2018 12:07:42 PM
 *  Author: anilj
 */ 
#include "Apps/Common/Common.h"
#include "Apps/SerialDebug/SerialDebug.h"

void modemTxTaskSchedule(void)
{
	const TickType_t xDelayMs = pdMS_TO_TICKS(500UL);
	DEBUG_PRINT("Entering Modem Tx Task");

	while(1)
	{
		DEBUG_PRINT("Running Modem Tx Task");
		vTaskDelay(xDelayMs);
	}
}
