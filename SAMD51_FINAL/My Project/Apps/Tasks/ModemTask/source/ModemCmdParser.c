/*
 * ModemCmdParser.c
 *
 * Created: 5/22/2018 11:31:35 AM
 *  Author: anilj
 */ 
#include "Apps/Tasks/ModemTask/include/ModemCmdParser.h"
#include "Apps/Tasks/ModemTask/include/ModemController.h"
#include "Apps/Tasks/ModemTask/include/ModemResponseHandles.h"
#include "Apps/SerialDebug/SerialDebug.h"
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
		INT_THREE,
		INT_TWO,
		mdmResp_AtRespHandler,
		(INT_THREE + INT_TWO + CRLF_CHAR_LEN)
	},

	/* AT + CGSN*/
	{
		CMD_AT_CGSN,
		"AT+CGSN\r",
		INT_EIGHT,
		INT_FIFTEEN,
		mdmResp_IMEIRespHandler,
		(INT_EIGHT + INT_FIFTEEN + CRLF_CHAR_LEN)
	},
	
	/* AT+WCARRIER */
	{
		CMD_AT_WCARRIER,
		"AT+WCARRIER\r",
		INT_TWELEVE,
		INT_THREE,
		defaultFunctionPointer,
		(INT_TWELEVE + INT_THREE + CRLF_CHAR_LEN)
	},
	
	/* AT+IPR */
	{
		CMD_AT_IPR,
		"AT+IPR?\r",
		INT_EIGHT,
		INT_TWELEVE,
		defaultFunctionPointer,
		(INT_EIGHT + INT_TWELEVE + CRLF_CHAR_LEN)
	},
	
	/* AT+CPIN */
	{
		CMD_AT_CPIN,
		"AT+CPIN?\r",
		INT_FIVE,
		INT_EIGHT,
		defaultFunctionPointer,
		(INT_FIVE + INT_EIGHT + CRLF_CHAR_LEN)
	},
	
	/* AT+CGREG */
	{
		CMD_AT_CGREG,
		"AT+CGREG?\r",
		INT_TEN,
		INT_ELEVEN,
		defaultFunctionPointer,
		(INT_TEN + INT_ELEVEN + CRLF_CHAR_LEN)
	},
	
	/* AT+KGSN */
	{
		CMD_AT_KGSN,
		"AT+KGSN=3\r",
		INT_TEN,
		INT_TWENTY_ONE,
		defaultFunctionPointer,
		(INT_TEN + INT_TWENTY_ONE + CRLF_CHAR_LEN)
	},
	
	/* ATE0 */
	{
		CMD_AT_ATE,
		"ATE1\r",
		INT_FIVE,
		INT_TWO,
		defaultFunctionPointer,
		(INT_FIVE + INT_TWO + CRLF_CHAR_LEN)
	},

	/* Connection Initialization Commands */
	{
		CMD_AT_KHTTP_CFG,
		"AT+KHTTPCFG=3,\"ingest1.response.cloud\"\r\n",
		INT_FOURTY,
		INT_TWELEVE,
		defaultFunctionPointer,
		(INT_FOURTY + INT_TWELEVE + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_1,
		"AT+KHTTPCLOSE=1,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_2,
		"AT+KHTTPCLOSE=2,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_3,
		"AT+KHTTPCLOSE=3,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_4,
		"AT+KHTTPCLOSE=4,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_5,
		"AT+KHTTPCLOSE=5,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_6,
		"AT+KHTTPCLOSE=6,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_7,
		"AT+KHTTPCLOSE=7,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_8,
		"AT+KHTTPCLOSE=8,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_9,
		"AT+KHTTPCLOSE=9,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_10,
		"AT+KHTTPCLOSE=10,0\r",
		INT_NINETEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_NINETEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	/* Default */
	{
		CMD_AT_MAX,
		"",
		INT_ZERO,
		INT_ZERO,
		defaultFunctionPointer,
		INT_ZERO
	}

};

/* Last send Command */
static AT_CMD_TYPE lastSendATCommand = CMD_AT_MAX;
static uint8_t responseDataBuffer[MAX_RESPONSE_SIZE];
static bool isPrevCmdRespProcessed = true;
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
	mdmParser_SetLastCmdProcessed(false);
	delay_ms(1000);
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
	MODEM_CMD_DATA cmdData = ModemCmdData[lastSendATCommand];

	if(lastSendATCommand != CMD_AT_MAX)
	{
		if(false != mdmParser_solicitedCmdParser(lastSendATCommand,responseDataBuffer))
		{
			if(lastSendATCommand == cmdData.AtCmd)
			{
				cmdData.respHandler(responseDataBuffer,cmdData.validDataCnt);
				mdmParser_SetLastCmdProcessed(true);
			}
		}
		else
		{
			DEBUG_PRINT("Expected modem response is not received");
		}

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

			/* Extract the data part from modem response */
			while(parseCnt < cmdData.validDataCnt)
			{
				response[parseCnt] = dataBuffer[dataStartIndex + parseCnt];
				parseCnt++;
			}
			response[parseCnt] = '\0';
			parseStatus = true;
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

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
static bool mdmParser_CheckForUnSolicitedResponses(void)
{


}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void defaultFunctionPointer(uint8_t* response, uint8_t length)
{

}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmParser_SetLastCmdProcessed(bool status)
{
	isPrevCmdRespProcessed = status;
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
bool mdmParser_IsLastCmdProcessed(void)
{
	return isPrevCmdRespProcessed;
}

