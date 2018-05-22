/*
 * ModemCmdParser.c
 *
 * Created: 5/22/2018 11:31:35 AM
 *  Author: anilj
 */ 
#include "Apps/Tasks/ModemTask/include/ModemCmdParser.h"
#include "Apps/Tasks/ModemTask/include/ModemController.h"
#include "driver_init.h"

#include <hal_usart_os.h>
#include <string.h>

static uint8_t printBuff[40];

/* Data structure for storing command parameters */
static const MODEM_CMD_DATA ModemCmdData[10] = \
{
	/* AT */
	{
		CMD_AT,
		{5,2},
		"AT\r",
		3,
		mdmParser_ParseModemResponse,
		9
	},

	/* AT + CGSN*/
	{
		CMD_AT_CGSN,
		{10,15},
		"AT+CGSN\r",
		8,
		mdmParser_ParseModemResponse,
		31
	}
};

/*============================================================================
**
** Function Name:      sendCommandToModem
**
** Description:        Transmits Data to Modem
**
**===========================================================================*/
void SendATCommandToModem(AT_CMD_TYPE atCmd)
{
	mdmCtrlr_SendDataToModem(ModemCmdData[atCmd].AtString,ModemCmdData[atCmd].CmdLength);
}

/*============================================================================
**
** Function Name:      sendCommandToModem
**
** Description:        Transmits Data to Modem
**
**===========================================================================*/
void mdmParser_ParseModemResponse(AT_CMD_TYPE cmd,uint8_t* resp)
{
	uint8_t Buffer[50];
	uint8_t parseCnt = 0;
	MODEM_CMD_DATA cmdData = ModemCmdData[cmd];
	uint8_t startIndex = cmdData.valid.startIndex;
	
	mdmCtrlr_ReadResponseFromModem(Buffer,cmdData.ResponseLength);
	
	while(startIndex < (cmdData.valid.validDataCount + cmdData.valid.startIndex))
	{
		resp[startIndex - cmdData.valid.startIndex] = Buffer[startIndex];
		startIndex++;
	}
	
	/* Null terminate the string */
	resp[startIndex - cmdData.valid.startIndex] = '\0';
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmParser_GetModemResponse(AT_CMD_TYPE cmd,uint8_t* response)
{
	ModemCmdData[cmd].ParseResponse(cmd,response);
}
