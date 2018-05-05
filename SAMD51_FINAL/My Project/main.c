#include <atmel_start.h>
#include <hal_delay.h>

#include "Apps/SerialDebug/SerialDebug.h"
#include "Apps/Tasks/DispatchTask/include/DispatchTask.h"
#include "Apps/Tasks/ModemTask/include/ModemTask.h"
#include "Apps/LedControl/include/ledControl.h"
#include "Apps/Common/Common.h"
#include "Apps/Tasks/ModemTask/include/ModemPowerControl.h"
#include "Apps/Tasks/ModemTask/include/ModemController.h"
#include <string.h>

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

BaseType_t DispatchTaskStatus;
BaseType_t ModemTaskStatus;
uint8_t printBuff[50];

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	/* Initialize the HL7618RD modem power signals */
	modemPowerInit();
	
	while (1)
	{
		/* Transmit "AT\r" to modem on every 4 seconds...
		 * If we receive any char from modem, debug message
		 * will be printed on the console saying data received
		 * from modem.
		 */
		
		/* Transmit data register empty interrupt is disabled after
		 * transmitting the first message to modem so that we can see
		 * the debug message in serial terminal if any data received back
		 * from modem.
		 * Only the TX data register interrupt is disabled. 
		 * Rx data complete Interrupt is still active and the corresponding
		 * call back will be called if any data received from modem */
		
		sendCommandToModem((uint8_t*)"AT\r",3);
		delay_ms(4000);
	}
}





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