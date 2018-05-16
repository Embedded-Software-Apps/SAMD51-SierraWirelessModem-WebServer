/*
 * ModemComms.c
 *
 * Created: 5/12/2018 7:43:55 PM
 *  Author: anilj
 */
#include "Apps/Tasks/ModemTask/include/ModemComms.h" 
#include "Apps/Tasks/ModemTask/include/ModemController.h"
#include "driver_init.h"

#include <hal_usart_os.h>
#include <string.h>

static uint8_t printBuff[40];

static const MODEM_COMM_TYPE ModemComms[10] = \
{
	/* AT */
	{
		CMD_AT,
		{5,2},
		"AT\r",
		3,
		mdmComms_ParseModemResponse,
		9	
	},

	/* AT + CGSN*/
	{
		CMD_AT_CGSN,
		{10,15},
		"AT+CGSN\r",
		8,
		mdmComms_ParseModemResponse,
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
	mdmCtrlr_SendDataToModem(ModemComms[atCmd].AtString,ModemComms[atCmd].CmdLength);
}

/*============================================================================
**
** Function Name:      sendCommandToModem
**
** Description:        Transmits Data to Modem
**
**===========================================================================*/
void mdmComms_ParseModemResponse(AT_CMD_TYPE cmd,uint8_t* resp)
{
	uint8_t Buffer[50];
	uint8_t parseCnt = 0;
	MODEM_COMM_TYPE cmdData = ModemComms[cmd];
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
** Function Name:      sendCommandToModem
**
** Description:        Transmits Data to Modem
**
**===========================================================================*/
void mdmComms_GetModemResponse(AT_CMD_TYPE cmd,uint8_t* response)
{	
	ModemComms[cmd].ParseResponse(cmd,response);
}