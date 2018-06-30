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
#define HOURLY_RESTART_TIMER_LOAD_VALUE pdMS_TO_TICKS(3600000)

static void SystemAutoRestartTimerCallBack(void* param);

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
    const TickType_t xDelayMs = pdMS_TO_TICKS(400UL);
    const TickType_t xDebugPrintDelayMs = pdMS_TO_TICKS(500UL);
    TimerHandle_t xAutoReloadHourlyTimer;

    modemPowerStateInit();
    MdmConnect_HttpConnectionInit();

    xAutoReloadHourlyTimer = xTimerCreate("HourlySystemRestartTimer",HOURLY_RESTART_TIMER_LOAD_VALUE,pdTRUE,0,SystemAutoRestartTimerCallBack);

    if(xAutoReloadHourlyTimer != NULL)
    {
    	if(pdPASS == xTimerStart(xAutoReloadHourlyTimer,0))
    	{
    		//DEBUG_PRINT("Hourly Modem Restart timer is started");
    	}
    }

    while(1)
    {
        modemPowerSchedule();

        if(getModemPowerStatus() == MDM_PWR_OPERATIONAL_READY_FOR_AT_CMDS)
        {
            MdmConnect_HttpConnectionSchedule();
			kickWatchDog();
            vTaskDelay(xDelayMs);
        }
    }
}

static void SystemAutoRestartTimerCallBack(void* param)
{
	DEBUG_PRINT("Hourly System Restart timer expired. Rebooting the system.\r\n");
	requestWatchDogForcedReset();
}
