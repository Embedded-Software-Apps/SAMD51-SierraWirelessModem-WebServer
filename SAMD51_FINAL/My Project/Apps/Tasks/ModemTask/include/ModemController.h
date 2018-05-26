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

void mdmCtrlr_DataCommInit(void);
uint32_t mdmCtrlr_SendDataToModem(const uint8_t *const TxData,const uint16_t length);
void mdmCtrlr_FlushRxBuffer(void);
bool mdmCtrlr_ReadResponseFromModem(uint8_t *const buf, const uint16_t length);
#endif /* MODEMCONTROLLER_H_ */