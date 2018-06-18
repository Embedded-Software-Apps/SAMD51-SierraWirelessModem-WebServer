#include <atmel_start.h>
#include <hal_delay.h>

#include "Apps/SerialDebug/SerialDebug.h"
#include "Apps/Tasks/DispatchTask/include/DispatchTask.h"
#include "Apps/Tasks/ModemTask/include/ModemTxTask.h"
#include "Apps/LedControl/include/ledControl.h"
#include "Apps/Common/Common.h"
#include "Apps/Tasks/ModemTask/include/ModemPowerControl.h"
#include "Apps/Tasks/ModemTask/include/ModemController.h"
#include "Apps/Tasks/ModemTask/include/ModemCmdParser.h"
#include "Apps/Tasks/ModemTask/include/ModemParameters.h"
#include "Apps/Tasks/ModemTask/include/ModemConnection.h"
#include "Apps/Tasks/ModemTask/include/ModemRxTask.h"
#include "Apps/Tasks/ModemTask/include/ModemProcessTask.h"
#include "Apps/Tasks/ModemTask/include/ModemTxTask.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"
#include <string.h>

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

BaseType_t DispatchTaskStatus;
BaseType_t ModemProcessTaskStatus;
BaseType_t ModemTxTaskStatus;
BaseType_t ModemRxTaskStatus;

int main(void)
{
    /* Initializes MCU, drivers and middleware */
    atmel_start_init();

#if 1
    /* Create the tasks */
    DispatchTaskStatus     = xTaskCreate( dispatchTaskSchedule, "DispatchTask", 100, NULL, 1, NULL);
    ModemProcessTaskStatus = xTaskCreate( modemProcessTaskSchedule, "ModemProcessTask", 100, NULL, 1, NULL);
    //ModemTxTaskStatus      = xTaskCreate( modemTxTaskSchedule, "ModemTxTask", 100, NULL, 2, NULL);
    //ModemRxTaskStatus      = xTaskCreate( modemRxTaskSchedule, "ModemRxTask", 100, NULL, 2, NULL);

    ModemTxTaskStatus  = pdPASS;
    ModemRxTaskStatus = pdPASS;

    if((DispatchTaskStatus == pdPASS) &&
       (ModemProcessTaskStatus == pdPASS) &&
       (ModemTxTaskStatus == pdPASS) &&
       (ModemRxTaskStatus == pdPASS))
    {
    	DEBUG_PRINT("Successfully Created the Tasks");
    	vTaskStartScheduler();
    }
    else
    {
    	DEBUG_PRINT("Failed to create tasks");
    }
#endif

    DEBUG_PRINT("Error: Scheduler exited");
    /* The execution won't reach here ideally */
    for( ;; );

    return 0;
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
