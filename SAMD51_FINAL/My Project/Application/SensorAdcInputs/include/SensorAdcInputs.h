/*
 * SensorAdcInputs.h
 *
 * Created: 4/19/2018 1:28:08 PM
 *  Author: anilj
 */ 


#ifndef SENSORADCINPUTS_H_
#define SENSORADCINPUTS_H_

/*
 * ADC conversion factor is actually (1.8/4096).
 *
 * Padded with zeros for fixed point arithmetic
 * to avoid floating points.
 */
#define ADC_CONVERTION_FACTOR ((18000000)/4096)
#define MAX_ADC_SAMPLES (1)

// ADC0.00 - ADC0.07
// 220K + 100K divider = 320/100, 16 bits
#define VOLT_SCALE_ADC0 (1.8/65536.0*3.2)

void sensorAdcInitialize(void);
int32_t sensorAdcReadChannel(const uint8_t channel, uint16_t* result,const uint16_t length);



#endif /* SENSORADCINPUTS_H_ */
