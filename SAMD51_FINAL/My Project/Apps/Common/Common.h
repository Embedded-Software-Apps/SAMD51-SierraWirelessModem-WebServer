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


typedef enum
{
	ZERO=0,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	TEN,
	ELEVEN,
	TWELEVE,
	THIRTEEN,
	FOURTEEN,
	FIFTEEN,
	SIXTEEN,
	SEVENTEEN,
	EIGHTEEN,
	NINETEEN,
	TWENTY,
	TWENTY_ONE,
	TWENTY_TWO,
	TWENTY_THREE,
	TWENTY_FOUR,
	TWENTY_FIVE,
	TWENTY_SIX,
	TWENTY_SEVEN,
	TWENTY_EIGHT,
	TWENTY_NINE,
	THIRTY,
	THIRTY_ONE,
	THIRTY_TWO,
	THIRTY_THREE,
	THIRTY_FOUR,
	THIRTY_FIVE,
	THIRTY_SIX,
	THIRTY_SEVEN,
	THIRTY_EIGHT,
	THIRTY_NINE,
	FOURTY

}INTEGER_T;


#define VERIFIED_EQUAL (0)

bool PowerOnSuccessfull;

#endif /* COMMON_H_ */
