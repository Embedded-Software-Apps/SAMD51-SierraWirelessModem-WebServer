/*
 * FreeRtosHooks.c
 *
 * Created: 6/18/2018 4:46:32 PM
 *  Author: anilj
 */ 
#include "Apps/Common/Common.h"

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	uint8_t dbgBuffer[100];
	memset(dbgBuffer,'\0',100);

	DEBUG_PRINT("**********************************************************");
	DEBUG_PRINT("***************STACK OVERFLOW DETECTED********************");
	DEBUG_PRINT("**********************************************************");
	DEBUG_PRINT("\r\n");
	sprintf((int8_t*)dbgBuffer," Task Handle - %d ### Task Name - %s",xTask,pcTaskName);
	SerialDebugPrint(dbgBuffer,strlen(dbgBuffer));

	while(1);
}
