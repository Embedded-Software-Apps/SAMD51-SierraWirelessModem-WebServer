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

    /* Create Dispatch Task */
    DispatchTaskStatus = xTaskCreate( DispatchTask, "DispatchTask", 150, NULL, 2, xDispatchTaskHandle );

    /* Create Modem Tx Task */
    ModemTxTaskStatus = xTaskCreate( ModemTxTask, "ModemTask", 150, NULL, 1, xModemTxTaskHandle );

    /* Create Modem Rx Task */
    ModemRxTaskStatus = xTaskCreate( ModemRxTask, "ModemRxTask", 150, NULL, 2, xModemRxTaskHandle);

    /* Create Modem Process Task */
    ModemProcessTaskStatus = xTaskCreate( ModemProcessTask, "ModemProcessTask", 150, NULL, 1, xModemProcessTaskHandle);

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

    DEBUG_PRINT("Error: Scheduler exited");
    /* The execution won't reach here ideally */
    for( ;; );

    return 0;
}
