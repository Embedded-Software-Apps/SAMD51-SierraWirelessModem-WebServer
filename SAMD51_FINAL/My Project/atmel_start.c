#include <atmel_start.h>
#include "Apps/SerialDebug/SerialDebug.h"

WdtDelay cfgSystemWdt[3] =
{
    {WDT_TIMEOUT_FAST,500},
    {WDT_TIMEOUT_NORMAL,4096},
    {WDT_TIMEOUT_DELAYED,10000},
};

/**
 * Initializes MCU, drivers and middleware in the project
**/
void atmel_start_init(void)
{
    system_init();
    delay_ms(1000);

    /* Enable the WDT with 1 second timeout */
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
	configureWatchDogTimeOut(WDT_TIMEOUT_NORMAL);
    
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
* NAME       : enableWatchDogTimer
*
* DESCRIPTION: Enable the WDT
*
********************************************************************************/
void kickWatchDog(void)
{
	wdt_feed(&WDT_0);
}

