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
	CMD_AT = 0,
	CMD_AT_CGSN = 1,
	CMD_AT_WCARRIER = 2,
	CMD_AT_IPR = 3,
	CMD_AT_CPIN = 4,
	CMD_AT_CGREG = 5,
	CMD_AT_KGSN = 6,
	CMD_AT_ATE = 7,

	/* Connection related Commands */
	CMD_AT_KHTTP_CFG = 8,
	CMD_AT_KHTTP_CLOSE_1 = 9,
	CMD_AT_KHTTP_CLOSE_2 = 10,
	CMD_AT_KHTTP_CLOSE_3 = 11,
	CMD_AT_KHTTP_CLOSE_4 = 12,
	CMD_AT_KHTTP_CLOSE_5 = 13,
	CMD_AT_KHTTP_CLOSE_6 = 14,
	CMD_AT_KHTTP_CLOSE_7 = 15,
	CMD_AT_KHTTP_CLOSE_8 = 16,
	CMD_AT_KHTTP_CLOSE_9 = 17,
	CMD_AT_KHTTP_CLOSE_10 = 18,
	CMD_AT_KPATTERN = 19,
	CMD_AT_KCNXCFG = 20,
	CMD_AT_KCNXTIMER = 21,
	CMD_AT_KHTTP_HEADER = 22,
	CMD_AT_KHTTP_GET = 23,
	CMD_AT_TERMINATE_HEADER = 24,
	CMD_AT_KCNX_DOWN = 25,
	CMD_AT_CGATT = 26,
	CMD_AT_MAX
}AT_CMD_TYPE;

typedef void (*ResponseHandler)(uint8_t* response,uint8_t length);

typedef struct
{
	AT_CMD_TYPE AtCmd;
	uint8_t* AtString;
	INTEGER_T CmdLength;
	uint16_t validDataCnt;
	ResponseHandler respHandler;
	uint16_t ResponseLength;
	
}MODEM_CMD_DATA;

void getModemCommandData(AT_CMD_TYPE cmd, MODEM_CMD_DATA* cmdData);
void mdmParser_SetKhttpHeaderString(uint8_t* sessionID);
#endif /* MODEMATCOMMANDSET_H_ */
