/*
 * ModemProcessTask.c
 *
 * Created: 6/17/2018 8:31:21 AM
 *  Author: anilj
 */ 
#include <hal_delay.h>
#include <stdio.h>
#include <string.h>
#include "Apps/SerialDebug/SerialDebug.h"
#include "apps/Common/Common.h"
#include "apps/Tasks/ModemTask/include/ModemPowerControl.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/projdefs.h"

void ModemProcessTask( void *ModemTaskParam)
{
	const TickType_t xDelayMs = pdMS_TO_TICKS(10UL);
	DEBUG_PRINT("Entering Modem Process Task");

	while(1)
	{
		DEBUG_PRINT("Running Modem Process Task successfully");
		vTaskDelay(xDelayMs);
	}
}

