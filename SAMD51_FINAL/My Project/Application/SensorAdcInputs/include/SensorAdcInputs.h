/*
 * SensorAdcInputs.h
 *
 * Created: 4/19/2018 1:28:08 PM
 *  Author: anilj
 */ 


#ifndef SENSORADCINPUTS_H_
#define SENSORADCINPUTS_H_

#define ADC_CONVERTION_FACTOR ((18000000)/4096)

void sensorAdcInitialize(void);
int32_t sensorAdcReadChannel(const uint8_t channel, uint8_t *const buffer,const uint16_t length);



#endif /* SENSORADCINPUTS_H_ */
