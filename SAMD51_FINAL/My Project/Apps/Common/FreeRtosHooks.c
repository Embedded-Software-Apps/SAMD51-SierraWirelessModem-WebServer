/*
 * FreeRtosHooks.c
 *
 * Created: 6/18/2018 4:46:32 PM
 *  Author: anilj
 */ 
#include "Apps/Common/Common.h"

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	uint8_t dbgBuffer[30];
	uint8_t taskNameBuffer[30];
	
	memset(dbgBuffer,'0',30);
    memset(taskNameBuffer,'0',30);
	DEBUG_PRINT("**********************************************************");
	DEBUG_PRINT("***************STACK OVERFLOW DETECTED********************");
	DEBUG_PRINT("**********************************************************");
	DEBUG_PRINT("\r\n");
	sprintf((int8_t*)dbgBuffer,"Task Handle - %d\r\n",xTask);
	SerialDebugPrint(dbgBuffer,strlen(dbgBuffer));
	memcpy(taskNameBuffer,pcTaskName,strlen(pcTaskName)+1);
	SerialDebugPrint(taskNameBuffer,strlen(taskNameBuffer));

	while(1);
}
