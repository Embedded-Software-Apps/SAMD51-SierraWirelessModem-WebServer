/*=======================================================================================
 * ModemAtCommandSet.h
 *
 * Implements the AT Command set and related functionalities.
 *======================================================================================*/
#ifndef MODEMATCOMMANDSET_H_
#define MODEMATCOMMANDSET_H_

/****************************************************************************************
 INCLUDE FILES
*****************************************************************************************/
#include "Application/Common/Common.h"
/******************************************************************************************
************************************GLOBAL VARIABLES***************************************
*******************************************************************************************/
#define TOTAL_MODEM_CMDS (60)
#define CRLF_CHAR_LEN (4)
#define MAX_RESPONSE_SIZE (700)

/* Supported AT Command Set */
typedef enum
{
	CMD_AT_MAX = 0,

	/* Diagnostics Commands */
	CMD_AT,
	CMD_AT_CGSN,
	CMD_AT_WCARRIER,
	CMD_AT_IPR,
	CMD_AT_CPIN,
	CMD_AT_CGREG,
	CMD_AT_KGSN,
	CMD_AT_ATE,

	/* Connection related Commands */
	CMD_AT_KHTTP_CFG,
	CMD_AT_KHTTP_CLOSE_1,
	CMD_AT_KHTTP_CLOSE_2,
	CMD_AT_KHTTP_CLOSE_3,
	CMD_AT_KHTTP_CLOSE_4,
	CMD_AT_KHTTP_CLOSE_5,
	CMD_AT_KHTTP_CLOSE_6,
	CMD_AT_KHTTP_CLOSE_7,
	CMD_AT_KHTTP_CLOSE_8,
	CMD_AT_KHTTP_CLOSE_9,
	CMD_AT_KHTTP_CLOSE_10,
	CMD_AT_KPATTERN,
	CMD_AT_KCNXCFG,
	CMD_AT_KCNXTIMER,
	CMD_AT_KHTTP_HEADER,
	CMD_AT_KHTTP_GET,
	CMD_AT_TERMINATE_HEADER,
	CMD_AT_KCNX_DOWN,
	CMD_AT_CGATT,

	/* FOTA related Commands */
	CMD_AT_WDSS_SET_APN,
	CMD_AT_WDSS_START_DM_SESSION,
	CMD_AT_WDSI,
	CMD_AT_WDSC_FW_DOWNLOAD,
	CMD_AT_WDSC_FW_INSTALL,
	CMD_AT_WDSR_ACCEPT_FW_DOWNLOAD,
	CMD_AT_WDSR_ACCEPT_FW_INSTALL
}AT_CMD_TYPE;

typedef enum
{
	AT_CMD_SET_DIAGNOSTICS,
	AT_CMD_SET_CONNECTION,
	AT_CMD_SET_FOTA_APP,
	AT_CMD_SET_MAX
}AT_CMD_SET;

typedef void (*ResponseHandler)(AT_CMD_TYPE AtCmd, uint8_t* response, uint16_t length);

typedef struct
{
	AT_CMD_TYPE AtCmd;
	AT_CMD_SET cmdSet;
	uint8_t* AtString;
	uint8_t CmdLength;
	uint16_t validDataCnt;
	ResponseHandler respHandler;
	uint16_t ResponseLength;
}MODEM_CMD_DATA;

typedef enum
{
	SENSOR_TASK = 0,
	MODEM_PROCESS_TASK,
	MODEM_TX_TASK,
	MODEM_RX_TASK,
	MODEM_DIAG_TASK,
	FOTA_APP_TASK
}TASK_ID_TYPE;

typedef struct
{
	TASK_ID_TYPE taskID;
	AT_CMD_TYPE atCmd;
	uint32_t pData;
}AtTxMsgType;


typedef struct
{
	AT_CMD_TYPE atCmd;
}AtRxMsgType;

typedef struct
{
	AT_CMD_TYPE atCmd;
	uint16_t length;
	uint8_t* response;
}CmdResponseType;



void getModemCommandData(AT_CMD_TYPE cmd, MODEM_CMD_DATA* cmdData);
void buildHttpHeaderWithActiveSessionID(const uint8_t* activeSessionId);
void buildDataPacketsToServer(void);
#endif /* MODEMATCOMMANDSET_H_ */
