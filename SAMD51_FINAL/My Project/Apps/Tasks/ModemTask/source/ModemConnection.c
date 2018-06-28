/*
 * ModemConnection.c
 *
 * Created: 5/22/2018 12:51:38 PM
 *  Author: anilj
 */ 
#include "Apps/Tasks/ModemTask/include/ModemConnection.h"
#include "Apps/Tasks/ModemTask/include/ModemCmdParser.h"
#include "Apps/Tasks/ModemTask/include/ModemAtCommandSet.h"

/* Global variables */
static HTTP_CONNECT_STATES_T gHttpConnectionState;
static HTTP_CONNECT_IN_PROGRESS_SUBSTATES_T gHttpConnectionInProgressSubstate;
static HTTP_CONNECT_OPERATIONAL_STATE_T gHttpConnectOpMode;
static HTTP_CLOSE_CONNECTIONS_STATE_T gHttpCloseConnectionsState;
static uint8_t sessionIdCount = 0;
static CmdResponseType ConnectionResponse;

static void MdmCnct_CloseActiveConnections(void);
static AT_CMD_TYPE getCloseActiveSessionCmd(uint8_t sessionID);
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
	sessionIdCount = 3;

	ConnectionResponse.atCmd = CMD_AT_MAX;
	ConnectionResponse.length = 0;
	ConnectionResponse.response = NULL;
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
        	gHttpConnectionState = MDM_HTTP_CONNECTION_IN_PROGRESS;
        	gHttpConnectionInProgressSubstate = CONNECT_IN_PROGRESS_CLOSE_CONNECTION;
        	gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
        }
        break;

        case MDM_HTTP_CONNECTION_IN_PROGRESS:
        {
        	MdmCnct_ConnectInProgressSubStateMachine();
        }
        break;

        case MDM_HTTP_CONNECTED:
        {

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
    const TickType_t TransmitDelayMs = pdMS_TO_TICKS(1000UL);

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
                                DEBUG_PRINT("Sent the Session Close request to Tx Task");
                                gHttpConnectOpMode = HTTP_CONNECT_OP_RX_MODE;
                                vTaskDelay(TransmitDelayMs);
                            }
                            else
                            {
                                DEBUG_PRINT("Failed to sent the Session Close request to Tx Task");
                                vTaskDelay(TransmitDelayMs);
                            }
        		        }
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
        		xQueueReceive( CmdResponseQueue, &ConnectionResponse, portMAX_DELAY );

        		if(ConnectionResponse.atCmd == getCloseActiveSessionCmd(sessionIdCount))
        		{
        			DEBUG_PRINT("Received a connection response in RX Mode");

        			if(sessionIdCount > 0)
        			{
        				sessionIdCount--;
        				gHttpConnectOpMode = HTTP_CONNECT_OP_TX_MODE;
        				SerialDebugPrint(ConnectionResponse.response,ConnectionResponse.length);
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
        	else
        	{

        	}
        }
        break;

        case CONNECT_IN_PROGRESS_SET_EOF_PATTERN:
        {
        	if(gHttpConnectOpMode == HTTP_CONNECT_OP_TX_MODE)
        	{

        	}
        	else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
        	{

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

        	}
        	else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
        	{

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

        	}
        	else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
        	{

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

        	}
        	else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
        	{

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

        	}
        	else if(gHttpConnectOpMode == HTTP_CONNECT_OP_RX_MODE)
        	{

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
static void MdmCnct_CloseActiveConnections(void)
{

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
}
