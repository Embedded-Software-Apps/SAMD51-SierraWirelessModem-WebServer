/*
 * ModemTask.c
 *
 * Created: 4/19/2018 12:07:42 PM
 *  Author: anilj
 */ 
#include <hal_delay.h>
#include <stdio.h>
#include <string.h>
#include "Apps/SerialDebug/SerialDebug.h"
#include "Apps/Common/Common.h"
#include "apps/Tasks/ModemTask/include/ModemPowerControl.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/projdefs.h"


void ModemTxTask( void *ModemTaskParam)
{
	const TickType_t xDelayMs = pdMS_TO_TICKS(6000UL);

	while(1)
	{
		DEBUG_PRINT("Running Modem Tx Task successfully");
		vTaskDelay(xDelayMs);
	}
}
