/*
 * SensorTask.c
 *
 * Created: 4/19/2018 12:09:05 PM
 *  Author: anilj
 */ 
#include <hal_delay.h>
#include <stdio.h>
#include <string.h>
#include "Application/Common/Common.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/FreeRTOS.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/timers.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/task.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"
#include "Application/SerialDebug/SerialDebug.h"


void SensorTask( void *SensorTaskParam)
{
	TickType_t xLastWakeTime;
	const TickType_t xDelayMs = pdMS_TO_TICKS(700UL);
	const TickType_t xDebugPrintDelayMs = pdMS_TO_TICKS(500UL);
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		kickWatchDog();
		vTaskDelayUntil( &xLastWakeTime, xDelayMs);
	}
}