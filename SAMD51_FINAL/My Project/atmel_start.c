/*=======================================================================================
 * atmel_start.c
 *
 * Implements the MCU and WDT initialization functions.
 *======================================================================================*/

/****************************************************************************************
 INCLUDE FILES
*****************************************************************************************/
#include <atmel_start.h>
#include "Application/SerialDebug/SerialDebug.h"

/******************************************************************************************
************************************STATIC VARIABLES***************************************
*******************************************************************************************/
WdtDelay cfgSystemWdt[3] =
{
    {WDT_TIMEOUT_FAST,WDT_1_SEC_TIMEOUT},
    {WDT_TIMEOUT_NORMAL,WDT_3_SEC_TIMEOUT},
    {WDT_TIMEOUT_DELAYED,WDT_10_SEC_TIMEOUT},
};

static WDT_FORCED_RESET_TYPE ForcedResetRequested = WDT_FORCED_RESET_OFF;

/*******************************************************************************
*
* NAME       : atmel_start_init
*
* DESCRIPTION: Initializes MCU, drivers and middleware in the project.
*
********************************************************************************/
void atmel_start_init(void)
{
    system_init();
    delay_ms(1000);
    ForcedResetRequested = WDT_FORCED_RESET_OFF;
    /* Enable the WDT with 10 second timeout as of now */
    enableWatchDogTimer();
}

/*******************************************************************************
*
* NAME       : configureWatchDogTimeOut
*
* DESCRIPTION: Configure the system watch dog timout.
*
********************************************************************************/
void configureWatchDogTimeOut(WDT_TIMEOUT_TYPE type)
{
	int32_t cfgstatus;
	
	cfgstatus =  wdt_set_timeout_period(&WDT_0, WDT_FREQ, cfgSystemWdt[type].timeOutValue);

	if(0 == cfgstatus)
	{
		//DEBUG_PRINT("Set the WDT timeout successfully");
	}
	else
	{
		//DEBUG_PRINT("Failed to set the WDT time out");
	}
}

/*******************************************************************************
*
* NAME       : enableWatchDogTimer
*
* DESCRIPTION: Enable the WDT
*
********************************************************************************/
void enableWatchDogTimer(void)
{
	configureWatchDogTimeOut(WDT_TIMEOUT_DELAYED);
    
	if(0 == wdt_enable(&WDT_0))
    {
    	//DEBUG_PRINT("Enabled the WDT");
    }
    else
    {
    	//DEBUG_PRINT("Failed to enable WDT");
    }
}

/*******************************************************************************
*
* NAME       : kickWatchDog
*
* DESCRIPTION: Restart the WDT
*
********************************************************************************/
int32_t kickWatchDog(void)
{	
	int32_t status = ERR_NONE;

	if(WDT_FORCED_RESET_OFF == ForcedResetRequested)
	{
		status = wdt_feed(&WDT_0);
	}
	else
	{
		status = ERR_DENIED;
	}

	return status;
}

/*******************************************************************************
*
* NAME       : kickWatchDog
*
* DESCRIPTION: Restart the WDT
*
********************************************************************************/
void requestWatchDogForcedReset(void)
{
	ForcedResetRequested = WDT_FORCED_RESET_ON;
	wdt_disable(&WDT_0);
	configureWatchDogTimeOut(WDT_TIMEOUT_FAST);
	wdt_enable(&WDT_0);
}
