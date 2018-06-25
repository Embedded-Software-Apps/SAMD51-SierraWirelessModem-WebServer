/*=======================================================================================
 * ModemAtCommandSet.c
 *
 * Implements the AT Command set and related functionalities.
 *======================================================================================*/

/****************************************************************************************
 INCLUDE FILES
*****************************************************************************************/
#include "Apps/Common/Common.h"
#include "Apps/Tasks/ModemTask/include/ModemAtCommandSet.h"
#include "Apps/Tasks/ModemTask/include/ModemResponseHandles.h"

uint8_t kHttpHeaderString[INT_SEVENTEEN] = {'A','T','+','K','H','T','T','P','H','E','A','D','E','R','=','0','\r'};

uint8_t kHttpGetString[INT_FIFTEEN] = {'A','T','+','K','H','T','T','P','G','E','T','=','0',',','\0'};

uint8_t kHttpGetCompleteData[60] = {0};

/* Data structure for storing command parameters */
static const MODEM_CMD_DATA ModemCmdData[TOTAL_MODEM_CMDS] = \
{
	/* Default */
	{
		CMD_AT_MAX,
		"",
		INT_ZERO,
		INT_ZERO,
		defaultFunctionPointer,
		INT_ZERO
	},

	/* AT */
	{
		CMD_AT,
		"AT\r",
		INT_THREE,
		INT_TWO,
		mdmResp_AtRespHandler,
		(INT_THREE + INT_TWO + CRLF_CHAR_LEN)
	},

	/* AT + CGSN*/
	{
		CMD_AT_CGSN,
		"AT+CGSN\r",
		INT_EIGHT,
		INT_FIFTEEN,
		mdmResp_IMEIRespHandler,
		(INT_EIGHT + INT_FIFTEEN + CRLF_CHAR_LEN)
	},

	/* AT+WCARRIER */
	{
		CMD_AT_WCARRIER,
		"AT+WCARRIER\r",
		INT_TWELEVE,
		INT_FOURTEEN,
		mdmResp_CarrierRespHandler,
		(INT_TWELEVE + INT_FOURTEEN + CRLF_CHAR_LEN)
	},

	/* AT+IPR */
	{
		CMD_AT_IPR,
		"AT+IPR?\r",
		INT_EIGHT,
		INT_TWELEVE,
		defaultFunctionPointer,
		(INT_EIGHT + INT_TWELEVE + CRLF_CHAR_LEN)
	},

	/* AT+CPIN */
	{
		CMD_AT_CPIN,
		"AT+CPIN?\r",
		INT_FIVE,
		INT_EIGHT,
		defaultFunctionPointer,
		(INT_FIVE + INT_EIGHT + CRLF_CHAR_LEN)
	},

	/* AT+CGREG */
	{
		CMD_AT_CGREG,
		"AT+CGREG?\r",
		INT_TEN,
		INT_ELEVEN,
		defaultFunctionPointer,
		(INT_TEN + INT_ELEVEN + CRLF_CHAR_LEN)
	},

	/* AT+KGSN */
	{
		CMD_AT_KGSN,
		"AT+KGSN=3\r",
		INT_TEN,
		INT_TWENTY_ONE,
		mdmResp_SerialNoRespHandler,
		(INT_TEN + INT_TWENTY_ONE + CRLF_CHAR_LEN)
	},

	/* ATE0 */
	{
		CMD_AT_ATE,
		"ATE1\r",
		INT_FIVE,
		INT_TWO,
		defaultFunctionPointer,
		(INT_FIVE + INT_TWO + CRLF_CHAR_LEN)
	},

	/* Connection Initialization Commands */
	{
		CMD_AT_KHTTP_CFG,
		"AT+KHTTPCFG=3,\"ingest1.response.cloud\"\r",
		INT_THIRTY_NINE,
		INT_TWELEVE,
		mdmResp_KhttpCfgHandler,
		(INT_THIRTY_NINE + INT_TWELEVE + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_1,
		"AT+KHTTPCLOSE=1,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_2,
		"AT+KHTTPCLOSE=2,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_3,
		"AT+KHTTPCLOSE=3,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_4,
		"AT+KHTTPCLOSE=4,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_5,
		"AT+KHTTPCLOSE=5,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_6,
		"AT+KHTTPCLOSE=6,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_7,
		"AT+KHTTPCLOSE=7,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_8,
		"AT+KHTTPCLOSE=8,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_9,
		"AT+KHTTPCLOSE=9,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_10,
		"AT+KHTTPCLOSE=10,0\r",
		INT_NINETEEN,
		INT_TWO,
		mdmResp_KhttpCloseHandler,
		(INT_NINETEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KPATTERN,
		"AT+KPATTERN=\"--EOF--Pattern--\"\r",
		INT_THIRTY_ONE,
		INT_TWO,
		mdmResp_KPatternHandler,
		(INT_THIRTY_ONE + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KCNXCFG,
		"AT+KCNXCFG=3, \"GPRS\",\"VZWINTERNET\"\r",
		INT_THIRTY_FIVE,
		INT_TWO,
		mdmResp_KcnxCfgHandler,
		(INT_THIRTY_FIVE + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KCNXTIMER,
		"AT+KCNXTIMER=3,60,2,70,60\r",
		INT_TWENTY_SIX,
		INT_TWO,
		mdmResp_KcnxTimerHandler,
		(INT_TWENTY_SIX + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_HEADER,
		kHttpHeaderString,
		INT_SEVENTEEN,
		INT_SEVEN,
		mdmResp_KhttpHeaderHandler,
		(INT_SEVENTEEN + INT_SEVEN + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_GET,
		kHttpGetCompleteData,
		INT_FIFTY_EIGHT,
		191,
		mdmResp_KhttpGetHandler,
		(INT_FIFTY_EIGHT +191 + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_TERMINATE_HEADER,
		"--EOF--Pattern--",
		INT_SIXTEEN,
		INT_TWO,
		mdmResp_KhttpGetHandler,
		(INT_SIXTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KCNX_DOWN,
		"AT+KCNXDOWN=3,1\r",
		INT_SIXTEEN,
		INT_TWO,
		defaultFunctionPointer,
		(INT_SIXTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_CGATT,
		"AT+CGATT=0\r",
		INT_ELEVEN,
		INT_TWO,
		defaultFunctionPointer,
		(INT_ELEVEN + INT_TWO + CRLF_CHAR_LEN)
	}

};


/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void getModemCommandData(AT_CMD_TYPE cmd, MODEM_CMD_DATA* cmdData)
{
	*cmdData = ModemCmdData[cmd];
}


/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmParser_SetKhttpHeaderString(uint8_t* sessionID)
{
	switch (*sessionID)
	{
		case 49:
		{
			kHttpHeaderString[15] = '1';
			kHttpGetString[12] = '1';
			DEBUG_PRINT("Session ID - 1");
		}
		break;

		case 50:
		{
			kHttpHeaderString[15] = '2';
			kHttpGetString[12] = '2';
			DEBUG_PRINT("Session ID - 2");
		}
		break;

		case 51:
		{
			kHttpHeaderString[15] = '3';
			kHttpGetString[12] = '3';
			DEBUG_PRINT("Session ID - 3");
		}
		break;

		case 52:
		{
			kHttpHeaderString[15] = '4';
			kHttpGetString[12] = '4';
			DEBUG_PRINT("Session ID - 4");
		}
		break;

		case 53:
		{
			kHttpHeaderString[15] = '5';
			kHttpGetString[12] = '5';
			DEBUG_PRINT("Session ID - 5");
		}
		break;

		case 54:
		{
			kHttpHeaderString[15] = '6';
			kHttpGetString[12] = '6';
			DEBUG_PRINT("Session ID - 6");
		}
		break;

		case 55:
		{
			kHttpHeaderString[15] = '7';
			kHttpGetString[12] = '7';
			DEBUG_PRINT("Session ID - 7");
		}
		break;

		case 56:
		{
			kHttpHeaderString[15] = '8';
			kHttpGetString[12] = '8';
			DEBUG_PRINT("Session ID - 8");
		}
		break;

		default:
		{
			DEBUG_PRINT("Session ID value exceeds the max value");
		}
		break;
	}

	DEBUG_PRINT("KHTTP HEADER String is ");

	strncpy(kHttpGetCompleteData,kHttpGetString,15);
	strncat(kHttpGetCompleteData,"\"?i=359998070228764&d=A1Y52XA2Y36&b=36&s=2\"\r",44);
}


