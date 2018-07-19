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
	/*
	 * ADC_0_init();
	 */

	// 256 * 2us * (6+1 + 12) --> 9.728ms/sample --> 60ms per loop --> 100 slow averages per post
	ADC0->SAMPCTRL.reg = 6; // (6+1+12) clocks per conversion.
	while(ADC0->SYNCBUSY.bit.SAMPCTRL);
	ADC0->AVGCTRL.reg = 0x08; // automatic 16 bit averaging.  0x48 gives 12 bit average
	while(ADC0->SYNCBUSY.bit.AVGCTRL);
	ADC0->REFCTRL.reg = 0x03;
	while(ADC0->SYNCBUSY.bit.REFCTRL);
	ADC0->CTRLB.reg = 0x0008; // 16 bit mode, right justified, single shot
	while(ADC0->SYNCBUSY.bit.CTRLB);
	ADC0->DBGCTRL.reg = 1;
	ADC0->INPUTCTRL.reg = 0x1900 | 0x00;  // ground vs. channel ... change 0x00 to channel
	while(ADC0->SYNCBUSY.bit.INPUTCTRL);
	ADC0->CTRLA.reg = 0x8002 | 0<<8; // rail to rail, 500KHz, enabled change 0 to prescaler
	while(ADC0->SYNCBUSY.bit.ENABLE);
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
	const TickType_t xAdcConvDelayMs = pdMS_TO_TICKS(10UL);
/*	adc_sync_enable_channel(&ADC_0, channel);
	adc_sync_set_inputs(&ADC_0, channel, 0x19, channel);
	bytesRead = adc_sync_read_channel(&ADC_0, channel, &buffer[0], length);
	adc_sync_disable_channel(&ADC_0, channel);
	*result = ((buffer[1] << 8) | (buffer[0] & 0x00FF));*/

	ADC0->SWTRIG.bit.FLUSH = 0;
	while(ADC0->SYNCBUSY.bit.SWTRIG);
	while(ADC0->STATUS.bit.ADCBUSY);

	if (channel < MAX_SENSOR_COUNT)
	{
		ADC0->INPUTCTRL.bit.MUXPOS = channel;
	}
	else
	{
		ADC0->INPUTCTRL.bit.MUXPOS = 0;
	}

	ADC0->SWTRIG.bit.START = 1;
	while(ADC0->SYNCBUSY.bit.SWTRIG);

	vTaskDelay(xAdcConvDelayMs);

	while(!(ADC0->INTFLAG.bit.RESRDY));

	*result = ADC0->RESULT.reg;

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
