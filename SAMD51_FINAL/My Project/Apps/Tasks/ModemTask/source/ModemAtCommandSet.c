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
		AT_CMD_SET_MAX,
		"",
		INT_ZERO,
		INT_ZERO,
		modemResponseHandler,
		INT_ZERO
	},

	/* AT */
	{
		CMD_AT,
		AT_CMD_SET_DIAGNOSTICS,
		"AT\r",
		INT_THREE,
		INT_TWO,
		modemResponseHandler,
		(INT_THREE + INT_TWO + CRLF_CHAR_LEN)
	},

	/* AT + CGSN*/
	{
		CMD_AT_CGSN,
		AT_CMD_SET_DIAGNOSTICS,
		"AT+CGSN\r",
		INT_EIGHT,
		INT_FIFTEEN,
		modemResponseHandler,
		(INT_EIGHT + INT_FIFTEEN + CRLF_CHAR_LEN)
	},

	/* AT+WCARRIER */
	{
		CMD_AT_WCARRIER,
		AT_CMD_SET_DIAGNOSTICS,
		"AT+WCARRIER\r",
		INT_TWELEVE,
		INT_FOURTEEN,
		modemResponseHandler,
		(INT_TWELEVE + INT_FOURTEEN + CRLF_CHAR_LEN)
	},

	/* AT+IPR */
	{
		CMD_AT_IPR,
		AT_CMD_SET_DIAGNOSTICS,
		"AT+IPR?\r",
		INT_EIGHT,
		INT_TWELEVE,
		modemResponseHandler,
		(INT_EIGHT + INT_TWELEVE + CRLF_CHAR_LEN)
	},

	/* AT+CPIN */
	{
		CMD_AT_CPIN,
		AT_CMD_SET_DIAGNOSTICS,
		"AT+CPIN?\r",
		INT_FIVE,
		INT_EIGHT,
		modemResponseHandler,
		(INT_FIVE + INT_EIGHT + CRLF_CHAR_LEN)
	},

	/* AT+CGREG */
	{
		CMD_AT_CGREG,
		AT_CMD_SET_DIAGNOSTICS,
		"AT+CGREG?\r",
		INT_TEN,
		INT_ELEVEN,
		modemResponseHandler,
		(INT_TEN + INT_ELEVEN + CRLF_CHAR_LEN)
	},

	/* AT+KGSN */
	{
		CMD_AT_KGSN,
		AT_CMD_SET_DIAGNOSTICS,
		"AT+KGSN=3\r",
		INT_TEN,
		INT_TWENTY_ONE,
		modemResponseHandler,
		(INT_TEN + INT_TWENTY_ONE + CRLF_CHAR_LEN)
	},

	/* ATE0 */
	{
		CMD_AT_ATE,
		AT_CMD_SET_DIAGNOSTICS,
		"ATE1\r",
		INT_FIVE,
		INT_TWO,
		modemResponseHandler,
		(INT_FIVE + INT_TWO + CRLF_CHAR_LEN)
	},

	/* Connection Initialization Commands */
	{
		CMD_AT_KHTTP_CFG,
		AT_CMD_SET_CONNECTION,
		"AT+KHTTPCFG=3,\"ingest1.response.cloud\"\r",
		INT_THIRTY_NINE,
		INT_TWELEVE,
		modemResponseHandler,
		(INT_THIRTY_NINE + INT_TWELEVE + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_1,
		AT_CMD_SET_CONNECTION,
		"AT+KHTTPCLOSE=1,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_2,
		AT_CMD_SET_CONNECTION,
		"AT+KHTTPCLOSE=2,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_3,
		AT_CMD_SET_CONNECTION,
		"AT+KHTTPCLOSE=3,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_4,
		AT_CMD_SET_CONNECTION,
		"AT+KHTTPCLOSE=4,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_5,
		AT_CMD_SET_CONNECTION,
		"AT+KHTTPCLOSE=5,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_6,
		AT_CMD_SET_CONNECTION,
		"AT+KHTTPCLOSE=6,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_7,
		AT_CMD_SET_CONNECTION,
		"AT+KHTTPCLOSE=7,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_8,
		AT_CMD_SET_CONNECTION,
		"AT+KHTTPCLOSE=8,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_9,
		AT_CMD_SET_CONNECTION,
		"AT+KHTTPCLOSE=9,0\r",
		INT_EIGHTEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_EIGHTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_CLOSE_10,
		AT_CMD_SET_CONNECTION,
		"AT+KHTTPCLOSE=10,0\r",
		INT_NINETEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_NINETEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KPATTERN,
		AT_CMD_SET_CONNECTION,
		"AT+KPATTERN=\"--EOF--Pattern--\"\r",
		INT_THIRTY_ONE,
		INT_TWO,
		modemResponseHandler,
		(INT_THIRTY_ONE + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KCNXCFG,
		AT_CMD_SET_CONNECTION,
		"AT+KCNXCFG=3, \"GPRS\",\"VZWINTERNET\"\r",
		INT_THIRTY_FIVE,
		INT_TWO,
		modemResponseHandler,
		(INT_THIRTY_FIVE + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KCNXTIMER,
		AT_CMD_SET_CONNECTION,
		"AT+KCNXTIMER=3,60,2,70,60\r",
		INT_TWENTY_SIX,
		INT_TWO,
		modemResponseHandler,
		(INT_TWENTY_SIX + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_HEADER,
		AT_CMD_SET_CONNECTION,
		kHttpHeaderString,
		INT_SEVENTEEN,
		INT_SEVEN,
		modemResponseHandler,
		(INT_SEVENTEEN + INT_SEVEN + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KHTTP_GET,
		AT_CMD_SET_CONNECTION,
		kHttpGetCompleteData,
		INT_FIFTY_EIGHT,
		191,
		modemResponseHandler,
		(INT_FIFTY_EIGHT +191 + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_TERMINATE_HEADER,
		AT_CMD_SET_CONNECTION,
		"--EOF--Pattern--",
		INT_SIXTEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_SIXTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_KCNX_DOWN,
		AT_CMD_SET_CONNECTION,
		"AT+KCNXDOWN=3,1\r",
		INT_SIXTEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_SIXTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_CGATT,
		AT_CMD_SET_CONNECTION,
		"AT+CGATT=0\r",
		INT_ELEVEN,
		INT_TWO,
		modemResponseHandler,
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


