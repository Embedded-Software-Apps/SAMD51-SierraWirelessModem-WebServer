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

#define DISPATCH_TIMER_LOAD_VALUE pdMS_TO_TICKS(6000)

void DispatchAutoTimerCallBack(void* param);

char printBuffer[40];
int dispatchCnt;
BaseType_t xQueuePushStatus;
Message_Type msg1 = {SENSOR_0,100,200,30};



void dispatchTaskSchedule(void)
{
	const TickType_t xDelayMs = pdMS_TO_TICKS(1000UL);
	DEBUG_PRINT("Entering Dispatch Task");

	while(1)
	{
		DEBUG_PRINT("Running Dispatch Task");
		vTaskDelay(xDelayMs);
	}
}


void DispatchTask( void *DispatchTaskParam)
{
    /* Create the 6 second periodic auto reload timer.
     * The various data such as sensor ADC, pulse Counter and battery level
     * will be read and transmitted to modem task on every seconds.
     */
    TimerHandle_t xAutoReloadDispatchTimer;
    BaseType_t xDispatchTimerStarted;
    
    xAutoReloadDispatchTimer = xTimerCreate("DispatchPeriodicTimer",DISPATCH_TIMER_LOAD_VALUE,pdTRUE,0,DispatchAutoTimerCallBack);
    
    if(xAutoReloadDispatchTimer != NULL)
    {
        SerialDebugPrint((uint8_t*)"Dispatch timer Created.\r\n",25);
        xDispatchTimerStarted = xTimerStart(xAutoReloadDispatchTimer,0);
        
        if(xDispatchTimerStarted == pdPASS)
        {
            SerialDebugPrint((uint8_t*)"Dispatch timer Started.\r\n",25);
        }
        else
        {
            SerialDebugPrint((uint8_t*)"Dispatch timer not Started.\r\n",28);
        }   
    }
    else
    {
        SerialDebugPrint((uint8_t*)"Dispatch timer can't be Created.\r\n",35);
    }
        
    while(1)
    {

    }

}

void DispatchAutoTimerCallBack(void* param)
{
    SerialDebugPrint((uint8_t*)"Dispatch Auto Timer Call Back.\r\n",32);
    
#if 1
    xQueuePushStatus = xQueueSendToBack( xDataQueue,&msg1, 0 );
    
    if(xQueuePushStatus == pdPASS)
    {
        SerialDebugPrint((uint8_t*)"Queue Push Successful.\r\n",24);
    }
    else
    {
        SerialDebugPrint((uint8_t*)"Queue Push not Successful.\r\n",28);
    }
#endif
}
