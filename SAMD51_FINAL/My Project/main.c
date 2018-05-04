#include <atmel_start.h>
#include <hal_delay.h>

#include "Apps/SerialDebug/SerialDebug.h"
#include "Apps/Tasks/DispatchTask/include/DispatchTask.h"
#include "Apps/Tasks/ModemTask/include/ModemTask.h"
#include "Apps/LedControl/include/ledControl.h"
#include "Apps/Common/Common.h"
#include <string.h>

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

BaseType_t DispatchTaskStatus;
BaseType_t ModemTaskStatus;
uint8_t printBuff[50];
uint8_t writeCnt;
uint8_t readCnt;
uint8_t response[10];

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	modemPowerInit();
	
	while (1)
	{
		//ModemUsartOsTest();
	}
	
#if 0
	/* Create the Message Queue */
	xDataQueue = xQueueCreate( MAX_DATA_QUEUE_SIZE, sizeof(Message_Type));
	
	if (xDataQueue != NULL)
	{
		/* Create the Dispatch Task */
		DispatchTaskStatus = xTaskCreate( DispatchTask, "DispatchTask", 150, NULL, 2, NULL );

		/* Create the Modem Task */
		ModemTaskStatus = xTaskCreate( ModemTask, "ModemTask", 150, NULL, 2, NULL );
		
		/* Start the scheduler to start the tasks executing. */
		if((DispatchTaskStatus == pdPASS) &&
		(ModemTaskStatus == pdPASS))
		{
			SerialDebugPrint((uint8_t*)"Tasks Created successfully.\r\n",29);
			vTaskStartScheduler();
		}
		else
		{
			SerialDebugPrint((uint8_t*)"Tasks can't be Created.\r\n",25);
		}

	}
	else
	{
		SerialDebugPrint((uint8_t*)"Message Queue can't be Created.\r\n",35);
	}
	#endif
	
	/* The execution won't reach here ideally */
	for( ;; );
	return 0;
}
