/*
 * ModemController.h
 *
 * Created: 4/21/2018 8:44:24 PM
 *  Author: toani
 */ 


#ifndef MODEMCONTROLLER_H_
#define MODEMCONTROLLER_H_

#include "driver_init.h"
#include <stdint.h>
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/FreeRTOS.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/task.h"

/*Modem Main States */
typedef enum
{
	MODEM_SHUTDOWN,
	MODEM_ACTIVATING,
	MODEM_OPERATIONAL,
	MODEM_NORMAL_DEACTIVATING,
	MODEM_EMRGNCY_DEACTIVATING,
	MODEM_FAULTY
}MODEM_CONTROLLER_STATES_T;

typedef enum
{
	MDM_SET_SLEEP_SETTINGS,
	MDM_WRITE_MODE,
	MDM_READ_MODE,
	MDM_AT_TEST,
	MDM_AT_IDLE
}MDM_TEST_STATES;

void ModemDataCommInit(void);
uint32_t sendCommandToModem(const uint8_t *const TxData,const uint16_t length);
int32_t ReadResponseFromModem(uint8_t *RecvdData,const int32_t length);
void ModemDataTxByteSentCallBack(struct _usart_async_device *device);
void ModemDataTxDoneCallBack(struct _usart_async_device *device);
void ModemDataRxDoneCallBack(struct _usart_async_device *device,uint8_t data);
void ModemDataErrorCallBack(struct _usart_async_device *device);
void UsartsendCommandToModem(const uint8_t *const TxData,const uint16_t length);
int32_t UsartReadResponseFromModem(uint8_t *RecvdData,const int32_t length);
void ModemUsartOsInit(void);
void ModemUsartOsTest(void);

#endif /* MODEMCONTROLLER_H_ */