/*
 * ModemCmdParser.h
 *
 * Created: 5/22/2018 11:31:22 AM
 *  Author: anilj
 */ 


#ifndef MODEMCMDPARSER_H_
#define MODEMCMDPARSER_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "Apps/Common/Common.h"
#include "Apps/Tasks/ModemTask/include/ModemAtCommandSet.h"

void mdmParser_SendCommandToModem(AT_CMD_TYPE atCmd);
void mdmParser_ParseModemResponse(AT_CMD_TYPE cmd,uint8_t *response);
void mdmParser_GetModemResponse(AT_CMD_TYPE cmd,uint8_t* response,uint8_t* respLength);
void mdmParser_ProcessModemResponse(void);
void mdmParser_SetLastCmdProcessed(bool status);
bool mdmParser_IsLastCmdProcessed(void);
void mdmParser_SetLastSentAtCommand(AT_CMD_TYPE cmd);
AT_CMD_TYPE mdmParser_GetLastSentAtCommand(void);
bool mdmParser_solicitedCmdParser(AT_CMD_TYPE cmd,uint8_t* response);

#endif /* MODEMCMDPARSER_H_ */
