/*
 * ModemParameters.h
 *
 * Created: 5/22/2018 7:39:06 PM
 *  Author: anilj
 */ 


#ifndef MODEMPARAMETERS_H_
#define MODEMPARAMETERS_H_

#include <stdbool.h>

typedef enum
{
	TEST_UART_COMM,
	TEST_UART_BAUDARTE,
	TEST_SIM_PIN_STATUS,
	TEST_NETWORK_REG_STATUS,
	TEST_NETWORK_CARRIER,
	TEST_DEVICE_SERIAL_NO
			
}TEST_MODEM_BASE_PARAMS;

/* HTTP initiation commands */
typedef enum
{
	AT_KPATTERN,
	AT_KCNXCFG,
	AT_KCNXTIMER,
	AT_KHTTPCFG,
	AT_KHTTPHEADER,
	AT_KHTTPGET
}CONNECT_COMMANDS;

typedef struct 
{
	CONNECT_COMMANDS CONN_CMD;
	uint8_t* CmdString;
	uint8_t CmdLen;
	uint8_t respLen;
}CONNECT_DATA;

bool MdmParam_CustomizeGeneralSettings(void);
bool MdmParam_TestModemBaseParameters(void);

void mdmParam_InitiateConnection(void);
#endif /* MODEMPARAMETERS_H_ */
