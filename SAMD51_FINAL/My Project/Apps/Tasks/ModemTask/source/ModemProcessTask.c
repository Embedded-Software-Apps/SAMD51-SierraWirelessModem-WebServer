/*=======================================================================================
 * ModemProcessTask.c
 *
 * Handles all the major state machines for managing the modem
 *======================================================================================*/

/****************************************************************************************
 INCLUDE FILES
*****************************************************************************************/
#include <hal_delay.h>
#include <stdio.h>
#include <string.h>
#include "Apps/SerialDebug/SerialDebug.h"
#include "apps/Common/Common.h"
#include "apps/Tasks/ModemTask/include/ModemPowerControl.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/projdefs.h"

/******************************************************************************************
************************************STATIC VARIABLES***************************************
*******************************************************************************************/



/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: This function converts a given signed integer(16-bit or 32-bit)
*               into a string and returns the string.
*
********************************************************************************/
void ModemProcessTask( void *ModemTaskParam)
{
    const TickType_t xDelayMs = pdMS_TO_TICKS(5000UL);
    const TickType_t xDebugPrintDelayMs = pdMS_TO_TICKS(500UL);

    modemPowerStateInit();

    while(1)
    {
        modemPowerSchedule();

        if(getModemPowerStatus() == MDM_PWR_OPERATIONAL_READY_FOR_AT_CMDS)
        {
            if( xSemaphoreTake( DebugPrintMutex,xDebugPrintDelayMs) == pdTRUE )
            {
            	DEBUG_PRINT("Running Modem Process Task successfully");
            	xSemaphoreGive(DebugPrintMutex);
            }
			kickWatchDog();
            vTaskDelay(xDelayMs);
        }
    }
}

