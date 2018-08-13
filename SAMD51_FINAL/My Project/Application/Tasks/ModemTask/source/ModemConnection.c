/*
 * ModemConnection.c
 *
 * Created: 5/22/2018 12:51:38 PM
 *  Author: anilj
 */ 
#include "Application/Tasks/ModemTask/include/ModemConnection.h"
#include "Application/Tasks/ModemTask/include/ModemCmdParser.h"
#include "Application/Tasks/ModemTask/include/ModemAtCommandSet.h"
#include "Application/Tasks/ModemTask/include/ModemConnectionConfig.h"
#include "Application/Common/Common.h"

/******************************************************************************************
************************************STATIC VARIABLES***************************************
*******************************************************************************************/
static HTTP_CONNECT_STATES_T gHttpConnectionState;
static HTTP_CONNECT_IN_PROGRESS_SUBSTATES_T gHttpConnectionInProgressSubstate;
static HTTP_CONNECT_OPERATIONAL_STATE_T gHttpConnectOpMode;
static HTTP_CONNECTED_SUBSTATES_T gHttpConnectedSubState;
static CONNECTION_ERROR_RECOVERY_STATE_T gErrorRecoveryState;
static uint8_t sessionIdCount = 0;
static CmdResponseType ConnectionResponse;
static uint8_t currentSessionId;
static uint8_t connectionStatus;

static void MdmCnct_CloseActiveConnections(void);
static AT_CMD_TYPE getCloseActiveSessionCmd(uint8_t sessionID);
static void performForcedRebootOfModem(void);
static void MdmCnct_ExtractSessionIdFromConfigResponse(uint8_t* response);
static bool MdmCnct_VerifyConnectionStatusFromConfigResponse(uint8_t* cfgResponse);
static bool processHttpHeaderResponse(uint8_t* response);
static bool validateCommonCommandResponse(uint8_t* response);
static void MdmCnct_ConnectedSubStateMachine(void);
static bool MdmCnct_validateServerResponse(uint8_t* response);
static bool MdmCnct_PeformErrorRecovery(void);
/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
void MdmConnect_HttpConnectionInit(void)
{
    gHttpConnectionState = MDM_HTTP_DISCONNECTED;
    gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_CLOSE_CONNECTION;
    gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
    gHttpConnectedSubState = CONNECTED_INITIALIZE_TRANSMISSION;
    gErrorRecoveryState = CLOSE_ALL_EXISTING_CONNECIONS;
    sessionIdCount = 5;

    ConnectionResponse.atCmd = CMD_AT_MAX;
    ConnectionResponse.length = 0;
    ConnectionResponse.response = NULL;

    currentSessionId = '0';
}
/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
void MdmConnect_HttpConnectionSchedule(void)
{
    switch (gHttpConnectionState)
    {
        case MDM_HTTP_DISCONNECTED:
        {
            if(isModemDiagDataBaseUpdated() != false)
            {
                gHttpConnectionState = MDM_HTTP_CONNECTION_IN_PROGRESS;
                gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_CLOSE_CONNECTION;
                gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                DEBUG_PRINT("Closing Active Connections");
            }
        }
        break;

        case MDM_HTTP_CONNECTION_IN_PROGRESS:
        {
            MdmCnct_ConnectInProgressSubStateMachine();
        }
        break;

        case MDM_HTTP_CONNECTED:
        {
            MdmCnct_ConnectedSubStateMachine();
        }
        break;

        case MDM_HTTP_DISCONNECTION_IN_PROGRESS:
        {

        }
        break;

        case MDM_HTTP_CONNECTION_FAULT:
        {

        }
        break;

        default:
        break;
    }
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
void MdmCnct_ConnectInProgressSubStateMachine(void)
{
    AtTxMsgType TxMsgQueueData;
    BaseType_t TxQueuePushStatus;
    const TickType_t QueuePushDelayMs = pdMS_TO_TICKS(500UL);
    const TickType_t TransmitDelayMs = pdMS_TO_TICKS(500UL);
    const TickType_t ResponseWaitDelayMs = pdMS_TO_TICKS(4000UL);
    const TickType_t PacketTransmitDelayMs = pdMS_TO_TICKS(100UL);

    switch (gHttpConnectionInProgressSubstate)
    {
        case CONNECT_IN_PROGRESS_CLOSE_CONNECTION:
        {
            if(gHttpConnectOpMode == HTTP_CONNECT_OP_TX_MODE)
            {
                if(sessionIdCount > 0)
                {
                    if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                    {
                        if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                        {
                            TxMsgQueueData.taskID = MODEM_PROCESS_TASK;
                            TxMsgQueueData.atCmd = getCloseActiveSessionCmd(sessionIdCount);
                            TxMsgQueueData.pData = NULL;
                            TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                            if(TxQueuePushStatus == pdPASS)
                            {
                                xSemaphoreGive(AtTxQueueLoadSemaphore);
                                gHttpConnectOpMode = HTTP_CONNECT_OP_RX_MODE;
                                vTaskDelay(TransmitDelayMs);
                            }
                            else
                            {
                                DEBUG_PRINT("Failed to sent the Session Close request to Tx Task");
                                vTaskDelay(TransmitDelayMs);
                            }
                        }
                        else
                        {
                            DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Transmit Queue is not empty");
                    }
                }
                else
                {
                    DEBUG_PRINT("No More Active Connections to close");
                    DEBUG_PRINT("\r\n");
                    gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_PDP_CONNECTION_DOWN;
                    gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                }

            }
            else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
            {
                if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
                {
                    if(ConnectionResponse.atCmd == getCloseActiveSessionCmd(sessionIdCount))
                    {
                        if(sessionIdCount > 0)
                        {
                            sessionIdCount--;
                            //SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
                            //DEBUG_PRINT("\r\n");
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        }
                        else
                        {
                            gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_PDP_CONNECTION_DOWN;
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                            DEBUG_PRINT("Closed All Active Connections");
                        }
                        vPortFree(ConnectionResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to receive connection response in RX mode");
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        vPortFree(ConnectionResponse.response);
                    }
                }
                else
                {
                    /* Wait untill there is a response received from Rx Task */
                }
            }
            else
            {
                /* This part will never execute */
            }
        }
        break;

        case CONNECT_IN_PROGRESS_PDP_CONNECTION_DOWN:
        {
            if(gHttpConnectOpMode == HTTP_CONNECT_OP_TX_MODE)
            {
                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = MODEM_PROCESS_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_KCNX_DOWN;
                        TxMsgQueueData.pData = NULL;
                        TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                        if(TxQueuePushStatus == pdPASS)
                        {
                            xSemaphoreGive(AtTxQueueLoadSemaphore);
                            gHttpConnectOpMode = HTTP_CONNECT_OP_RX_MODE;
                            gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_PDP_CONNECTION_DOWN;
                            vTaskDelay(TransmitDelayMs);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to sent the Session Close request to Tx Task");
                            vTaskDelay(TransmitDelayMs);
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                    }
                }
                else
                {
                    DEBUG_PRINT("Transmit Queue is not empty");
                }
            }
            else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
            {
                if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
                {
                    if(ConnectionResponse.atCmd == CMD_AT_KCNX_DOWN)
                    {
                        if(false != validateCommonCommandResponse(ConnectionResponse.response))
                        {
                            DEBUG_PRINT("Brought the PDP connection DOWN");
                            SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
                            DEBUG_PRINT("\r\n");
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                            gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_PS_CONNECTION_DETACH;
                        }
                        else
                        {
                            DEBUG_PRINT("Expected Response Not Received...");
                            DEBUG_PRINT("\r\n");
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                            performForcedRebootOfModem();
                        }
                        vPortFree(ConnectionResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to receive connection response in RX mode");
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        vPortFree(ConnectionResponse.response);
                    }
                }
                else
                {
                    /* Wait untill there is a response received from Rx Task */                 
                }
            }
            else
            {
                /* This part will never execute */
            }
        }
        break;

        case CONNECT_IN_PROGRESS_PS_CONNECTION_DETACH:
        {
            if(gHttpConnectOpMode == HTTP_CONNECT_OP_TX_MODE)
            {
                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = MODEM_PROCESS_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_CGATT;
                        TxMsgQueueData.pData = NULL;
                        TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                        if(TxQueuePushStatus == pdPASS)
                        {
                            xSemaphoreGive(AtTxQueueLoadSemaphore);
                            gHttpConnectOpMode = HTTP_CONNECT_OP_RX_MODE;
                            gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_PS_CONNECTION_DETACH;
                            vTaskDelay(TransmitDelayMs);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to sent the PS Detach request to Tx Task");
                            vTaskDelay(TransmitDelayMs);
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                    }
                }
                else
                {
                    DEBUG_PRINT("Transmit Queue is not empty");
                }
            }
            else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
            {
                if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
                {
                    if(ConnectionResponse.atCmd == CMD_AT_CGATT)
                    {
                        if(false != validateCommonCommandResponse(ConnectionResponse.response))
                        {
                            DEBUG_PRINT("Detached the PDP PS");
                            SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
                            DEBUG_PRINT("\r\n");

                            DEBUG_PRINT("Deleted all of the previous connection configurations");
                            DEBUG_PRINT("\r\n");

                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                            gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_SET_EOF_PATTERN;
                        }
                        else
                        {
                            DEBUG_PRINT("Expected Response Not Received...");
                            DEBUG_PRINT("\r\n");
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                            performForcedRebootOfModem();
                        }
                        vPortFree(ConnectionResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to receive connection response in RX mode");
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        vPortFree(ConnectionResponse.response);
                    }
                }
                else
                {
                    /* Wait untill there is a response received from Rx Task */ 
                }
            }
            else
            {

            }
        }
        break;

        case CONNECT_IN_PROGRESS_SET_EOF_PATTERN:
        {
            if(gHttpConnectOpMode == HTTP_CONNECT_OP_TX_MODE)
            {
                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = MODEM_PROCESS_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_KPATTERN;
                        TxMsgQueueData.pData = NULL;
                        TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                        if(TxQueuePushStatus == pdPASS)
                        {
                            xSemaphoreGive(AtTxQueueLoadSemaphore);
                            gHttpConnectOpMode = HTTP_CONNECT_OP_RX_MODE;
                            vTaskDelay(TransmitDelayMs);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to sent the EOF pattern to Tx Task");
                            vTaskDelay(TransmitDelayMs);
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                    }
                }
                else
                {
                    DEBUG_PRINT("Transmit Queue is not empty");
                }
            }
            else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
            {
                if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
                {
                    if(ConnectionResponse.atCmd == CMD_AT_KPATTERN)
                    {
                        DEBUG_PRINT("EOF Pattern configured");
                        SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
                        DEBUG_PRINT("\r\n");
                        gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_RETRIEVE_APN;
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;

                        vPortFree(ConnectionResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to receive connection response in RX mode");
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        vPortFree(ConnectionResponse.response);
                    }
                }
                else
                {
                    /* Wait untill there is a response received from Rx Task */ 
                }
            }
            else
            {

            }
        }
        break;

        case CONNECT_IN_PROGRESS_RETRIEVE_APN:
        {
            if(gHttpConnectOpMode == HTTP_CONNECT_OP_TX_MODE)
            {
                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = MODEM_PROCESS_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_CGDCONT;
                        TxMsgQueueData.pData = NULL;
                        TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                        if(TxQueuePushStatus == pdPASS)
                        {
                            xSemaphoreGive(AtTxQueueLoadSemaphore);
                            gHttpConnectOpMode = HTTP_CONNECT_OP_RX_MODE;
                            vTaskDelay(TransmitDelayMs);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to sent the retrieve APN command to Tx Task");
                            vTaskDelay(TransmitDelayMs);
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                    }
                }
                else
                {
                    DEBUG_PRINT("Transmit Queue is not empty");
                }
            }
            else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
            {
                if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
                {
                    if(ConnectionResponse.atCmd == CMD_AT_CGDCONT)
                    {
                        DEBUG_PRINT("Retrieved APN string.");
                        SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
                        DEBUG_PRINT("\r\n");
                        gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_SET_ACCESS_POINT;
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;

                        vPortFree(ConnectionResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to receive connection response in RX mode");
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        vPortFree(ConnectionResponse.response);
                    }
                }
                else
                {
                    /* Wait untill there is a response received from Rx Task */
                }
            }
            else
            {

            }
        }
        break;

        case CONNECT_IN_PROGRESS_SET_ACCESS_POINT:
        {
            if(gHttpConnectOpMode == HTTP_CONNECT_OP_TX_MODE)
            {
                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = MODEM_PROCESS_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_KCNXCFG;
                        TxMsgQueueData.pData = NULL;
                        TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                        if(TxQueuePushStatus == pdPASS)
                        {
                            xSemaphoreGive(AtTxQueueLoadSemaphore);
                            gHttpConnectOpMode = HTTP_CONNECT_OP_RX_MODE;
                            vTaskDelay(TransmitDelayMs);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to sent access point cmd to Tx Task");
                            vTaskDelay(TransmitDelayMs);
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                    }
                }
                else
                {
                    DEBUG_PRINT("Transmit Queue is not empty");
                }
            }
            else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
            {
                if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
                {
                    if(ConnectionResponse.atCmd == CMD_AT_KCNXCFG)
                    {
                        DEBUG_PRINT("Access Point configured");
                        SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
                        DEBUG_PRINT("\r\n");
                        gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_SET_CONNECT_TIMERS;
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;

                        vPortFree(ConnectionResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to receive connection response in RX mode");
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        vPortFree(ConnectionResponse.response);
                    }
                }
                else
                {
                    /* Wait untill there is a response received from Rx Task */ 
                }
            }
            else
            {

            }
        }
        break;

        case CONNECT_IN_PROGRESS_SET_CONNECT_TIMERS:
        {
            if(gHttpConnectOpMode == HTTP_CONNECT_OP_TX_MODE)
            {
                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = MODEM_PROCESS_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_KCNXTIMER;
                        TxMsgQueueData.pData = NULL;
                        TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                        if(TxQueuePushStatus == pdPASS)
                        {
                            xSemaphoreGive(AtTxQueueLoadSemaphore);
                            gHttpConnectOpMode = HTTP_CONNECT_OP_RX_MODE;
                            vTaskDelay(TransmitDelayMs);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to sent connection timer cmd to Tx Task");
                            vTaskDelay(TransmitDelayMs);
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                    }
                }
                else
                {
                    DEBUG_PRINT("Transmit Queue is not empty");
                }
            }
            else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
            {
                if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
                {
                    if(ConnectionResponse.atCmd == CMD_AT_KCNXTIMER)
                    {
                        DEBUG_PRINT("Connection Timers configured");
                        SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
                        DEBUG_PRINT("\r\n");
                        gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_SET_SERVER_ADDRESS;
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;

                        vPortFree(ConnectionResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to receive connection response in RX mode");
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        vPortFree(ConnectionResponse.response);
                    }
                }
                else
                {
                    /* Wait untill there is a response received from Rx Task */ 
                }
            }
            else
            {

            }
        }
        break;

        case CONNECT_IN_PROGRESS_SET_SERVER_ADDRESS:
        {
            if(gHttpConnectOpMode == HTTP_CONNECT_OP_TX_MODE)
            {
                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = MODEM_PROCESS_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_KHTTP_CFG;
                        TxMsgQueueData.pData = NULL;
                        TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                        if(TxQueuePushStatus == pdPASS)
                        {
                            xSemaphoreGive(AtTxQueueLoadSemaphore);
                            gHttpConnectOpMode = HTTP_CONNECT_OP_RX_MODE;
                            vTaskDelay(TransmitDelayMs);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to sent connection timer cmd to Tx Task");
                            vTaskDelay(TransmitDelayMs);
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                    }
                }
                else
                {
                    DEBUG_PRINT("Transmit Queue is not empty");
                }
            }
            else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
            {
                if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
                {
                    if(ConnectionResponse.atCmd == CMD_AT_KHTTP_CFG)
                    {
                        MdmCnct_ExtractSessionIdFromConfigResponse(ConnectionResponse.response);
                        SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
                        DEBUG_PRINT("\r\n");

                        if(false != MdmCnct_VerifyConnectionStatusFromConfigResponse(ConnectionResponse.response))
                        {
                            DEBUG_PRINT("Connection successful...Cloud Server configured");
                            DEBUG_PRINT("\r\n");
                            gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_SET_HTTP_HEADER;
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        }
                        else
                        {
                            DEBUG_PRINT("Error : Connection Failure");
                            DEBUG_PRINT("Restarting the connection initialization...");
                            DEBUG_PRINT("\r\n");
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                            gHttpConnectedSubState = CONNECTED_FAULT_IN_PACKET_TRANSMISSION;
                            gHttpConnectionState =  MDM_HTTP_CONNECTED;
                        }
                        vPortFree(ConnectionResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to receive connection response in RX mode");
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        vPortFree(ConnectionResponse.response);
                    }
                }
                else
                {
                    /* Wait untill there is a response received from Rx Task */ 
                }
            }
            else
            {

            }
        }
        break;

        case CONNECT_IN_PROGRESS_SET_HTTP_HEADER:
        {
            if(gHttpConnectOpMode == HTTP_CONNECT_OP_TX_MODE)
            {
                buildHttpHeaderWithActiveSessionID(&currentSessionId);

                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = MODEM_PROCESS_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_KHTTP_HEADER;
                        TxMsgQueueData.pData = NULL;
                        TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                        if(TxQueuePushStatus == pdPASS)
                        {
                            xSemaphoreGive(AtTxQueueLoadSemaphore);
                            gHttpConnectOpMode = HTTP_CONNECT_OP_RX_MODE;
                            vTaskDelay(TransmitDelayMs);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to sent connection timer cmd to Tx Task");
                            vTaskDelay(TransmitDelayMs);
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                    }
                }
                else
                {
                    DEBUG_PRINT("Transmit Queue is not empty");
                }
            }
            else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
            {
                if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
                {
                    if(ConnectionResponse.atCmd == CMD_AT_KHTTP_HEADER)
                    {
                        SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
                        DEBUG_PRINT("\r\n");

                        if(false != processHttpHeaderResponse(ConnectionResponse.response))
                        {
                            gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_CLOSE_CONNECTION;
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                            gHttpConnectionState = MDM_HTTP_CONNECTED;
                            gHttpConnectedSubState = CONNECTED_INITIALIZE_TRANSMISSION;
                            DEBUG_PRINT("\r\n");
                            vTaskDelay(PacketTransmitDelayMs);
                        }
                        else
                        {
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        }
                        vPortFree(ConnectionResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to receive connection response in RX mode");
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        vPortFree(ConnectionResponse.response);
                    }
                }
                else
                {
                    /* Wait untill there is a response received from Rx Task */ 
                }
            }
            else
            {

            }
        }
        break;

        default:
        break;
    }

}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
static void MdmCnct_ConnectedSubStateMachine(void)
{
    const TickType_t BuildPacketDelayMs = pdMS_TO_TICKS(500UL);
    const TickType_t TransmitDelayMs = pdMS_TO_TICKS(2500UL);
    const TickType_t ResponseWaitDelayMs = pdMS_TO_TICKS(5000UL);
    const TickType_t reTransmissionDelayMs = pdMS_TO_TICKS(1000UL);
    const TickType_t QueuePushDelayMs = pdMS_TO_TICKS(500UL);

    AtTxMsgType TxMsgQueueData;
    BaseType_t TxQueuePushStatus;

    switch(gHttpConnectedSubState)
    {
        case CONNECTED_INITIALIZE_TRANSMISSION:
        {
            mdmCtrlr_FlushRxBuffer();
            gHttpConnectedSubState = CONNECTED_IDLE_MONITOR_CONNECTION;
        }
        break;

        case CONNECTED_IDLE_MONITOR_CONNECTION:
        {
            if(false != isPacketTransmitTimerExpired())
            {
            	gHttpConnectedSubState = CONNECTED_PERIODIC_6SEC_TIMER_EXPIRED;
            }
        }
        break;

        case CONNECTED_PERIODIC_6SEC_TIMER_EXPIRED:
        {
        	xSemaphoreGive(SensorScanSemaphore);
        	gHttpConnectedSubState = CONNECTED_WAIT_FOR_DATA_FROM_SENSOR_TASK;
        }
        break;

        case CONNECTED_WAIT_FOR_DATA_FROM_SENSOR_TASK:
        {
			gHttpConnectedSubState = CONNECTED_BUILD_DATA_PACKET_TO_SERVER;
        }
        break;

        case CONNECTED_BUILD_DATA_PACKET_TO_SERVER:
        {
            buildDataPacketsToServer();
            gHttpConnectedSubState = CONNECTED_SEND_DATA_PACKETS_TO_SERVER;
        }
        break;

        case CONNECTED_SEND_DATA_PACKETS_TO_SERVER:
        {
            if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
            {
                if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                {
                    TxMsgQueueData.taskID = MODEM_PROCESS_TASK;
                    TxMsgQueueData.atCmd = CMD_AT_KHTTP_GET;
                    TxMsgQueueData.pData = NULL;
                    TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                    if(TxQueuePushStatus == pdPASS)
                    {
                        xSemaphoreGive(AtTxQueueLoadSemaphore);
                        gHttpConnectedSubState = CONNECTED_RECEIVE_RESPONSE_FROM_SERVER;
                        vTaskDelay(TransmitDelayMs);
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to sent connection timer cmd to Tx Task");
                        vTaskDelay(TransmitDelayMs);
                    }
                }
                else
                {
                    DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                }
            }
            else
            {
                DEBUG_PRINT("Transmit Queue is not empty");
            }
        }
        break;

        case CONNECTED_RECEIVE_RESPONSE_FROM_SERVER:
        {
            if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
            {
                if(ConnectionResponse.atCmd == CMD_AT_KHTTP_GET)
                {
                    if(ConnectionResponse.length > 23)
                    {
                        if(false != MdmCnct_validateServerResponse(ConnectionResponse.response))
                        {
                            SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
                            DEBUG_PRINT("\r\n");
                            clearPacketTransmitTimerExpiryFlag();
                            gHttpConnectedSubState = CONNECTED_IDLE_MONITOR_CONNECTION;
                        }
                        else
                        {
                            gHttpConnectedSubState = CONNECTED_FAULT_IN_PACKET_TRANSMISSION;
                        }                       
                    }
                    else
                    {
                        gHttpConnectedSubState = CONNECTED_FAULT_IN_PACKET_TRANSMISSION;
                    }

                    vPortFree(ConnectionResponse.response);
                    vTaskDelay(reTransmissionDelayMs);
                }
                else
                {
                    DEBUG_PRINT("Failed to receive connection response in RX mode");
                    gHttpConnectedSubState = CONNECTED_FAULT_IN_PACKET_TRANSMISSION;
                    vPortFree(ConnectionResponse.response);
                    vTaskDelay(reTransmissionDelayMs);
                }
            }
            else
            {
                /* Response for the data packet is not received on time */
                gHttpConnectedSubState = CONNECTED_FAULT_IN_PACKET_TRANSMISSION;
                vTaskDelay(reTransmissionDelayMs);
            }
        }
        break;

        case CONNECTED_FAULT_IN_PACKET_TRANSMISSION:
        {
            gHttpConnectedSubState = CONNECTED_PEFORM_ERROR_RECOVERY;
            gErrorRecoveryState = CLOSE_ALL_EXISTING_CONNECIONS;
            sessionIdCount = 5;
            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
            DEBUG_PRINT("\r\nConnection interrupted...Performing the Error Recovery....\r\n");
            DEBUG_PRINT("Closing the active connection");
            //vTaskDelay(QueuePushDelayMs);
        }
        break;

        case CONNECTED_PEFORM_ERROR_RECOVERY:
        {
            MdmCnct_PeformErrorRecovery();
        }
        break;
    }
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
static bool MdmCnct_validateServerResponse(uint8_t* response)
{
    bool status = false;
    static uint8_t* normalResponse = "CONNECT\r\nHTTP/1.1 200 OK";

    if(0==memcmp(response,"CONNECT\r\nHTTP/1.1 200 OK",24))
    {
        status = true;
    }
    else
    {
        status = false;
    }
    
    return status;
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
static bool MdmCnct_PeformErrorRecovery(void)
{
    static uint8_t errorRecoveryCnt = 0;
    static uint8_t forcedModemRebootCnt = 0;
    AtTxMsgType TxMsgQueueData;
    BaseType_t TxQueuePushStatus;
    const TickType_t TransmitDelayMs = pdMS_TO_TICKS(500UL);
    const TickType_t ResponseWaitDelayMs = pdMS_TO_TICKS(4000UL);
    const TickType_t QueuePushDelayMs = pdMS_TO_TICKS(500UL);

    switch(gErrorRecoveryState)
    {
        case CLOSE_ALL_EXISTING_CONNECIONS:
        {
            if(gHttpConnectOpMode == HTTP_CONNECT_OP_TX_MODE)
            {
                if(sessionIdCount > 0)
                {
                    if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                    {
                        if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                        {
                            TxMsgQueueData.taskID = MODEM_PROCESS_TASK;
                            TxMsgQueueData.atCmd = getCloseActiveSessionCmd(sessionIdCount);
                            TxMsgQueueData.pData = NULL;
                            TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                            if(TxQueuePushStatus == pdPASS)
                            {
                                xSemaphoreGive(AtTxQueueLoadSemaphore);
                                gHttpConnectOpMode = HTTP_CONNECT_OP_RX_MODE;
                                gHttpConnectedSubState = CONNECTED_PEFORM_ERROR_RECOVERY;
                                vTaskDelay(TransmitDelayMs);
                            }
                            else
                            {
                                DEBUG_PRINT("Failed to sent the Session Close request to Tx Task");
                                vTaskDelay(TransmitDelayMs);
                            }
                        }
                        else
                        {
                            DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Transmit Queue is not empty");
                    }
                }
                else
                {
                    DEBUG_PRINT("No More Active Connections to close");
                    gErrorRecoveryState = BRING_ACTIVE_PDP_CONNECTION_DOWN;
                    gHttpConnectedSubState = CONNECTED_PEFORM_ERROR_RECOVERY;
                }

            }
            else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
            {
                if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
                {
                    if(ConnectionResponse.atCmd == getCloseActiveSessionCmd(sessionIdCount))
                    {
                        if(sessionIdCount > 0)
                        {
                            sessionIdCount--;
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        }
                        else
                        {
                            gErrorRecoveryState = BRING_ACTIVE_PDP_CONNECTION_DOWN;
                            gHttpConnectedSubState = CONNECTED_PEFORM_ERROR_RECOVERY;
                            DEBUG_PRINT("Closed All Active Connections");
                        }
                        vPortFree(ConnectionResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to receive connection response in RX mode");
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        vPortFree(ConnectionResponse.response);
                    }
                }
                else
                {
                    if(forcedModemRebootCnt <= 3)
                    {
                    	DEBUG_PRINT("Problem in Auto Recovery.");
                    	DEBUG_PRINT("Trying to re-establish the connection through a modem restart....\r\n");
                    	forcedModemRebootCnt++;
                    	performForcedRebootOfModem();
                    }
                    else
                    {
                    	DEBUG_PRINT("Maximum retry count for auto recovery is expired.");
                    	DEBUG_PRINT("Trying to re-establish the connection through a whole system restart....\r\n");
                    	forcedModemRebootCnt = 0;
                    	requestWatchDogForcedReset();
                    }
                }
            }
            else
            {

            }
        }
        break;

        case BRING_ACTIVE_PDP_CONNECTION_DOWN:
        {
            if(gHttpConnectOpMode == HTTP_CONNECT_OP_TX_MODE)
            {
                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = MODEM_PROCESS_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_KCNX_DOWN;
                        TxMsgQueueData.pData = NULL;
                        TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                        if(TxQueuePushStatus == pdPASS)
                        {
                            xSemaphoreGive(AtTxQueueLoadSemaphore);
                            gHttpConnectOpMode = HTTP_CONNECT_OP_RX_MODE;
                            gHttpConnectedSubState = CONNECTED_PEFORM_ERROR_RECOVERY;
                            vTaskDelay(TransmitDelayMs);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to sent the Session Close request to Tx Task");
                            vTaskDelay(TransmitDelayMs);
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                    }
                }
                else
                {
                    DEBUG_PRINT("Transmit Queue is not empty");
                }
            }
            else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
            {
                if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
                {
                    if(ConnectionResponse.atCmd == CMD_AT_KCNX_DOWN)
                    {
                        if(false != validateCommonCommandResponse(ConnectionResponse.response))
                        {
                            DEBUG_PRINT("Brought the PDP connection DOWN");
                            SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
                            DEBUG_PRINT("\r\n");
                            gErrorRecoveryState = PDP_PERFORM_PS_CONNECTION_DETACH;
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                            gHttpConnectedSubState = CONNECTED_PEFORM_ERROR_RECOVERY;
                        }
                        else
                        {
                            DEBUG_PRINT("Expected Response Not Received...");
                            DEBUG_PRINT("\r\n");
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                            performForcedRebootOfModem();
                        }
                        vPortFree(ConnectionResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to receive connection response in RX mode");
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        vPortFree(ConnectionResponse.response);
                    }
                }
                else
                {
                    if(forcedModemRebootCnt <= 3)
                    {
                    	DEBUG_PRINT("Problem in Auto Recovery.");
                    	DEBUG_PRINT("Trying to re-establish the connection through a modem restart....\r\n");
                    	forcedModemRebootCnt++;
                    	performForcedRebootOfModem();
                    }
                    else
                    {
                    	DEBUG_PRINT("Maximum retry count for auto recovery is expired.");
                    	DEBUG_PRINT("Trying to re-establish the connection through a whole system restart....\r\n");
                    	forcedModemRebootCnt = 0;
                    	requestWatchDogForcedReset();
                    }
                }
            }
            else
            {

            }
        }
        break;

        case PDP_PERFORM_PS_CONNECTION_DETACH:
        {
            if(gHttpConnectOpMode == HTTP_CONNECT_OP_TX_MODE)
            {
                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = MODEM_PROCESS_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_CGATT;
                        TxMsgQueueData.pData = NULL;
                        TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                        if(TxQueuePushStatus == pdPASS)
                        {
                            xSemaphoreGive(AtTxQueueLoadSemaphore);
                            gHttpConnectOpMode = HTTP_CONNECT_OP_RX_MODE;
                            gHttpConnectedSubState = CONNECTED_PEFORM_ERROR_RECOVERY;
                            vTaskDelay(TransmitDelayMs);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to sent the Session Close request to Tx Task");
                            vTaskDelay(TransmitDelayMs);
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                    }
                }
                else
                {
                    DEBUG_PRINT("Transmit Queue is not empty");
                }
            }
            else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
            {
                if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
                {
                    if(ConnectionResponse.atCmd == CMD_AT_CGATT)
                    {
                        if(false != validateCommonCommandResponse(ConnectionResponse.response))
                        {
                            DEBUG_PRINT("Detached the PDP PS");
                            SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
                            DEBUG_PRINT("\r\n");
                            gErrorRecoveryState = CLOSE_ALL_EXISTING_CONNECIONS;
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                            gHttpConnectedSubState = CONNECTED_INITIALIZE_TRANSMISSION;
                            gHttpConnectionState = MDM_HTTP_DISCONNECTED;
                            gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_CLOSE_CONNECTION;
                            sessionIdCount = 5;

                            DEBUG_PRINT("Error Recovery Completed\r\n");
                            DEBUG_PRINT("=================================================\r\n");
                            DEBUG_PRINT("Establishing a new connection with server");
                            errorRecoveryCnt++;
                            vPortFree(ConnectionResponse.response);

                            if(errorRecoveryCnt >=3)
                            {
                                DEBUG_PRINT("System is auto recovered for more than 3 times.");
                                DEBUG_PRINT("Performing a system restart....................\r\n");
                                errorRecoveryCnt = 0;
                                requestWatchDogForcedReset();
                            }
                        }
                        else
                        {
                            DEBUG_PRINT("Expected Response Not Received...");
                            DEBUG_PRINT("\r\n");
                            gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                            vPortFree(ConnectionResponse.response);
                            performForcedRebootOfModem();
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to receive connection response in RX mode");
                        gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
                        gHttpConnectedSubState = CONNECTED_PEFORM_ERROR_RECOVERY;
                        vPortFree(ConnectionResponse.response);
                    }
                }
                else
                {
                    if(forcedModemRebootCnt <= 3)
                    {
                    	DEBUG_PRINT("Problem in Auto Recovery.");
                    	DEBUG_PRINT("Trying to re-establish the connection through a modem restart....\r\n");
                    	forcedModemRebootCnt++;
                    	performForcedRebootOfModem();
                    }
                    else
                    {
                    	DEBUG_PRINT("Maximum retry count for auto recovery is expired.");
                    	DEBUG_PRINT("Trying to re-establish the connection through a whole system restart....\r\n");
                    	forcedModemRebootCnt = 0;
                    	requestWatchDogForcedReset();
                    }
                }
            }
            else
            {

            }
        }
        break;
    }
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
static void MdmCnct_ExtractSessionIdFromConfigResponse(uint8_t* cfgResponse)
{
    uint8_t sessionIdCountValue = 0;

    /* Update the value for current session ID */
    currentSessionId = cfgResponse[SESSION_ID_POSITION];
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
static bool MdmCnct_VerifyConnectionStatusFromConfigResponse(uint8_t* cfgResponse)
{
    bool status = false;

    connectionStatus = cfgResponse[CONNECT_STATUS_POSITION];

    switch(connectionStatus)
    {
        case PDP_DISCONNECTED_DUE_TO_NETWORK:
        {
            DEBUG_PRINT("Connection Status : DISCONNECTED DUE TO NETWORK");
            status = false;
        }
        break;

        case PDP_CONNECTED:
        {
            DEBUG_PRINT("Connection Status : CONNECTED");
            status = true;
        }
        break;

        case PDP_FAILED_TO_CONNECT:
        {
            DEBUG_PRINT("Connection Status : FAILED TO CONNECT");
            status = false;
        }
        break;

        case PDP_CLOSED:
        {
            DEBUG_PRINT("Connection Status : CONNECTION CLOSED");
            status = false;
        }
        break;

        case PDP_CONNECTION_IN_PROGRESS:
        {
            DEBUG_PRINT("Connection Status : CONNECTION IN PROGRESS");
            status = false;
        }
        break;

        case PDP_IDLE_TIMER_STARTED:
        {
            DEBUG_PRINT("Connection Status : IDLE TIMER STARTED FOR DISCONNECTION");
            status = false;
        }
        break;

        case PDP_IDLE_TIMER_CANCELLED:
        {
            DEBUG_PRINT("Connection Status : IDLE TIMER CANCELLED");
            status = false;
        }
        break;

        default:
        break;
    }

    return status;
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
static bool processHttpHeaderResponse(uint8_t* response)
{
    bool status = false;

    if(0==memcmp(response,"CONNECT",7))
    {
        mdmCtrlr_SendDataToModem("--EOF--Pattern--",16);
        mdmCtrlr_FlushRxBuffer();
        status = true;
    }
    else
    {
        status = false;
    }

    return status;
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
static bool validateCommonCommandResponse(uint8_t* response)
{
    bool status = false;

    if(0==memcmp(response,"OK",2))
    {
        status = true;
    }
    else
    {
        status = false;
    }

    return status;
}
/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
static AT_CMD_TYPE getCloseActiveSessionCmd(uint8_t sessionID)
{
    AT_CMD_TYPE sessionCloseCmd;

    switch(sessionID)
    {
        case 1:
        {
            sessionCloseCmd = CMD_AT_KHTTP_CLOSE_1;
        }
        break;

        case 2:
        {
            sessionCloseCmd = CMD_AT_KHTTP_CLOSE_2;
        }
        break;

        case 3:
        {
            sessionCloseCmd = CMD_AT_KHTTP_CLOSE_3;
        }
        break;

        case 4:
        {
            sessionCloseCmd = CMD_AT_KHTTP_CLOSE_4;
        }
        break;

        case 5:
        {
            sessionCloseCmd = CMD_AT_KHTTP_CLOSE_5;
        }
        break;

        case 6:
        {
            sessionCloseCmd = CMD_AT_KHTTP_CLOSE_6;
        }
        break;

        case 7:
        {
            sessionCloseCmd = CMD_AT_KHTTP_CLOSE_7;
        }
        break;

        case 8:
        {
            sessionCloseCmd = CMD_AT_KHTTP_CLOSE_8;
        }
        break;

        case 9:
        {
            sessionCloseCmd = CMD_AT_KHTTP_CLOSE_9;
        }
        break;

        case 10:
        {
            sessionCloseCmd = CMD_AT_KHTTP_CLOSE_10;
        }
        break;

        default:
        break;
    }
    
    return sessionCloseCmd;
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
static void performForcedRebootOfModem(void)
{
	DEBUG_PRINT("Connection to the server is lost.....");
	DEBUG_PRINT("Trying to establish the connection to server...Please wait......\r\n");

	/* Perform a physical modem restart */
	modemPowerStateInit();

	/* Reset the modem connection States */
	MdmConnect_HttpConnectionInit();
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
void performForcedErrorRecovery(void)
{
	DEBUG_PRINT("Performing forced error recovery....\r\n");

	/* Perform a physical modem restart */
	modemPowerStateInit();

	/* Reset the modem connection States */
	MdmConnect_HttpConnectionInit();
}
