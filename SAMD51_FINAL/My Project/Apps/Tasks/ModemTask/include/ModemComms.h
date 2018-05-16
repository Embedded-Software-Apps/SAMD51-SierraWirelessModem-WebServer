/*
 * ModemComms.h
 *
 * Created: 5/12/2018 7:43:35 PM
 *  Author: anilj
 */ 


#ifndef MODEMCOMMS_H_
#define MODEMCOMMS_H_

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
	
}MODEM_COMM_TYPE;


typedef enum
{
	MODEM_TX,
	MODEM_RX	
}MDM_COMMS_TEST;

void SendATCommandToModem(AT_CMD_TYPE atCmd);
void mdmComms_ParseModemResponse(AT_CMD_TYPE cmd,uint8_t *response);
void mdmComms_GetModemResponse(AT_CMD_TYPE cmd,uint8_t *response);
#endif /* MODEMCOMMS_H_ */