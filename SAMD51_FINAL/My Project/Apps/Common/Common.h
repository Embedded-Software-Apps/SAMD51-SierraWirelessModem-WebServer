/*
 * Common.h
 *
 * Created: 4/20/2018 7:02:49 AM
 *  Author: anilj
 */ 


#ifndef COMMON_H_
#define COMMON_H_

#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/FreeRTOS.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"
#include <stdbool.h>

#define MAX_DATA_QUEUE_SIZE (5)

typedef enum
{
	SENSOR_0,
	SENSOR_1,
	SENSOR_2,
	SENSOR_3,
	SENSOR_4,
	SENSOR_5,
	SENSOR_6,
	SENSOR_7,
	SENSOR_8
}SENSOR_INDEX_T;

QueueHandle_t xDataQueue;

typedef struct  
{
	SENSOR_INDEX_T sensor;
	uint16_t AdcReading;
	uint32_t CounterValue;
	uint8_t BatteryLevel;	
}Message_Type;

bool PowerOnSuccessfull;

#endif /* COMMON_H_ */