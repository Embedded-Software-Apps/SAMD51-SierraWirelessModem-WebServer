/*
 * ModemConnection.h
 *
 * Created: 5/22/2018 12:51:22 PM
 *  Author: anilj
 */ 


#ifndef MODEMCONNECTION_H_
#define MODEMCONNECTION_H_

#include "Application/SerialDebug/SerialDebug.h"

/* Main states for Modem Connection */
typedef enum
{
	MDM_HTTP_DISCONNECTED,
	MDM_HTTP_CONNECTION_IN_PROGRESS,
	MDM_HTTP_CONNECTED,
	MDM_HTTP_DISCONNECTION_IN_PROGRESS,
	MDM_HTTP_CONNECTION_FAULT
}HTTP_CONNECT_STATES_T;

/* MDM_HTTP_CONNECTION_IN_PROGRESS sub-states */
typedef enum
{
	CONNECT_IN_PROGRESS_CLOSE_CONNECTION,
	CONNECT_IN_PROGRESS_PDP_CONNECTION_DOWN,
	CONNECT_IN_PROGRESS_PS_CONNECTION_DETACH,
	CONNECT_IN_PROGRESS_SET_EOF_PATTERN,
	CONNECT_IN_PROGRESS_SET_ACCESS_POINT,
	CONNECT_IN_PROGRESS_SET_CONNECT_TIMERS,
	CONNECT_IN_PROGRESS_SET_SERVER_ADDRESS,
	CONNECT_IN_PROGRESS_SET_HTTP_HEADER
}HTTP_CONNECT_IN_PROGRESS_SUBSTATES_T;

/* MDM_HTTP_CONNECTION_IN_PROGRESS sub-states */
typedef enum
{
	CONNECTED_INITIALIZE_TRANSMISSION,
	CONNECTED_IDLE_MONITOR_CONNECTION,
	CONNECTED_PERIODIC_6SEC_TIMER_EXPIRED,
	CONNECTED_WAIT_FOR_DATA_FROM_SENSOR_TASK,
	CONNECTED_BUILD_DATA_PACKET_TO_SERVER,
	CONNECTED_SEND_DATA_PACKETS_TO_SERVER,
	CONNECTED_RECEIVE_RESPONSE_FROM_SERVER,
	CONNECTED_FAULT_IN_PACKET_TRANSMISSION,
	CONNECTED_PEFORM_ERROR_RECOVERY
}HTTP_CONNECTED_SUBSTATES_T;

typedef enum
{
	HTTP_CONNECT_OP_TX_MODE,
	HTTP_CONNECT_OP_RX_MODE,
	HTTP_CONNECT_OP_MAX_MODE
}HTTP_CONNECT_OPERATIONAL_STATE_T;

typedef enum
{
	CLOSE_ALL_EXISTING_CONNECIONS,
	BRING_ACTIVE_PDP_CONNECTION_DOWN,
	PDP_PERFORM_PS_CONNECTION_DETACH
}CONNECTION_ERROR_RECOVERY_STATE_T;

#define MAX_ACTIVE_SESSION_ID     (10)
#define SESSION_ID_POSITION       (11)
#define CONNECT_STATUS_POSITION   (55)
#define SESSION_ID_POS_IN_HEADER  (15)
#define SESSION_ID_POS_IN_GET_REQ (12)

void MdmCnct_ConnectInProgressSubStateMachine(void);
void MdmConnect_HttpConnectionSchedule(void);
void MdmConnect_HttpConnectionInit(void);
void performForcedErrorRecovery(void);

#endif /* MODEMCONNECTION_H_ */
