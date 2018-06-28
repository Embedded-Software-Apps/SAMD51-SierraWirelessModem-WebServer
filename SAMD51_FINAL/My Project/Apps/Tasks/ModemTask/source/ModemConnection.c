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
static HTTP_CONNECTED_SUBSTATES_T gHttpConnectedSubState;
static CONNECTION_ERROR_RECOVERY_STATE_T gErrorRecoveryState;
static uint8_t sessionIdCount = 0;
static CmdResponseType ConnectionResponse;
static uint8_t currentSessionId;

static void MdmCnct_CloseActiveConnections(void);
static AT_CMD_TYPE getCloseActiveSessionCmd(uint8_t sessionID);
static void MdmCnct_ExtractSessionIdFromConfigResponse(uint8_t* response);
static bool processHttpHeaderResponse(uint8_t* response);
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
	        	SerialDebugPrint("Closing Active Connections\r\n",28);
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
    const TickType_t ResponseWaitDelayMs = pdMS_TO_TICKS(300UL);
    const TickType_t PacketTransmitDelayMs = pdMS_TO_TICKS(1500UL);

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
        			gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_SET_EOF_PATTERN;
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
			        		//SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
							//SerialDebugPrint("\r\n",2);
		        		}
		        		else
		        		{
			        		gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_SET_EOF_PATTERN;
			        		SerialDebugPrint("Closed All Active Connections\r\n",31);
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
        		if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
				{
	        		if(ConnectionResponse.atCmd == CMD_AT_KPATTERN)
	        		{
	        			SerialDebugPrint("EOF Pattern configured\r\n",24);
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
        		if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
				{
	        		if(ConnectionResponse.atCmd == CMD_AT_KCNXCFG)
	        		{
	        			SerialDebugPrint("Access Point configured\r\n",25);
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
        		if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
				{
	        		if(ConnectionResponse.atCmd == CMD_AT_KCNXTIMER)
	        		{
	        			SerialDebugPrint("Connection Timers configured\r\n",30);
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
        		if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
				{
	        		if(ConnectionResponse.atCmd == CMD_AT_KHTTP_CFG)
	        		{
	        			SerialDebugPrint("Cloud Server configured\r\n",25);
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
        		if(pdPASS == xQueueReceive( CmdResponseQueue, &ConnectionResponse, ResponseWaitDelayMs))
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
							gHttpConnectedSubState = CONNECTED_INITIALIZE_TRANSMISSION;
							SerialDebugPrint("Successfully Established the connection with server\r\n",53);
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
	const TickType_t ResponseWaitDelayMs = pdMS_TO_TICKS(2000UL);
	const TickType_t reTransmissionDelayMs = pdMS_TO_TICKS(7000UL);
	const TickType_t QueuePushDelayMs = pdMS_TO_TICKS(500UL);

    AtTxMsgType TxMsgQueueData;
    BaseType_t TxQueuePushStatus;


	switch(gHttpConnectedSubState)
	{
		case CONNECTED_INITIALIZE_TRANSMISSION:
		{
			mdmCtrlr_FlushRxBuffer();
			gHttpConnectedSubState = CONNECTED_BUILD_DATA_PACKET_TO_SERVER;
		}
		break;

		case CONNECTED_WAIT_FOR_PERIODIC_TIMER_EXPIRY:
		{

		}
		break;

		case CONNECTED_BUILD_DATA_PACKET_TO_SERVER:
		{
			buildDataPacketsToServer();
			gHttpConnectedSubState = CONNECTED_SEND_DATA_PACKETS_TO_SERVER;
			vTaskDelay(BuildPacketDelayMs);
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
        			if(false != MdmCnct_validateServerResponse(ConnectionResponse.response))
        			{
    	        		SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
    					SerialDebugPrint("\r\n",2);
    					SerialDebugPrint("\r\n",2);
    					gHttpConnectedSubState = CONNECTED_SEND_DATA_PACKETS_TO_SERVER;
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
			SerialDebugPrint("Performing the Error Recovery\r\n",31);
			vTaskDelay(QueuePushDelayMs);
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
    AtTxMsgType TxMsgQueueData;
    BaseType_t TxQueuePushStatus;
	const TickType_t TransmitDelayMs = pdMS_TO_TICKS(2500UL);
	const TickType_t ResponseWaitDelayMs = pdMS_TO_TICKS(2000UL);
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
        			gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_SET_EOF_PATTERN;
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
			        		//SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
							//SerialDebugPrint("\r\n",2);
		        		}
		        		else
		        		{
		        			gErrorRecoveryState = BRING_ACTIVE_PDP_CONNECTION_DOWN;
		        			gHttpConnectedSubState = CONNECTED_PEFORM_ERROR_RECOVERY;
			        		SerialDebugPrint("Closed All Active Connections\r\n",31);
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
	        			SerialDebugPrint("Brought the PDP connection DOWN\r\n",33);
		        		SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
						SerialDebugPrint("\r\n",2);
						gErrorRecoveryState = PDP_PERFORM_PS_CONNECTION_DETACH;
						gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
						gHttpConnectedSubState = CONNECTED_PEFORM_ERROR_RECOVERY;
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
	        			SerialDebugPrint("Detached the PDP PS\r\n",21);
		        		SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
						SerialDebugPrint("\r\n",2);
						gErrorRecoveryState = CLOSE_ALL_EXISTING_CONNECIONS;
						gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
						gHttpConnectedSubState = CONNECTED_INITIALIZE_TRANSMISSION;
						gHttpConnectionState = MDM_HTTP_DISCONNECTED;
						gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_CLOSE_CONNECTION;
						sessionIdCount = 5;

						SerialDebugPrint("Error Recovery Completed\r\n\r\n",28);
						SerialDebugPrint("=================================================\r\n\r\n",54);
						SerialDebugPrint("Establishing a new connection with server\r\n",43);
						errorRecoveryCnt++;
		        		vPortFree(ConnectionResponse.response);
	        		}
	        		else
	        		{
		        		DEBUG_PRINT("Failed to receive connection response in RX mode");
		        		gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
		        		gHttpConnectedSubState = CONNECTED_PEFORM_ERROR_RECOVERY;
		        		vPortFree(ConnectionResponse.response);
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
