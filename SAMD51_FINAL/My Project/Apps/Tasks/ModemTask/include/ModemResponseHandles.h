/*
 * ModemResponseHandles.h
 *
 * Created: 5/30/2018 11:58:47 PM
 *  Author: anilj
 */ 


#ifndef MODEMRESPONSEHANDLES_H_
#define MODEMRESPONSEHANDLES_H_
#include <stdint.h>
#include <stdbool.h>

void mdmResp_AtRespHandler(uint8_t* response, uint8_t length);
void mdmResp_IMEIRespHandler(uint8_t* response, uint8_t length);
void mdmResp_CarrierRespHandler(uint8_t* response, uint8_t length);
void mdmResp_SerialNoRespHandler(uint8_t* response, uint8_t length);
void mdmResp_KhttpCloseHandler(uint8_t* response, uint8_t length);
void mdmResp_KPatternHandler(uint8_t* response, uint8_t length);
void mdmResp_KcnxCfgHandler(uint8_t* response, uint8_t length);
void mdmResp_KcnxTimerHandler(uint8_t* response, uint8_t length);
void mdmResp_KhttpCfgHandler(uint8_t* response, uint8_t length);
void mdmResp_KhttpHeaderHandler(uint8_t* response, uint8_t length);
void mdmResp_KhttpGetHandler(uint8_t* response, uint8_t length);
void mdmResp_TerminateHeaderHandler(uint8_t* response, uint8_t length);
void defaultFunctionPointer(uint8_t* response, uint8_t length);
void parseSessionId(uint8_t* response);
bool getHeaderResponseOkStatus(void);
#endif /* MODEMRESPONSEHANDLES_H_ */
