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
	uint16_t adcValue;
	uint32_t voltageInMv = 0;

	adc_sync_enable_channel(&ADC_0, channel);
	adc_sync_set_inputs(&ADC_0, channel, 0x19, channel);
	bytesRead = adc_sync_read_channel(&ADC_0, channel, &buffer[0], length);
	adc_sync_disable_channel(&ADC_0, channel);

	adcValue = ((buffer[1] << 8) | (buffer[0] & 0x00FF));

	ConsoleDebugPrint("Sensor",channel+1);
	printAdcValueToConsole("ADC Count", adcValue);
	voltageInMv = ((adcValue * ADC_CONVERTION_FACTOR)/10000);
	printVoltageToConsole("Voltage on analog pin",voltageInMv);
	DEBUG_PRINT("\r\n");

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
