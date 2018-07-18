/*
 * SensorAdcInputs.h
 *
 * Created: 4/19/2018 1:28:08 PM
 *  Author: anilj
 */ 


#ifndef SENSORADCINPUTS_H_
#define SENSORADCINPUTS_H_

#define ADC_CONVERTION_FACTOR ((18000000)/4096)
#define MAX_ADC_SAMPLES (1)

void sensorAdcInitialize(void);
int32_t sensorAdcReadChannel(const uint8_t channel, uint16_t* result,const uint16_t length);



#endif /* SENSORADCINPUTS_H_ */
