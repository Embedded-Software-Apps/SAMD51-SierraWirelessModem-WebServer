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

void mdmCtrlr_DataCommInit(void);
uint32_t mdmCtrlr_SendDataToModem(const uint8_t *const TxData,const uint16_t length);
void mdmCtrlr_FlushRxBuffer(void);
bool mdmCtrlr_ReadResponseFromModem(uint8_t *const buf, const uint16_t length);
#endif /* MODEMCONTROLLER_H_ */