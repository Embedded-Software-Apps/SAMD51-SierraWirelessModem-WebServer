/*
 * SensorAdcInputs.c
 *
 * Created: 4/19/2018 1:27:58 PM
 *  Author: anilj
 */ 
#include "driver_init.h"
#include "Application/SensorAdcInputs/include/SensorAdcInputs.h"
#include "Application/Common/Common.h"
#include <hpl_adc_config.h>

/*==========================================================================================
**
** Function Name:      ModemDataCommInit
**
** Description:        Initializes the SERCOM3 UART Module for Modem Data.
**
**===========================================================================================*/
void sensorAdcInitialize(void)
{
	ADC_0_init();
}

/*==========================================================================================
**
** Function Name:      ModemDataCommInit
**
** Description:        Initializes the SERCOM3 UART Module for Modem Data.
**
**===========================================================================================*/
int32_t sensorAdcReadChannel(const uint8_t channel, uint16_t* result,const uint16_t length)
{
	int32_t bytesRead = 0;
	uint32_t voltageInMv = 0;
	uint8_t buffer[2] = {0};
	*result = 0;

	adc_sync_enable_channel(&ADC_0, channel);
	adc_sync_set_inputs(&ADC_0, channel, 0x19, channel);
	bytesRead = adc_sync_read_channel(&ADC_0, channel, &buffer[0], length);
	adc_sync_disable_channel(&ADC_0, channel);

	/* calculate the ADC reading from result register data */
	*result = ((buffer[1] << 8) | (buffer[0] & 0x00FF));

	return bytesRead;
}



/************************************************************************************************************/
/*
 * Not using ADC interrupts now.
 * These are dummy handlers for ADC interrupts.
 */
void ADC0_0_Handler(void)
{
	DEBUG_PRINT("IN ADC0_0 handler");
}

void ADC0_1_Handler(void)
{
	DEBUG_PRINT("IN ADC0_1 handler");
}
