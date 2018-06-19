/*
 * DispatchTask.c
 *
 * Created: 4/19/2018 12:09:05 PM
 *  Author: anilj
 */ 
#include <hal_delay.h>
#include <stdio.h>
#include <string.h>
#include "Apps/Common/Common.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/FreeRTOS.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/timers.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/task.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"
#include "Apps/SerialDebug/SerialDebug.h"


void DispatchTask( void *DispatchTaskParam)
{
	TickType_t xLastWakeTime;
	const TickType_t xDelayMs = pdMS_TO_TICKS(10000UL);
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		DEBUG_PRINT("Running Dispatch Task successfully");
		vTaskDelayUntil( &xLastWakeTime, xDelayMs);
	}
}
