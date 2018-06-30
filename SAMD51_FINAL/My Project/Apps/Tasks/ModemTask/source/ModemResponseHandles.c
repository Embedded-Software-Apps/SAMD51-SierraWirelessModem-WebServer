/*
 * ModemResponseHandles.c
 *
 * Created: 5/30/2018 11:58:30 PM
 *  Author: anilj
 */ 
#include "Apps/Tasks/ModemTask/include/ModemResponseHandles.h"
#include "Apps/Tasks/ModemTask/include/ModemAtCommandSet.h"
#include "Apps/SerialDebug/SerialDebug.h"
#include "Apps/Tasks/ModemTask/include/ModemCmdParser.h"
#include "Apps/Tasks/ModemTask/include/ModemDiagTask.h"
#include "Apps/Common/Common.h"

static uint8_t sessionID = 0;
bool headerResponseOk = false;
bool dataPacketSentOk = false;
static MODEM_CMD_DATA cmdData;
static CmdResponseType cmdResponse;

static void SendEOFPattern(void);
static uint8_t responseBuffer[700];
/*============================================================================
**
** Function Name:      modemResponseHandler
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void modemResponseHandler(AT_CMD_TYPE cmd,uint8_t* response, uint16_t length)
{
	BaseType_t CmdResponseQueuePushStatus;
	const TickType_t QueuePushDelayMs = pdMS_TO_TICKS(100UL);
    getModemCommandData(cmd,&cmdData);

    if(response != NULL)
    {
        switch (cmdData.cmdSet)
        {
            case AT_CMD_SET_DIAGNOSTICS:
            {
            	cmdResponse.atCmd = cmd;
            	cmdResponse.length = length;
            	memset(responseBuffer,0,700);
            	memcpy(responseBuffer,response,length);
            	ModemDiagUpdateDataBase(responseBuffer,&cmdResponse);
				cmd = CMD_AT_MAX;
            }
            break;

            case AT_CMD_SET_CONNECTION:
            {
                if (uxQueueMessagesWaiting(CmdResponseQueue) == 0)
                {
                	cmdResponse.atCmd = cmd;
                	cmdResponse.length = length;
                	cmdResponse.response = (uint8_t*)pvPortMalloc((length)*(sizeof(uint8_t)));

                	if(cmdResponse.response != NULL)
                	{
                		memcpy(cmdResponse.response,response,length);
                		CmdResponseQueuePushStatus = xQueueSendToBack(CmdResponseQueue, &cmdResponse, QueuePushDelayMs);

                		if(CmdResponseQueuePushStatus == pdPASS)
                		{
                			//DEBUG_PRINT("Successfully posted connection Response to Queue");
                		}
                		else
                		{
                			DEBUG_PRINT("Failed to post the connection Response to Queue");
                		}
                	}
                }
                else
                {
                	DEBUG_PRINT("Error : Command Response Queue is not empty");
                }
            }
            break;

            case AT_CMD_SET_MAX:
            {

            }
            break;

            default:
                break;
        }
    }
    else
    {
    	DEBUG_PRINT("ERROR: Received an empty response string in command response handler");
    }
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

