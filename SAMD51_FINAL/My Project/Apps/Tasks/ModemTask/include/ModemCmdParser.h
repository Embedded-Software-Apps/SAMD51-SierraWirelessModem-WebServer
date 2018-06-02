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
#include "Apps/Common/Common.h"

#define TOTAL_MODEM_CMDS (20)
#define CRLF_CHAR_LEN (4)
#define MAX_RESPONSE_SIZE (700)

typedef void (*ResponseHandler)(uint8_t* response,uint8_t length);

typedef enum
{
	CMD_AT,
	CMD_AT_CGSN,
	CMD_AT_WCARRIER,
	CMD_AT_IPR,
	CMD_AT_CPIN,
	CMD_AT_CGREG,
	CMD_AT_KGSN,
	CMD_AT_ATE,

	/* Connection Commands */
	CMD_AT_KHTTP_CFG,
	CMD_AT_MAX
}AT_CMD_TYPE;


typedef struct
{
	AT_CMD_TYPE AtCmd;
	uint8_t* AtString;
	INTEGER_T CmdLength;
	INTEGER_T validDataCnt;
	ResponseHandler respHandler;
	INTEGER_T ResponseLength;
	
}MODEM_CMD_DATA;

void mdmParser_SendCommandToModem(AT_CMD_TYPE atCmd);
void mdmParser_ParseModemResponse(AT_CMD_TYPE cmd,uint8_t *response);
void mdmParser_GetModemResponse(AT_CMD_TYPE cmd,uint8_t* response,uint8_t* respLength);
void defaultFunctionPointer(uint8_t* response, uint8_t length);
void mdmParser_ProcessModemResponse(void);

#endif /* MODEMCMDPARSER_H_ */
