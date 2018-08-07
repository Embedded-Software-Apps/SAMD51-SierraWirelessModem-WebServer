
/*=======================================================================================
 * main.c
 *
 * Starting point for the application
 *  - Initializes the drivers and middle wares.
 *  - Implements the task creation.
 *  - Starts the Free RTOS scheduler.
 *======================================================================================*/

/****************************************************************************************
 INCLUDE FILES
*****************************************************************************************/
#include <atmel_start.h>
#include <hal_delay.h>
#include <math.h>

#include "Application/SerialDebug/SerialDebug.h"
#include "Application/Tasks/SensorTask/include/SensorTask.h"
#include "Application/Tasks/ModemTask/include/ModemTxTask.h"
#include "Application/LedControl/include/ledControl.h"
#include "Application/Common/Common.h"
#include "Application/Tasks/ModemTask/include/ModemPowerControl.h"
#include "Application/Tasks/ModemTask/include/ModemController.h"
#include "Application/Tasks/ModemTask/include/ModemCmdParser.h"
#include "Application/Tasks/ModemTask/include/ModemParameters.h"
#include "Application/Tasks/ModemTask/include/ModemConnection.h"
#include "Application/Tasks/ModemTask/include/ModemRxTask.h"
#include "Application/Tasks/ModemTask/include/ModemProcessTask.h"
#include "Application/Tasks/ModemTask/include/ModemTxTask.h"
#include "Application/Tasks/ModemTask/include/ModemDiagTask.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/******************************************************************************************
************************************STATIC VARIABLES***************************************
*******************************************************************************************/
static BaseType_t SensorTaskStatus;
static BaseType_t ModemProcessTaskStatus;
static BaseType_t ModemTxTaskStatus;
static BaseType_t ModemRxTaskStatus;
static BaseType_t ModemDiagTaskStatus;

/*******************************************************************************
*
* NAME       : main
*
* DESCRIPTION:  Starting point for the application
*                - Initializes the drivers and middle wares.
*                - Implements the task creation.
*                - Starts the Free RTOS scheduler.
*
********************************************************************************/
int main(void)
{
    /* Initializes MCU, drivers and middleware */
    atmel_start_init();	

    /* Create Sensor Task */
    SensorTaskStatus = xTaskCreate( SensorTask, "SensorTask", 150, NULL, 2, &xSensorTaskHandle );

    /* Create Modem Tx Task */
    ModemTxTaskStatus = xTaskCreate( ModemTxTask, "ModemTxTask", 150, NULL, 1, &xModemTxTaskHandle );

    /* Create Modem Rx Task */
    ModemRxTaskStatus = xTaskCreate( ModemRxTask, "ModemRxTask", 150, NULL, 2, &xModemRxTaskHandle);

    /* Create Modem Process Task */
    ModemProcessTaskStatus = xTaskCreate( ModemProcessTask, "ModemProcessTask", 150, NULL, 1, &xModemProcessTaskHandle);

    /* Create Modem Diagnostics Task */
    ModemDiagTaskStatus = xTaskCreate( ModemDiagTask, "ModemDiagTask", 150, NULL, 1, &xModemDiagTaskHandle);

    /* Create FOTA Task */

    if((SensorTaskStatus == pdPASS) &&
       (ModemProcessTaskStatus == pdPASS) &&
       (ModemTxTaskStatus == pdPASS) &&
       (ModemRxTaskStatus == pdPASS) &&
	   (ModemDiagTaskStatus == pdPASS))
    {
    	if(false != createQueuesAndSemaphores())
    	{
    		vTaskStartScheduler();
    	}
    	else
    	{
    		DEBUG_PRINT("Free RTOS Scheduler not started");
    	}
    }
    else
    {
    	DEBUG_PRINT("Failed to create tasks");
    }

    DEBUG_PRINT("Error: Scheduler exited");

    /* The execution won't reach here ideally */
    for( ;; );

    return 0;
}

/*******************************************************************************
*
* NAME       : createQueuesAndSemaphores
*
* DESCRIPTION:  Creats the Queues and Semaphores
*
********************************************************************************/
bool createQueuesAndSemaphores(void)
{
	bool status = true;

    /* Create the Message Queues */
    AtTransmitQueue    = xQueueCreate(MAX_TX_QUEUE_SIZE, sizeof(AtTxMsgType));
    AtReceiveQueue     = xQueueCreate(MAX_RX_QUEUE_SIZE, sizeof(AtRxMsgType));
    CmdResponseQueue   = xQueueCreate(MAX_CMD_RESP_QUEUE_SIZE, sizeof(CmdResponseType));


    if((AtTransmitQueue != NULL) &&
       (AtReceiveQueue  != NULL) &&
	   (CmdResponseQueue != NULL))
    {
    	AtTxQueueLoadSemaphore = xSemaphoreCreateBinary();
    	SensorScanSemaphore    = xSemaphoreCreateBinary();
    	DebugPrintMutex        = xSemaphoreCreateMutex();
		
		if((AtTxQueueLoadSemaphore != NULL) &&
           (SensorScanSemaphore    != NULL) &&
		   (DebugPrintMutex        != NULL))
		{
			//DEBUG_PRINT("Semaphores and Mutexes are created");
		}
		else
		{
			status = false;
		}
    }
    else
    {
    	status = false;
    }
}

/****************************************************FOTA BRANCH******************************************************************/
