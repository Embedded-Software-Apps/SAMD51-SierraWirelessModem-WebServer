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

void sensorAdcInitialize(void)
{
	ADC_0_init();
}

int32_t sensorAdcReadChannel(const uint8_t channel, uint8_t *const buffer,const uint16_t length)
{
	int32_t bytesRead = 0;
/*	adc_sync_enable_channel(&ADC_0, channel);
	adc_sync_set_inputs(&ADC_0, channel, 0x19, channel);
	bytesRead = adc_sync_read_channel(&ADC_0, channel, &buffer[0], length);
	adc_sync_disable_channel(&ADC_0, channel);*/

	_adc_async_enable_channel(&SENSOR_ADC, channel);
	_adc_async_set_inputs(&SENSOR_ADC, channel, 0x19, channel);
	_adc_async_convert(&SENSOR_ADC);

	return bytesRead;
}

void ADC0_0_Handler(void)
{
	DEBUG_PRINT("IN ADC0_0 handler");
}

void ADC0_1_Handler(void)
{
	DEBUG_PRINT("IN ADC0_1 handler");
}
