/*
 * ModemCmdParser.c
 *
 * Created: 5/22/2018 11:31:35 AM
 *  Author: anilj
 */ 
#include "Apps/Tasks/ModemTask/include/ModemCmdParser.h"
#include "Apps/Tasks/ModemTask/include/ModemController.h"
#include "Apps/Common/Common.h"
#include "driver_init.h"

#include <hal_usart_os.h>
#include <string.h>

/* Data structure for storing command parameters */
static const MODEM_CMD_DATA ModemCmdData[TOTAL_MODEM_CMDS] = \
{
	/* AT */
	{
		CMD_AT,
		"AT\r",
		THREE,
		TWO,
		defaultFunctionPointer,
		(THREE + TWO + CRLF_CHAR_LEN)
	},

	/* AT + CGSN*/
	{
		CMD_AT_CGSN,
		"AT+CGSN\r",
		EIGHT,
		FIFTEEN,
		defaultFunctionPointer,
		(EIGHT + FIFTEEN + CRLF_CHAR_LEN)
	},
	
	/* AT+WCARRIER */
	{
		CMD_AT_WCARRIER,
		"AT+WCARRIER\r",
		TWELEVE,
		THREE,
		defaultFunctionPointer,
		(TWELEVE + THREE + CRLF_CHAR_LEN)
	},
	
	/* AT+IPR */
	{
		CMD_AT_IPR,
		"AT+IPR?\r",
		EIGHT,
		TWELEVE,
		defaultFunctionPointer,
		(EIGHT + TWELEVE + CRLF_CHAR_LEN)
	},
	
	/* AT+CPIN */
	{
		CMD_AT_CPIN,
		"AT+CPIN?\r",
		FIVE,
		EIGHT,
		defaultFunctionPointer,
		(FIVE + EIGHT + CRLF_CHAR_LEN)
	},
	
	/* AT+CGREG */
	{
		CMD_AT_CGREG,
		"AT+CGREG?\r",
		TEN,
		ELEVEN,
		defaultFunctionPointer,
		(TEN + ELEVEN + CRLF_CHAR_LEN)
	},
	
	/* AT+KGSN */
	{
		CMD_AT_KGSN,
		"AT+KGSN=3\r",
		TEN,
		TWENTY_ONE,
		defaultFunctionPointer,
		(TEN + TWENTY_ONE + CRLF_CHAR_LEN)
	},
	
	/* ATE0 */
	{
		CMD_AT_ATE,
		"ATE1\r",
		FIVE,
		TWO,
		defaultFunctionPointer,
		(FIVE + TWO + CRLF_CHAR_LEN)
	},

	/* Default */
	{
		CMD_AT_MAX,
		"",
		ZERO,
		ZERO,
		defaultFunctionPointer,
		ZERO
	}

};

/* Last send Command */
static AT_CMD_TYPE lastSendATCommand = CMD_AT_MAX;
static uint8_t responseDataBuffer[40];

static bool mdmParser_solicitedCmdParser(AT_CMD_TYPE cmd,uint8_t* response);
/*============================================================================
**
** Function Name:      sendCommandToModem
**
** Description:        Transmits Data to Modem
**
**===========================================================================*/
void mdmParser_SendCommandToModem(AT_CMD_TYPE atCmd)
{
	mdmCtrlr_FlushRxBuffer();
	mdmCtrlr_SendDataToModem(ModemCmdData[atCmd].AtString,ModemCmdData[atCmd].CmdLength);
	lastSendATCommand = atCmd;
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
	//ModemCmdData[cmd].ParseResponse(cmd,response);
	*respLength = ModemCmdData[cmd].validDataCnt;
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmParser_ProcessModemResponse(void)
{
	if(false != mdmParser_solicitedCmdParser(lastSendATCommand,responseDataBuffer))
	{
		//SerialDebugPrint("Successfully Received modem response data\r\n",45);
		SerialDebugPrint(responseDataBuffer,ModemCmdData[lastSendATCommand].validDataCnt);
		SerialDebugPrint("\r\n",2);
		lastSendATCommand = CMD_AT_MAX;
	}
	else
	{
		SerialDebugPrint("Failed to Receive modem response data\r\n",40);
		lastSendATCommand = CMD_AT_MAX;
	}
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
static bool mdmParser_solicitedCmdParser(AT_CMD_TYPE cmd,uint8_t* response)
{
	bool readStatus = false;
	bool parseStatus = false;
	uint8_t dataBuffer[60];
	uint8_t parseCnt=0;
	MODEM_CMD_DATA cmdData = ModemCmdData[cmd];

	/* command length + /r/n */
	uint8_t dataStartIndex = (cmdData.CmdLength + 2);

	readStatus = mdmCtrlr_ReadResponseFromModem(dataBuffer,cmdData.ResponseLength);

	if(readStatus != false)
	{
		//SerialDebugPrint(dataBuffer,cmdData.ResponseLength);
		if(VERIFIED_EQUAL == strncmp(cmdData.AtString, dataBuffer, cmdData.CmdLength))
		{
			/* Command response is correctly identified */
			//SerialDebugPrint("Successfully parsed the command string\r\n",40);

			/* Extract the data part from modem response */
			while(parseCnt < cmdData.validDataCnt)
			{
				response[parseCnt] = dataBuffer[dataStartIndex + parseCnt];
				parseCnt++;
			}
			response[parseCnt] = '\0';
			parseStatus = true;
			//SerialDebugPrint("Successfully updated the cmd response data to buffer\r\n",50);
		}
		else
		{
			SerialDebugPrint("Failed to verify the command string\r\n",40);
			parseStatus = false;
		}
	}
	else
	{
		SerialDebugPrint("Read from modem controller is failed\r\n",40);
		parseStatus = false;
	}

	mdmCtrlr_FlushRxBuffer();

	return parseStatus;
}





































void defaultFunctionPointer(void)
{

}
