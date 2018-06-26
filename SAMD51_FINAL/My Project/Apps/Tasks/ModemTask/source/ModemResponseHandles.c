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

static void SendEOFPattern(void);

/*============================================================================
**
** Function Name:      modemResponseHandler
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void modemResponseHandler(AT_CMD_TYPE cmd,uint8_t* response, uint8_t length)
{
    MODEM_CMD_DATA cmdData;
    CmdResponseType cmdResponse;
    BaseType_t QueuePushStatus;

    const TickType_t QueuePushDelayMs = pdMS_TO_TICKS(500UL);
    getModemCommandData(cmd,&cmdData);

    if(response != NULL)
    {
        switch (cmdData.cmdSet)
        {
            case AT_CMD_SET_DIAGNOSTICS:
            {
                cmdResponse.atCmd = cmdData.AtCmd;
                cmdResponse.length = length;
                cmdResponse.response = (uint8_t*)pvPortMalloc((length)*(sizeof(uint8_t)));

                if ((cmdResponse.response != NULL) &&
                    (uxQueueMessagesWaiting(DiagResponseQueue) == 0))
                {
                    memcpy(cmdResponse.response,response,length);

                    QueuePushStatus = xQueueSendToBack(DiagResponseQueue, &cmdResponse, QueuePushDelayMs);

                    if(QueuePushStatus == pdPASS)
                    {
                        DEBUG_PRINT("Sent the Diagnostics Response to Diagnostic Task");
                    }
                }
                else
                {
                	DEBUG_PRINT("ERROR: Problem during packing the data in response handler");
                }
            }
            break;

            case AT_CMD_SET_CONNECTION:
            {

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
void mdmResp_AtRespHandler(uint8_t* response, uint8_t length)
{
	DEBUG_PRINT("In AT handler\r\n");
    SerialDebugPrint(response,length);
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
void mdmResp_IMEIRespHandler(uint8_t* response, uint8_t length)
{
    DEBUG_PRINT("In CGSN handler");
    SerialDebugPrint(response,length);
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_CarrierRespHandler(uint8_t* response, uint8_t length)
{
    DEBUG_PRINT("In Carrier handler");
    SerialDebugPrint(response,length);
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_SerialNoRespHandler(uint8_t* response, uint8_t length)
{
    DEBUG_PRINT("In KGSN handler");
    SerialDebugPrint(response,length);
}
/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KhttpCloseHandler(uint8_t* response, uint8_t length)
{
    //DEBUG_PRINT("In KHTTP CLOSE handler");

    if(response[0] == 'O')
    {
        SerialDebugPrint(response,length);
        DEBUG_PRINT("\r\nClosed an active connection");
        mdmParser_SendCommandToModem(CMD_AT_KCNX_DOWN);
        delay_ms(1000);
        mdmParser_ProcessModemResponse();
        delay_ms(500);
        mdmParser_SendCommandToModem(CMD_AT_CGATT);
        delay_ms(1000);
        mdmParser_ProcessModemResponse();
        delay_ms(500);
        mdmCtrlr_FlushRxBuffer();
    }
    else
    {
        DEBUG_PRINT("There is no connection established with this session ID...");
        DEBUG_PRINT("Checking for an active connection with next session ID...\n");
    }

    DEBUG_PRINT("\r\n");
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KPatternHandler(uint8_t* response, uint8_t length)
{
    DEBUG_PRINT("In KPATTERN handler");
    SerialDebugPrint(response,length);
    DEBUG_PRINT("\r\n");
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KcnxCfgHandler(uint8_t* response, uint8_t length)
{
    DEBUG_PRINT("In KCNXCFG handler");
    SerialDebugPrint(response,length);
    DEBUG_PRINT("\r\n");
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KcnxTimerHandler(uint8_t* response, uint8_t length)
{
    DEBUG_PRINT("In KCNXTIMER handler");
    SerialDebugPrint(response,length);
    DEBUG_PRINT("\r\n");
}


/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KhttpCfgHandler(uint8_t* response, uint8_t length)
{
    DEBUG_PRINT("In KHTTPCFG handler");
    SerialDebugPrint(response,length);
    DEBUG_PRINT("\r\n");

    sessionID = response[11];
    mdmParser_SetKhttpHeaderString(&sessionID);
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KhttpHeaderHandler(uint8_t* response, uint8_t length)
{
    DEBUG_PRINT("In KHTTP HEADER handler");
    SerialDebugPrint(response,length);
    DEBUG_PRINT("\r\n");

    if(0==memcmp(response,"CONNECT",7))
    {
        headerResponseOk = true;
        DEBUG_PRINT("Header Response Ok");
        mdmCtrlr_SendDataToModem("--EOF--Pattern--",16);
        
        //SendEOFPattern();
        delay_ms(1000);
        mdmCtrlr_FlushRxBuffer();
        dataPacketSentOk = true;
    }
    else
    {
        headerResponseOk = false;
        dataPacketSentOk = false;
        DEBUG_PRINT("Header Response Not Ok");
    }
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KhttpGetHandler(uint8_t* response, uint8_t length)
{
    DEBUG_PRINT("\r\n\n");
    //SerialDebugPrint(response,length);
    dataPacketSentOk = true;
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_TerminateHeaderHandler(uint8_t* response, uint8_t length)
{
    DEBUG_PRINT("In TERMINATE HEADER handler");
    SerialDebugPrint(response,length);
    DEBUG_PRINT("\r\n");
    dataPacketSentOk = true;
}


/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
uint8_t getSessionId(void)
{
    return sessionID;
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
bool getHeaderResponseOkStatus(void)
{
    return headerResponseOk;
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
bool getDataPacketOkStatus(void)
{
    return dataPacketSentOk;
}


/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void setDataPacketOkStatus(bool status)
{
    dataPacketSentOk = status;
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
static void SendEOFPattern(void)
{
    mdmParser_SendCommandToModem(CMD_AT_TERMINATE_HEADER);
    delay_ms(2000);
    mdmParser_ProcessModemResponse();
    delay_ms(2000);
}
