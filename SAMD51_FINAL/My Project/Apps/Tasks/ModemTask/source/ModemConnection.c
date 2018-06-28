/*
 * ModemConnection.c
 *
 * Created: 5/22/2018 12:51:38 PM
 *  Author: anilj
 */ 
#include "Apps/Tasks/ModemTask/include/ModemConnection.h"
#include "Apps/Tasks/ModemTask/include/ModemCmdParser.h"
#include "Apps/Tasks/ModemTask/include/ModemAtCommandSet.h"
#include "Apps/Tasks/ModemTask/include/ModemConnectionConfig.h"
#include "Apps/Common/Common.h"

/* Global variables */
static HTTP_CONNECT_STATES_T gHttpConnectionState;
static HTTP_CONNECT_IN_PROGRESS_SUBSTATES_T gHttpConnectionInProgressSubstate;
static HTTP_CONNECT_OPERATIONAL_STATE_T gHttpConnectOpMode;
static HTTP_CLOSE_CONNECTIONS_STATE_T gHttpCloseConnectionsState;
static uint8_t sessionIdCount = 0;
static CmdResponseType ConnectionResponse;
static uint8_t currentSessionId;

static void MdmCnct_CloseActiveConnections(void);
static AT_CMD_TYPE getCloseActiveSessionCmd(uint8_t sessionID);
static void MdmCnct_ExtractSessionIdFromConfigResponse(uint8_t* response);
static bool processHttpHeaderResponse(uint8_t* response);
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
	gHttpCloseConnectionsState = GET_TOTAL_NO_OF_ACTIVE_CONNECTIONS;
	sessionIdCount = 10;

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
			mdmCtrlr_FlushRxBuffer();
        	SerialDebugPrint("HTTP CONNECTED",14);
        	gHttpConnectionState = MDM_HTTP_DISCONNECTION_IN_PROGRESS;
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
    const TickType_t ResponseWaittDelayMs = pdMS_TO_TICKS(300UL);
	
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
                                if( xSemaphoreTake( DebugPrintMutex,portMAX_DELAY) == pdTRUE )
                                {
                                	DEBUG_PRINT("Sent the Session Close request to Tx Task");
                                	xSemaphoreGive(DebugPrintMutex);
                                }
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
        			gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_SET_EOF_PATTERN;
        		}

        	}
        	else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
        	{
        		if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaittDelayMs))
				{
	        		if(ConnectionResponse.atCmd == getCloseActiveSessionCmd(sessionIdCount))
	        		{
		        		if( xSemaphoreTake( DebugPrintMutex,portMAX_DELAY) == pdTRUE )
		        		{
			        		DEBUG_PRINT("Received a connection response in RX Mode");
			        		xSemaphoreGive(DebugPrintMutex);
		        		}

		        		if(sessionIdCount > 0)
		        		{
			        		sessionIdCount--;
			        		gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
			        		SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
							SerialDebugPrint("\r\n",2);
		        		}
		        		else
		        		{
			        		gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_SET_EOF_PATTERN;
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
        		if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaittDelayMs))
				{
	        		if(ConnectionResponse.atCmd == CMD_AT_KPATTERN)
	        		{
		        		SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
						SerialDebugPrint("\r\n",2);
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
        		if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaittDelayMs))
				{
	        		if(ConnectionResponse.atCmd == CMD_AT_KCNXCFG)
	        		{
		        		SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
						SerialDebugPrint("\r\n",2);
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
        		if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaittDelayMs))
				{
	        		if(ConnectionResponse.atCmd == CMD_AT_KCNXTIMER)
	        		{
		        		SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
						SerialDebugPrint("\r\n",2);
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
        		if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaittDelayMs))
				{
	        		if(ConnectionResponse.atCmd == CMD_AT_KHTTP_CFG)
	        		{
		        		SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
						SerialDebugPrint("\r\n",2);
						MdmCnct_ExtractSessionIdFromConfigResponse(ConnectionResponse.response);
						gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_SET_HTTP_HEADER;
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
        		if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaittDelayMs))
				{
	        		if(ConnectionResponse.atCmd == CMD_AT_KHTTP_HEADER)
	        		{
		        		SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
						SerialDebugPrint("\r\n",2);

						if(false != processHttpHeaderResponse(ConnectionResponse.response))
						{
							gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_CLOSE_CONNECTION;
							gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
							gHttpConnectionState = MDM_HTTP_CONNECTED;
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
