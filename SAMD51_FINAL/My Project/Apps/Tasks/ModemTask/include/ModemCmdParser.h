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

typedef enum
{
	CMD_AT,
	CMD_AT_CGSN,
	CMD_AT_WCARRIER
}AT_CMD_TYPE;

typedef struct
{
	uint8_t startIndex;
	uint8_t validDataCount;
}CMD_VALID_DATA;

typedef struct
{
	AT_CMD_TYPE AtCmd;
	CMD_VALID_DATA valid;
	uint8_t* AtString;
	uint8_t CmdLength;
	void (*ParseResponse)(AT_CMD_TYPE,uint8_t* response);
	uint8_t ResponseLength;
	
}MODEM_CMD_DATA;


typedef enum
{
	MODEM_TX,
	MODEM_RX
}MDM_COMMS_TEST;

void SendATCommandToModem(AT_CMD_TYPE atCmd);
void mdmParser_ParseModemResponse(AT_CMD_TYPE cmd,uint8_t *response);
void mdmParser_GetModemResponse(AT_CMD_TYPE cmd,uint8_t *response);



#endif /* MODEMCMDPARSER_H_ */