/*
 * ModemResponseHandles.h
 *
 * Created: 5/30/2018 11:58:47 PM
 *  Author: anilj
 */ 


#ifndef MODEMRESPONSEHANDLES_H_
#define MODEMRESPONSEHANDLES_H_
#include <stdint.h>



void mdmResp_AtRespHandler(uint8_t* response, uint8_t length);
void mdmResp_IMEIRespHandler(uint8_t* response, uint8_t length);
void mdmResp_KhttpCloseHandler(uint8_t* response, uint8_t length);

#endif /* MODEMRESPONSEHANDLES_H_ */
