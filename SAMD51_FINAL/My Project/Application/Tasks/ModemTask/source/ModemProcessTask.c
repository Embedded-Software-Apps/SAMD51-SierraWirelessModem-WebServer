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
#include "Application/SerialDebug/SerialDebug.h"
#include "Application/Common/Common.h"
#include "Application/Tasks/ModemTask/include/ModemPowerControl.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/projdefs.h"

/******************************************************************************************
************************************STATIC VARIABLES***************************************
*******************************************************************************************/
#define HOURLY_RESTART_TIMER_LOAD_VALUE pdMS_TO_TICKS(3600000)
#define PACKET_SEND_TIMER_LOAD_VALUE pdMS_TO_TICKS(6000)
static bool packetTransmitPeriodicTimerExpired;

static void SystemAutoRestartTimerCallBack(void* param);
static void packetTransmitPeriodicTimerCallBack(void* param);
static bool initializeModemTimers(void);
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
    modemPowerStateInit();
    MdmConnect_HttpConnectionInit();

    if(false == initializeModemTimers())
    {
    	/*
    	 *  Not able to initialize the modem timers.
    	 *  Reboot the system.
    	 */
    	DEBUG_PRINT("Failed to initialize the modem timers. Rebooting the system");
    	requestWatchDogForcedReset();
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

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: This function converts a given signed integer(16-bit or 32-bit)
*               into a string and returns the string.
*
********************************************************************************/
static bool initializeModemTimers(void)
{
    TimerHandle_t xAutoReloadHourlyTimer;
    TimerHandle_t xPacketSendPeriodicTimer;
    bool status = false;
    packetTransmitPeriodicTimerExpired = false;

    xAutoReloadHourlyTimer   = xTimerCreate("HourlySystemRestartTimer",HOURLY_RESTART_TIMER_LOAD_VALUE,pdTRUE,0,SystemAutoRestartTimerCallBack);
    xPacketSendPeriodicTimer = xTimerCreate("PacketTransmitPeriodicTimer",PACKET_SEND_TIMER_LOAD_VALUE,pdTRUE,0,packetTransmitPeriodicTimerCallBack);

    if((xPacketSendPeriodicTimer != NULL) &&
       (xAutoReloadHourlyTimer != NULL))
    {
    	if((pdPASS == xTimerStart(xAutoReloadHourlyTimer,0)) &&
		   (pdPASS == xTimerStart(xPacketSendPeriodicTimer,0)))
    	{
    		DEBUG_PRINT("Modem timers are started");
    		status = true;
    	}
    }

    return status;
}

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: This function converts a given signed integer(16-bit or 32-bit)
*               into a string and returns the string.
*
********************************************************************************/
static void packetTransmitPeriodicTimerCallBack(void* param)
{
	packetTransmitPeriodicTimerExpired = true;
}

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: This function converts a given signed integer(16-bit or 32-bit)
*               into a string and returns the string.
*
********************************************************************************/
void clearPacketTransmitTimerExpiryFlag(void)
{
	packetTransmitPeriodicTimerExpired = false;
}

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: This function converts a given signed integer(16-bit or 32-bit)
*               into a string and returns the string.
*
********************************************************************************/
bool isPacketTransmitTimerExpired(void)
{
	return packetTransmitPeriodicTimerExpired;
}

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: This function converts a given signed integer(16-bit or 32-bit)
*               into a string and returns the string.
*
********************************************************************************/
static void SystemAutoRestartTimerCallBack(void* param)
{
	DEBUG_PRINT("Hourly System Restart timer expired. Rebooting the system.\r\n");
	requestWatchDogForcedReset();
}
