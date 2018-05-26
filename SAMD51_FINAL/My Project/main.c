#include <atmel_start.h>
#include <hal_delay.h>

#include "Apps/SerialDebug/SerialDebug.h"
#include "Apps/Tasks/DispatchTask/include/DispatchTask.h"
#include "Apps/Tasks/ModemTask/include/ModemTask.h"
#include "Apps/LedControl/include/ledControl.h"
#include "Apps/Common/Common.h"
#include "Apps/Tasks/ModemTask/include/ModemPowerControl.h"
#include "Apps/Tasks/ModemTask/include/ModemController.h"
#include "Apps/Tasks/ModemTask/include/ModemCmdParser.h"
#include "Apps/Tasks/ModemTask/include/ModemParameters.h"
#include <string.h>

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

BaseType_t DispatchTaskStatus;
BaseType_t ModemTaskStatus;
uint8_t printBuff[50];
char rxReadBuf[50];
uint8_t length;
void uartTest();

/* Mile Stone 1 */
/* Started working in Dev Branch */

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	/* Initialize the HL7618RD modem power signals */
	modemPowerInit();
	
	/* Initiate the HTTP connection to server */
	mdmParam_InitiateConnection();
	
	while (1)
	{
		
		
		uartTest();
		
	
	}
}

void uartTest()
{
		SendATCommandToModem(CMD_AT_CGSN); //359998070228764
		delay_ms(2000);
		
		memset(rxReadBuf,0,50);
		mdmParser_GetModemResponse(CMD_AT_CGSN,rxReadBuf,&length);
		
		SerialDebugPrint(rxReadBuf,length);
		SerialDebugPrint("\r\n",2);
		delay_ms(3000);
}
/* Below code needs to be enabled */


/*****************************************************************************************************
********Need to add below part to main() function, once modem communication issue is resolved*********
******************************************************************************************************/	
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
	
	
	/* The execution won't reach here ideally */
	for( ;; );
	return 0;
}
#endif
