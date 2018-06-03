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

#define TOTAL_MODEM_CMDS (20)
#define CRLF_CHAR_LEN (4)
#define MAX_RESPONSE_SIZE (700)

typedef void (*ResponseHandler)(uint8_t* response,uint8_t length);

typedef enum
{
	CMD_AT = 0,
	CMD_AT_CGSN = 1,
	CMD_AT_WCARRIER = 2,
	CMD_AT_IPR = 3,
	CMD_AT_CPIN = 4,
	CMD_AT_CGREG = 5,
	CMD_AT_KGSN = 6,
	CMD_AT_ATE = 7,

	/* Connection Commands */
	CMD_AT_KHTTP_CFG = 8,
	CMD_AT_KHTTP_CLOSE_1 = 9,
	CMD_AT_KHTTP_CLOSE_2 = 10,
	CMD_AT_KHTTP_CLOSE_3 = 11,
	CMD_AT_KHTTP_CLOSE_4 = 12,
	CMD_AT_KHTTP_CLOSE_5 = 13,
	CMD_AT_KHTTP_CLOSE_6 = 14,
	CMD_AT_KHTTP_CLOSE_7 = 15,
	CMD_AT_KHTTP_CLOSE_8 = 16,
	CMD_AT_KHTTP_CLOSE_9 = 17,
	CMD_AT_KHTTP_CLOSE_10 = 18,
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
void mdmParser_SetLastCmdProcessed(bool status);
bool mdmParser_IsLastCmdProcessed(void);

#endif /* MODEMCMDPARSER_H_ */
