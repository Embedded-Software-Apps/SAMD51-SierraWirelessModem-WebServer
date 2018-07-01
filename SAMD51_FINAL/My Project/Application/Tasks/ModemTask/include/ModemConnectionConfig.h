/*
 * ModemConnectionConfig.h
 *
 * Created: 5/22/2018 7:31:34 PM
 *  Author: Anil Joseph
 */ 


#ifndef MODEMCONNECTIONCONFIG_H_
#define MODEMCONNECTIONCONFIG_H_

/* Define the Access Point Name */
#define MODEM_APN "VZWINTERNET"

/* Define the Server Name */
#define HTTP_SERVER "ingest1.response.cloud"

/* PDP CONNECTION STATUS reported from Modem */
typedef enum
{
	PDP_DISCONNECTED_DUE_TO_NETWORK = 0,
	PDP_CONNECTED = 1,
	PDP_FAILED_TO_CONNECT = 2,
	PDP_CLOSED = 3,
	PDP_CONNECTION_IN_PROGRESS = 4,
	PDP_IDLE_TIMER_STARTED = 5,
	PDP_IDLE_TIMER_CANCELLED = 6
}PDP_CONNECTION_STATUS;

/* Session ID for HTTP connection */
typedef enum
{
	SESSION_ID_1 = 49,
	SESSION_ID_2 = 50,
	SESSION_ID_3 = 51,
	SESSION_ID_4 = 52,
	SESSION_ID_5 = 53,
	SESSION_ID_6 = 54,
	SESSION_ID_7 = 55,
	SESSION_ID_8 = 56,
	SESSION_ID_9 = 57
}HTTP_SESSION_ID;

/* PDP Context Configuration */
#define PDP_CONTEXT_CONFIG_1 (1)
#define PDP_CONTEXT_CONFIG_2 (2)
#define PDP_CONTEXT_CONFIG_3 (3)
#define PDP_CONTEXT_CONFIG_4 (4)
#define PDP_CONTEXT_CONFIG_5 (5)

/* PDP Connection Parameters */
#define PDP_CONNECTION_RETRY_TIMEOUT (60)
#define PPD_CONNECTION_MAX_RETRY_COUNT (2)
#define PDP_CONNECTION_MIN_TIMEOUT (70)
#define PDP_CONNECTION_IDLE_TIMEOUT (60)
#define DEFAULT_EOF_PATTERN "--EOF--Pattern--"




#endif /* MODEMCONNECTIONCONFIG_H_ */
