/*
 * SensorTask.c
 *
 * Created: 4/19/2018 12:09:05 PM
 *  Author: anilj
 */ 
#include <hal_delay.h>
#include <stdio.h>
#include <string.h>
#include "Application/Common/Common.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/FreeRTOS.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/timers.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/task.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"
#include "Application/SerialDebug/SerialDebug.h"
#include "Application/JsonParser/include/JsonPacket.h"
#include "Application/SelectInputs/include/selectInputs.h"
#include "Application/SensorAdcInputs/include/SensorAdcInputs.h"
#include "Application/Tasks/SensorTask/include/SensorTask.h"

static void initializeSensorTask(void);
static void initializeSensorOutputData(void);
static void initializeSensorInputData(void);
/******************************************************************************************
************************************STATIC VARIABLES***************************************
*******************************************************************************************/
static SENSOR_OUTPUT_DATA_TYPE sensorOutputData[MAX_SENSOR_COUNT];
static SENSOR_INPUT_DATA_TYPE sensorInputData[MAX_SENSOR_COUNT];
static SENSOR_MAIN_STATES_T sensorMainState;
static SENSOR_DATA_PACK sensorDataPack[MAX_SENSOR_COUNT];
/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
void SensorTask( void *SensorTaskParam)
{
	TickType_t xLastWakeTime;
	const TickType_t xDelayMs = pdMS_TO_TICKS(5000UL);
	xLastWakeTime = xTaskGetTickCount();

	initializeSensorTask();

	while(1)
	{
		sensorTaskSchedule();
		kickWatchDog();
		//vTaskDelayUntil( &xLastWakeTime, xDelayMs);
	}
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
static void initializeSensorTask(void)
{
	/* Initialize the sensor select line GPIO */
	intializeSensorSelectLines();

	/* Initialize the ADC 0 module and the ADC channels */
	sensorAdcInitialize();

	initializeSensorInputData();
	initializeSensorOutputData();
	sensorMainState = WAIT_FOR_TRIGGER_FROM_PROCESS_TASK;
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
static void initializeSensorOutputData(void)
{
	SENSOR_INDEX_T sensorIndex = 0;

	for(sensorIndex = SENSOR_0; sensorIndex < MAX_SENSOR_COUNT; sensorIndex++)
	{
		sensorOutputData[sensorIndex].sensor   = sensorIndex;
		sensorOutputData[sensorIndex].active   = false;
		sensorOutputData[sensorIndex].adcCount = DEFAULT_VALUE;
		sensorOutputData[sensorIndex].voltage  = DEFAULT_VALUE;
	}
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
static void initializeSensorInputData(void)
{
	SENSOR_INDEX_T sensorIndex = 0;

	for(sensorIndex = SENSOR_0; sensorIndex < MAX_SENSOR_COUNT; sensorIndex++)
	{
		switch(sensorIndex)
		{
			case SENSOR_0:
			{
				sensorInputData[SENSOR_0].selectLine = SELECT_0;
				sensorInputData[SENSOR_0].adcChannel = SENSOR_0_AIN0;

				/* Update the sensor pack data */
				sensorDataPack[SENSOR_0].sensorIndex  = SENSOR_0;
				sensorDataPack[SENSOR_0].adcString[0] = 'A';
				sensorDataPack[SENSOR_0].adcString[1] = '0';
				sensorDataPack[SENSOR_0].adcString[2] = '\0';
			}
			break;

			case SENSOR_1:
			{
				sensorInputData[SENSOR_1].selectLine = SELECT_1;
				sensorInputData[SENSOR_1].adcChannel = SENSOR_1_AIN1;

				/* Update the sensor pack data */
				sensorDataPack[SENSOR_1].sensorIndex  = SENSOR_1;
				sensorDataPack[SENSOR_1].adcString[0] = 'A';
				sensorDataPack[SENSOR_1].adcString[1] = '1';
				sensorDataPack[SENSOR_1].adcString[2] = '\0';
			}
			break;

			case SENSOR_2:
			{
				sensorInputData[SENSOR_2].selectLine = SELECT_2;
				sensorInputData[SENSOR_2].adcChannel = SENSOR_2_AIN2;

				/* Update the sensor pack data */
				sensorDataPack[SENSOR_2].sensorIndex  = SENSOR_2;
				sensorDataPack[SENSOR_2].adcString[0] = 'A';
				sensorDataPack[SENSOR_2].adcString[1] = '2';
				sensorDataPack[SENSOR_2].adcString[2] = '\0';
			}
			break;

			case SENSOR_3:
			{
				sensorInputData[SENSOR_3].selectLine = SELECT_3;
				sensorInputData[SENSOR_3].adcChannel = SENSOR_3_AIN3;

				/* Update the sensor pack data */
				sensorDataPack[SENSOR_3].sensorIndex  = SENSOR_3;
				sensorDataPack[SENSOR_3].adcString[0] = 'A';
				sensorDataPack[SENSOR_3].adcString[1] = '3';
				sensorDataPack[SENSOR_3].adcString[2] = '\0';
			}
			break;

			case SENSOR_4:
			{
				sensorInputData[SENSOR_4].selectLine = SELECT_4;
				sensorInputData[SENSOR_4].adcChannel = SENSOR_4_AIN4;

				/* Update the sensor pack data */
				sensorDataPack[SENSOR_4].sensorIndex  = SENSOR_4;
				sensorDataPack[SENSOR_4].adcString[0] = 'A';
				sensorDataPack[SENSOR_4].adcString[1] = '4';
				sensorDataPack[SENSOR_4].adcString[2] = '\0';
			}
			break;

			case SENSOR_5:
			{
				sensorInputData[SENSOR_5].selectLine = SELECT_5;
				sensorInputData[SENSOR_5].adcChannel = SENSOR_5_AIN5;

				/* Update the sensor pack data */
				sensorDataPack[SENSOR_5].sensorIndex  = SENSOR_5;
				sensorDataPack[SENSOR_5].adcString[0] = 'A';
				sensorDataPack[SENSOR_5].adcString[1] = '5';
				sensorDataPack[SENSOR_5].adcString[2] = '\0';
			}
			break;

			case SENSOR_6:
			{
				sensorInputData[SENSOR_6].selectLine = SELECT_6;
				sensorInputData[SENSOR_6].adcChannel = SENSOR_6_AIN6;

				/* Update the sensor pack data */
				sensorDataPack[SENSOR_6].sensorIndex  = SENSOR_6;
				sensorDataPack[SENSOR_6].adcString[0] = 'A';
				sensorDataPack[SENSOR_6].adcString[1] = '6';
				sensorDataPack[SENSOR_6].adcString[2] = '\0';
			}
			break;

			case SENSOR_7:
			{
				sensorInputData[SENSOR_7].selectLine = SELECT_7;
				sensorInputData[SENSOR_7].adcChannel = SENSOR_7_AIN7;

				/* Update the sensor pack data */
				sensorDataPack[SENSOR_7].sensorIndex  = SENSOR_7;
				sensorDataPack[SENSOR_7].adcString[0] = 'A';
				sensorDataPack[SENSOR_7].adcString[1] = '7';
				sensorDataPack[SENSOR_7].adcString[2] = '\0';
			}
			break;

			default:
			break;
		}
	}
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
void sensorTaskSchedule(void)
{
	BaseType_t xResult;
	static SENSOR_INDEX_T sensorIndex = SENSOR_0;
	SENSOR_DATA_REQUEST_TYPE request;
	const TickType_t xSensorScanTriggerWaitMs = pdMS_TO_TICKS(500UL);
	uint8_t adcBuffer[2];
	int32_t bytesRead;

	switch(sensorMainState)
	{
		case WAIT_FOR_TRIGGER_FROM_PROCESS_TASK:
		{
			xResult = xSemaphoreTake(SensorScanSemaphore, xSensorScanTriggerWaitMs);

			if(xResult == pdPASS)
			{
				sensorMainState = SCAN_ALL_OF_THE_SENSOR_SELECT_LINES;
			}
		}
		break;

		case SCAN_ALL_OF_THE_SENSOR_SELECT_LINES:
		{
			if(gpio_get_pin_level(sensorInputData[sensorIndex].selectLine) == false)
			{
				DEBUG_PRINT("Select Line Low");

				/* Update the active flag for the active sensor */
				sensorOutputData[sensorIndex].active = true;
				sensorIndex++;

				if(sensorIndex >= MAX_SENSOR_COUNT)
				{
					sensorMainState = FETCH_ADC_READINGS_FOR_ACTIVE_SENSORS;
					sensorIndex = SENSOR_0;
				}
			}
			else
			{
				DEBUG_PRINT("Select Line High");
				sensorOutputData[sensorIndex].active = false;
				sensorIndex++;

				if(sensorIndex >= MAX_SENSOR_COUNT)
				{
					sensorMainState = FETCH_ADC_READINGS_FOR_ACTIVE_SENSORS;
					sensorIndex = SENSOR_0;
				}
			}
		}
		break;

		case FETCH_ADC_READINGS_FOR_ACTIVE_SENSORS:
		{
			if(sensorOutputData[sensorIndex].active == true)
			{
				bytesRead = sensorAdcReadChannel(sensorInputData[sensorIndex].adcChannel,adcBuffer,2);
				DEBUG_PRINT("Read ADC data");
				ConsoleDebugPrint("No of Bytes Read is ",bytesRead);
				sensorIndex++;
			}
			else
			{
				sensorIndex++;
			}

			if(sensorIndex >= MAX_SENSOR_COUNT)
			{
				sensorMainState = WAIT_FOR_TRIGGER_FROM_PROCESS_TASK;
				sensorIndex = SENSOR_0;
			}

		}
		break;

		case BUILD_DATA_FROM_THE_SENSOR_ADC_READINGS:
		{

		}
		break;

		case SEND_PACKED_SENSOR_DATA_TO_PROCESS_TASK:
		{

		}
		break;

		default:
		break;
	}
}

