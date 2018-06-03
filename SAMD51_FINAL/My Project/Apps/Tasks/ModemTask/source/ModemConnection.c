/*
 * ModemConnection.c
 *
 * Created: 5/22/2018 12:51:38 PM
 *  Author: anilj
 */ 
#include "Apps/Tasks/ModemTask/include/ModemConnection.h"
#include "Apps/Tasks/ModemTask/include/ModemCmdParser.h"

/* Global variables */
static HTTP_CONNECT_STATES_T gHttpMainState;
static HTTP_CONNECT_IN_PROGRESS_SUBSTATES_T gHttpCnctnPrgrsSubstate;

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
void MdmCnct_HttpConnectionStateMachine(void)
{
	switch (gHttpMainState)
	{
        case MDM_HTTP_DISCONNECTED:
        {
        	gHttpMainState = MDM_HTTP_CONNECTION_IN_PROGRESS;
        	DEBUG_PRINT("In MDM_HTTP_DISCONNECTED");
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
	static uint8_t sessionIDIndex=0;

	switch (gHttpCnctnPrgrsSubstate)
	{
        case CONNECT_IN_PROGRESS_CLOSE_CONNECTION:
        {
        	if(sessionIDIndex < MAX_ACTIVE_SESSION_ID)
        	{
        		if(false != mdmParser_IsLastCmdProcessed())
        		{
        			DEBUG_PRINT("Closing Connection");
        			mdmParser_SendCommandToModem(CMD_AT_KHTTP_CLOSE_1 + sessionIDIndex);
        			sessionIDIndex++;
        		}
        	}
        	else
        	{
        		DEBUG_PRINT("Closed all connections");
        		sessionIDIndex = 0;
        		gHttpCnctnPrgrsSubstate = CONNECT_IN_PROGRESS_SET_EOF_PATTERN;
        	}
        }
        break;

        case CONNECT_IN_PROGRESS_SET_EOF_PATTERN:
        {
        	DEBUG_PRINT("In CONNECT_IN_PROGRESS_SET_EOF_PATTERN");
        	gHttpCnctnPrgrsSubstate = CONNECT_IN_PROGRESS_SET_ACCESS_POINT;
        }
        break;

        case CONNECT_IN_PROGRESS_SET_ACCESS_POINT:
        {

        }
        break;

        case CONNECT_IN_PROGRESS_SET_CONNECT_TIMERS:
        {

        }
        break;

        case CONNECT_IN_PROGRESS_SET_SERVER_ADDRESS:
        {

        }
        break;

        case CONNECT_IN_PROGRESS_SET_HTTP_HEADER:
        {

        }
        break;

        default:
        break;
	}

}
