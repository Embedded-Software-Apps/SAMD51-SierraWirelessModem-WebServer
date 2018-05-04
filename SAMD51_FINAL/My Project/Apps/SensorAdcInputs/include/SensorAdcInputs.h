/*
 * SensorAdcInputs.h
 *
 * Created: 4/19/2018 1:28:08 PM
 *  Author: anilj
 */ 


#ifndef SENSORADCINPUTS_H_
#define SENSORADCINPUTS_H_

void sensorAdcInit(void);
void sensorAdcReadChannel(const uint8_t channel, uint8_t *const buffer,const uint16_t length);



#endif /* SENSORADCINPUTS_H_ */