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
Message_Type ReceivedMsg;
TickType_t xTicksToWait = pdMS_TO_TICKS(6100);
char printBuff[50];



void modemProcessTaskSchedule(void)
{
	const TickType_t xDelayMs = pdMS_TO_TICKS(5000UL);
	DEBUG_PRINT("Entering Modem Process Task");

	while(1)
	{
		DEBUG_PRINT("Running Modem Process Task");
		vTaskDelay(xDelayMs);
	}

}

































































void ModemTask( void *ModemTaskParam)
{
	BaseType_t xQueueReceiveStatus;
	modemPwrInit();
	
	while(1)
	{
		/* Execute the Modem Power Control State Machine */
		modemPwrStateSchedule();
		
		ModemControllerSchedule();
		
		#if 1
		if( uxQueueMessagesWaiting( xDataQueue ) != 0 )
		{
			SerialDebugPrint((uint8_t*)"Modem Task Queue should have been empty.\r\n",43);
		}
		
		xQueueReceiveStatus = xQueueReceive( xDataQueue, &ReceivedMsg, xTicksToWait );
		
		if(xQueueReceiveStatus == pdPASS)
		{
			sprintf(printBuff,"Received ADC Value is %d\r\n",ReceivedMsg.AdcReading);
			SerialDebugPrint((uint8_t*)printBuff,sizeof(printBuff));
		}
		else
		{
			SerialDebugPrint((uint8_t*)"Queue Receive Failed\r\n",22);
		}
		#endif

	}

}
