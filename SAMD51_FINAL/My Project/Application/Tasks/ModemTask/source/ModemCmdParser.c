/*
 * ModemCmdParser.c
 *
 * Created: 5/22/2018 11:31:35 AM
 *  Author: anilj
 */ 
#include "Application/Tasks/ModemTask/include/ModemCmdParser.h"
#include "Application/Tasks/ModemTask/include/ModemController.h"
#include "Application/Tasks/ModemTask/include/ModemResponseHandles.h"
#include "Application/Tasks/ModemTask/include/ModemAtCommandSet.h"
#include "Application/SerialDebug/SerialDebug.h"
#include "Application/Common/Common.h"
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
	delay_ms(1000);
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
bool mdmParser_solicitedCmdParser(AT_CMD_TYPE cmd)
{
	bool readStatus = false;
	bool parseStatus = false;
	uint8_t* responseBuffer = NULL;
	uint8_t* parsedDataBuffer = NULL;
	uint8_t parseCnt=0;
	MODEM_CMD_DATA cmdData;

	getModemCommandData(cmd, &cmdData);

	/* command length + /r/n */
	uint8_t dataStartIndex = (cmdData.CmdLength + 2);
	
	responseBuffer = (uint8_t*)pvPortMalloc((cmdData.ResponseLength)*(sizeof(uint8_t)));

	if(responseBuffer != NULL)
	{
		readStatus = mdmCtrlr_ReadResponseFromModem(responseBuffer,cmdData.ResponseLength);

		if(readStatus != false)
		{
			if(VERIFIED_EQUAL == strncmp(cmdData.AtString, responseBuffer, cmdData.CmdLength))
			{
				/* Command response is correctly identified. Allocate memory for parsed data */
				parsedDataBuffer = (uint8_t*)pvPortMalloc((((cmdData.validDataCnt)*(sizeof(uint8_t))) + 1));

				if(parsedDataBuffer != NULL)
				{
					/* Extract the data part from modem response */
					while(parseCnt < cmdData.validDataCnt)
					{
						parsedDataBuffer[parseCnt] = responseBuffer[dataStartIndex + parseCnt];
						parseCnt++;
					}
					parsedDataBuffer[parseCnt] = '\0';
					cmdData.respHandler(cmdData.AtCmd,parsedDataBuffer,cmdData.validDataCnt+1);
					vPortFree(parsedDataBuffer);
					parseStatus = true;
				}
				else
				{
					DEBUG_PRINT("Error: Heap allocation for parse data buffer failed");
				}
			}
			else
			{
				parseStatus = false;
				DEBUG_PRINT("Error: Not able to verify the command string during parsing");
			}
		}
		else
		{
			parseStatus = false;
			DEBUG_PRINT("Error: Not able to read data from receive ring buffer during parser");
		}
		vPortFree(responseBuffer);
	}
	else
	{
		DEBUG_PRINT("Error: Heap allocation for response data buffer failed");
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

