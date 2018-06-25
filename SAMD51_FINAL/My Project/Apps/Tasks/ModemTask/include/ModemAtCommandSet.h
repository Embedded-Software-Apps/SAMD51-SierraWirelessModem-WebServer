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
#include "Apps/Common/Common.h"
/******************************************************************************************
************************************GLOBAL VARIABLES***************************************
*******************************************************************************************/
#define TOTAL_MODEM_CMDS (60)
#define CRLF_CHAR_LEN (4)
#define MAX_RESPONSE_SIZE (700)

/* Supported AT Command Set */
typedef enum
{
	/* Basic Commands */
	CMD_AT_MAX = 0,
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
	CMD_AT_CGATT
}AT_CMD_TYPE;

typedef void (*ResponseHandler)(uint8_t* response,uint8_t length);

typedef struct
{
	AT_CMD_TYPE AtCmd;
	uint8_t* AtString;
	uint8_t CmdLength;
	uint16_t validDataCnt;
	ResponseHandler respHandler;
	uint16_t ResponseLength;
}MODEM_CMD_DATA;

typedef enum
{
	DISPATCH_TASK = 0,
	MODEM_PROCESS_TASK,
	MODEM_TX_TASK,
	MODEM_RX_TASK,
	MODEM_DIAG_TASK
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

void getModemCommandData(AT_CMD_TYPE cmd, MODEM_CMD_DATA* cmdData);
void mdmParser_SetKhttpHeaderString(uint8_t* sessionID);
#endif /* MODEMATCOMMANDSET_H_ */
