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

void sensorAdcReadChannel(const uint8_t channel, uint8_t *const buffer,const uint16_t length)
{
	 adc_sync_enable_channel(&ADC_0, channel);
	 adc_sync_set_inputs(&ADC_0, channel, 0x18, channel);
	 adc_sync_read_channel(&ADC_0, channel, &buffer[0], length);
	 adc_sync_disable_channel(&ADC_0, channel);
}


