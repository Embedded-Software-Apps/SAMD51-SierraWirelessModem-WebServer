/*
 * ModemCmdParser.c
 *
 * Created: 5/22/2018 11:31:35 AM
 *  Author: anilj
 */ 
#include "Apps/Tasks/ModemTask/include/ModemCmdParser.h"
#include "Apps/Tasks/ModemTask/include/ModemController.h"
#include "Apps/Tasks/ModemTask/include/ModemResponseHandles.h"
#include "Apps/Tasks/ModemTask/include/ModemAtCommandSet.h"
#include "Apps/SerialDebug/SerialDebug.h"
#include "Apps/Common/Common.h"
#include "driver_init.h"

#include <hal_usart_os.h>
#include <string.h>

/* Last send Command */
static AT_CMD_TYPE lastSendATCommand = CMD_AT_MAX;
static uint8_t responseDataBuffer[MAX_RESPONSE_SIZE];
static bool isPrevCmdRespProcessed = true;
static void mdmParser_PerformErrorRecovery(void);

/*============================================================================
**
** Function Name:      sendCommandToModem
**
** Description:        Transmits Data to Modem
**
**===========================================================================*/
void mdmParser_SendCommandToModem(AT_CMD_TYPE atCmd)
{
	MODEM_CMD_DATA ModemCmdData;
	getModemCommandData(atCmd, &ModemCmdData);

	if(atCmd == CMD_AT_KHTTP_GET)
	{
		SerialDebugPrint("\r\n",2);
	}
	mdmCtrlr_FlushRxBuffer();
	mdmCtrlr_SendDataToModem(ModemCmdData.AtString,ModemCmdData.CmdLength);
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
	MODEM_CMD_DATA cmdData;
	getModemCommandData(lastSendATCommand, &cmdData);

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

			if (lastSendATCommand == CMD_AT_KHTTP_GET)
			{
				DEBUG_PRINT("No Response from Web Sever....Posting data to sever is failed");
				DEBUG_PRINT("Performing the Error Recovery Procedures..");
				mdmParser_PerformErrorRecovery();
			}
		}

		lastSendATCommand = CMD_AT_MAX;
	}
	else
	{
		DEBUG_PRINT("Error : Process response failed - Last Command Invalid");
	}
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
bool mdmParser_solicitedCmdParser(AT_CMD_TYPE cmd,uint8_t* response)
{
	bool readStatus = false;
	bool parseStatus = false;
	uint8_t dataBuffer[700];
	uint8_t parseCnt=0;

	MODEM_CMD_DATA cmdData;
	getModemCommandData(cmd, &cmdData);

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
			parseStatus = false;
		}
	}
	else
	{
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
static void mdmParser_PerformErrorRecovery(void)
{
	mdmParser_SendCommandToModem(CMD_AT_KHTTP_CLOSE_1);
	delay_ms(1000);
	mdmParser_ProcessModemResponse();

	mdmParser_SendCommandToModem(CMD_AT_KCNX_DOWN);
	delay_ms(1000);
	mdmParser_ProcessModemResponse();

	mdmParser_SendCommandToModem(CMD_AT_CGATT);
	delay_ms(1000);
	mdmParser_ProcessModemResponse();
	delay_ms(3000);

	mdmParam_InitiateConnection();
	DEBUG_PRINT("Closed and Reopened the session......");
	DEBUG_PRINT("Auto recovery completed......");
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

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmParser_SetLastSentAtCommand(AT_CMD_TYPE cmd)
{
	lastSendATCommand = cmd;
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
AT_CMD_TYPE mdmParser_GetLastSentAtCommand(void)
{
	return lastSendATCommand;
}

