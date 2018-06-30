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
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/task.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/timers.h"
#include "Application/Tasks/ModemTask/include/ModemAtCommandSet.h"
#include "Application/SerialDebug/SerialDebug.h"
#include "semphr.h"
#include "limits.h"
#include <stdbool.h>
#include <string.h>

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

/* Task Priorities */
#define TASK_PRIORITY_1 (1)

#define TASK_PRIORITY_2 (2)

#define TASK_PRIORITY_3 (3)

/* Task Handles */

TaskHandle_t xModemProcessTaskHandle;

TaskHandle_t xModemTxTaskHandle;

TaskHandle_t xModemRxTaskHandle;

TaskHandle_t xSensorTaskHandle;

TaskHandle_t xModemDiagTaskHandle;

typedef struct  
{
	SENSOR_INDEX_T sensor;
	uint16_t AdcReading;
	uint32_t CounterValue;
	uint8_t BatteryLevel;	
}Message_Type;


typedef enum
{
	INT_ZERO=0,
	INT_ONE,
	INT_TWO,
	INT_THREE,
	INT_FOUR,
	INT_FIVE,
	INT_SIX,
	INT_SEVEN,
	INT_EIGHT,
	INT_NINE,
	INT_TEN,
	INT_ELEVEN,
	INT_TWELEVE,
	INT_THIRTEEN,
	INT_FOURTEEN,
	INT_FIFTEEN,
	INT_SIXTEEN,
	INT_SEVENTEEN,
	INT_EIGHTEEN,
	INT_NINETEEN,
	INT_TWENTY,
	INT_TWENTY_ONE,
	INT_TWENTY_TWO,
	INT_TWENTY_THREE,
	INT_TWENTY_FOUR,
	INT_TWENTY_FIVE,
	INT_TWENTY_SIX,
	INT_TWENTY_SEVEN,
	INT_TWENTY_EIGHT,
	INT_TWENTY_NINE,
	INT_THIRTY,
	INT_THIRTY_ONE,
	INT_THIRTY_TWO,
	INT_THIRTY_THREE,
	INT_THIRTY_FOUR,
	INT_THIRTY_FIVE,
	INT_THIRTY_SIX,
	INT_THIRTY_SEVEN,
	INT_THIRTY_EIGHT,
	INT_THIRTY_NINE,
	INT_FOURTY,
	INT_FOURTY_ONE,
	INT_FOURTY_TWO,
	INT_FOURTY_THREE,
	INT_FOURTY_FOUR,
	INT_FOURTY_FIVE,
	INT_FOURTY_SIX,
	INT_FOURTY_SEVEN,
	INT_FOURTY_EIGHT,
	INT_FOURTY_NINE,
	INT_FIFTY,
	INT_FIFTY_ONE,
	INT_FIFTY_TWO,
	INT_FIFTY_THREE,
	INT_FIFTY_FOUR,
	INT_FIFTY_FIVE,
	INT_FIFTY_SIX,
	INT_FIFTY_SEVEN,
	INT_FIFTY_EIGHT,
	INT_FIFTY_NINE,
	INT_SIXTY,

}INTEGER_T;


#define VERIFIED_EQUAL (0)

bool PowerOnSuccessfull;


/* System Time Parameters */
uint32_t sysTimeFirst;
uint32_t sysTimeFinal;
uint32_t getSystemTime(void);

/* IPC Mechanisms */
QueueHandle_t AtTransmitQueue;
QueueHandle_t AtReceiveQueue;
QueueHandle_t CmdResponseQueue;
SemaphoreHandle_t AtTxQueueLoadSemaphore;
SemaphoreHandle_t DebugPrintMutex;

#define MAX_TX_QUEUE_SIZE (1)
#define MAX_RX_QUEUE_SIZE (1)
#define MAX_CMD_RESP_QUEUE_SIZE (1)

typedef enum
{
	OP_TX_MODE,
	OP_RX_MODE
}TASK_OPERATION_MODE;

bool createQueuesAndSemaphores(void);
#endif /* COMMON_H_ */
