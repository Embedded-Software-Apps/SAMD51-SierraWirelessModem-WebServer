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
static const MODEM_CMD_DATA ModemCmdData[TOTAL_MODEM_CMDS] = \
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
	},
	
	/* AT+WCARRIER */
	{
		CMD_AT_WCARRIER,
		{11,3},
		"AT+WCARRIER\r",
		12,
		mdmParser_ParseModemResponse,
		20
	},
	
	/* AT+IPR */
	{
		CMD_AT_IPR,
		{6+8,6},
		"AT+IPR?\r",
		8,
		mdmParser_ParseModemResponse,
		29
	},
	
	/* AT+CPIN */
	{
		CMD_AT_CPIN,
		{7,5},
		"AT+CPIN?\r",
		8,
		mdmParser_ParseModemResponse,
		30
	},
	
	/* AT+CGREG */
	{
		CMD_AT_CGREG,
		{10,1},
		"AT+CGREG?\r",
		10,
		mdmParser_ParseModemResponse,
		28
	},
	
	/* AT+CGREG */
	{
		CMD_AT_KGSN,
		{7,14},
		"AT+KGSN=3\r",
		10,
		mdmParser_ParseModemResponse,
		38
	},
	
	/* ATE0 */
	{
		CMD_AT_ATE,
		{0,2},
		"ATE1\r",
		5,
		mdmParser_ParseModemResponse,
		4
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
	bool readStatus = false;
	uint8_t Buffer[50];
	uint8_t parseCnt = 0;
	MODEM_CMD_DATA cmdData = ModemCmdData[cmd];
	uint8_t startIndex = cmdData.valid.startIndex;
	
	readStatus = mdmCtrlr_ReadResponseFromModem(Buffer,cmdData.ResponseLength);
	
	if(readStatus != false)
	{
		while(startIndex < (cmdData.valid.validDataCount + cmdData.valid.startIndex))
		{
			resp[startIndex - cmdData.valid.startIndex] = Buffer[startIndex];
			startIndex++;
		}
		
		/* Null terminate the string */
		resp[startIndex - cmdData.valid.startIndex] = '\0';		
	}
	else
	{
		/* Not able to read the data from Rx Buffer. Parsing Failed. */
		SerialDebugPrint("Problem in Parsing \r\n",20);
	}
	
	mdmCtrlr_FlushRxBuffer();
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmParser_GetModemResponse(AT_CMD_TYPE cmd,uint8_t* response,uint8_t* respLength)
{
	ModemCmdData[cmd].ParseResponse(cmd,response);
	*respLength = ModemCmdData[cmd].valid.validDataCount;
}
