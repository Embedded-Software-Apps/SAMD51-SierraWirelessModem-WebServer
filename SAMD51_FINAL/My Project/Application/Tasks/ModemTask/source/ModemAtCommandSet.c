/*=======================================================================================
 * ModemAtCommandSet.c
 *
 * Implements the AT Command set and related functionalities.
 *======================================================================================*/

/****************************************************************************************
 INCLUDE FILES
*****************************************************************************************/
#include "Application/Common/Common.h"
#include "Application/Tasks/ModemTask/include/ModemAtCommandSet.h"
#include "Application/Tasks/ModemTask/include/ModemResponseHandles.h"
#include "Application/Tasks/ModemTask/include/ModemConnection.h"
#include "Application/Tasks/ModemTask/include/ModemConnectionConfig.h"

static uint8_t HttpHeaderString[INT_SEVENTEEN] = {'A','T','+','K','H','T','T','P','H','E','A','D','E','R','=','X','\r'};

static uint8_t kHttpGetString[INT_FIFTEEN] = {'A','T','+','K','H','T','T','P','G','E','T','=','0',',','\0'};

static uint8_t kHttpGetCompleteData[60] = {0};

static uint8_t kHttpAPNDefaultString[INT_THIRTY_FIVE] = {0};

static const uint8_t kHttp_VZWINTERNET_String[INT_THIRTY_FIVE] = "AT+KCNXCFG=3, \"GPRS\",\"VZWINTERNET\"\r";

static const uint8_t kHttp_ApnStringFirstPart[INT_TWENTY_TWO] = "AT+KCNXCFG=3, \"GPRS\",\"";
static const uint8_t kHttp_ApnStringSecondPart[INT_TWO] = "\"\r";

static uint8_t* GeneratedAPNString = NULL;


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
		61,
		modemResponseHandler,
		(INT_THIRTY_NINE + 61 + CRLF_CHAR_LEN)
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
		kHttpAPNDefaultString,
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
		HttpHeaderString,
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
	},

	{
		CMD_AT_CGDCONT,
		AT_CMD_SET_CONNECTION,
		"AT+CGDCONT\?\r",
		INT_TWELEVE,
		184,
		modemResponseHandler,
		(INT_TWELEVE + 184 + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_WMANTSEL,
		AT_CMD_SET_CONNECTION,
		"AT+WMANTSEL=1\r",
		INT_FOURTEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_FOURTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	/* FOTA Commands */

	{
		CMD_AT_WDSS_SET_APN,
		AT_CMD_SET_FOTA_APP,
		"AT+WDSS=0,\"VZWINTERNET\"\r",
		INT_TWENTY_FOUR,
		INT_TWO,
		modemResponseHandler,
		(INT_TWENTY_FOUR + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_WDSS_START_DM_SESSION,
		AT_CMD_SET_FOTA_APP,
		"AT+WDSS=1,1\r",
		INT_TWELEVE,
		INT_TWO,
		modemResponseHandler,
		(INT_TWELEVE + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_WDSI,
		AT_CMD_SET_FOTA_APP,
		"AT+WDSI=8191\r",
		INT_THIRTEEN,
		INT_TWO,
		modemResponseHandler,
		(INT_THIRTEEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_WDSC_FW_DOWNLOAD,
		AT_CMD_SET_FOTA_APP,
		"AT+WDSC=1,1\r",
		INT_TWELEVE,
		INT_TWO,
		modemResponseHandler,
		(INT_TWELEVE + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_WDSC_FW_INSTALL,
		AT_CMD_SET_FOTA_APP,
		"AT+WDSC=2,1\r",
		INT_TWELEVE,
		INT_TWO,
		modemResponseHandler,
		(INT_TWELEVE + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_WDSR_ACCEPT_FW_DOWNLOAD,
		AT_CMD_SET_FOTA_APP,
		"AT+WDSR=3\r",
		INT_TEN,
		INT_TWO,
		modemResponseHandler,
		(INT_TEN + INT_TWO + CRLF_CHAR_LEN)
	},

	{
		CMD_AT_WDSR_ACCEPT_FW_INSTALL,
		AT_CMD_SET_FOTA_APP,
		"AT+WDSR=4\r",
		INT_TEN,
		INT_TWO,
		modemResponseHandler,
		(INT_TEN + INT_TWO + CRLF_CHAR_LEN)
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
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
void buildHttpHeaderWithActiveSessionID(const uint8_t* activeSessionId)
{
	switch (*activeSessionId)
	{
		case SESSION_ID_1:
		{
			HttpHeaderString[SESSION_ID_POS_IN_HEADER] = '1';
			kHttpGetString[SESSION_ID_POS_IN_GET_REQ]  = '1';
		}
		break;

		case SESSION_ID_2:
		{
			HttpHeaderString[SESSION_ID_POS_IN_HEADER] = '2';
			kHttpGetString[SESSION_ID_POS_IN_GET_REQ]  = '2';
		}
		break;

		case SESSION_ID_3:
		{
			HttpHeaderString[SESSION_ID_POS_IN_HEADER] = '3';
			kHttpGetString[SESSION_ID_POS_IN_GET_REQ]  = '3';
		}
		break;

		case SESSION_ID_4:
		{
			HttpHeaderString[SESSION_ID_POS_IN_HEADER] = '4';
			kHttpGetString[SESSION_ID_POS_IN_GET_REQ]  = '4';
		}
		break;

		case SESSION_ID_5:
		{
			HttpHeaderString[SESSION_ID_POS_IN_HEADER] = '5';
			kHttpGetString[SESSION_ID_POS_IN_GET_REQ]  = '5';
		}
		break;

		case SESSION_ID_6:
		{
			HttpHeaderString[SESSION_ID_POS_IN_HEADER] = '6';
			kHttpGetString[SESSION_ID_POS_IN_GET_REQ]  = '6';
		}
		break;

		case SESSION_ID_7:
		{
			HttpHeaderString[SESSION_ID_POS_IN_HEADER] = '7';
			kHttpGetString[SESSION_ID_POS_IN_GET_REQ]  = '7';
		}
		break;

		case SESSION_ID_8:
		{
			HttpHeaderString[SESSION_ID_POS_IN_HEADER] = '8';
			kHttpGetString[SESSION_ID_POS_IN_GET_REQ]  = '8';
		}
		break;

		case SESSION_ID_9:
		{
			HttpHeaderString[SESSION_ID_POS_IN_HEADER] = '9';
			kHttpGetString[SESSION_ID_POS_IN_GET_REQ]  = '9';
		}
		break;

		default:
		{
			/* Session ID value exceeds. */
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
void buildDataPacketsToServer(void)
{
	strncpy(kHttpGetCompleteData,kHttpGetString,15);
	strncat(kHttpGetCompleteData,"\"?i=359998070228764&d=A1Y52XA2Y36&b=36&s=2\"\r",44);
}


///*============================================================================
//**
//** Function Name:      mdmCtrlr_FlushRxBuffer
//**
//** Description:        Flushes the Rx Ring Buffer
//**
//**===========================================================================*/
//void retrieveAPNStringFromResponse(uint8_t* cfgResponse)
//{
//	uint8_t SourceStartIndex = 116;
//	uint8_t SourceEndIndex = 126+1;
//	uint8_t DestStartIndex = 22;
//	uint8_t DestEndIndex = 32 + 1;
//	uint8_t apnString[11] = {0};
//
//	for(SourceStartIndex = 116; SourceStartIndex < SourceEndIndex; SourceStartIndex++)
//	{
//		kHttpAPNDefaultString[(DestStartIndex)+(SourceStartIndex-116)] = cfgResponse[SourceStartIndex];
//		apnString[SourceStartIndex - 116] = cfgResponse[SourceStartIndex];
//	}
//
//	SerialDebugPrint(apnString,11);
//	DEBUG_PRINT("\r\n");
//}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
void retrieveAPNStringFromResponse(uint8_t* cfgResponse)
{
	uint8_t SourceStartIndex = 0;
	uint8_t SourceStartIndexCopy = 0;
	bool parseStatus = false;
	uint8_t* pApnString = NULL;
	uint8_t apnStringLength = 0;
	uint8_t apnStringCopyIndex = 0;
	uint8_t apnIndex = 0;

	if(GeneratedAPNString != NULL)
	{
		vPortFree(GeneratedAPNString);
	}

	/* Finding the end of first line in APN response */
	while(cfgResponse[SourceStartIndex] != '\r')
	{
		SourceStartIndex++;
	}

	SourceStartIndex++;

	if(cfgResponse[SourceStartIndex] == '\n')
	{
		/* Jump to second line */
		SourceStartIndex = SourceStartIndex + 3;

		/* Finding the end of second line in APN response */
		while(cfgResponse[SourceStartIndex] != '\r')
		{
			SourceStartIndex++;
		}

		/* End of second line found */
		SourceStartIndex++;

		if(cfgResponse[SourceStartIndex] == '\n')
		{
			/* Jump to third line */
			SourceStartIndex = SourceStartIndex + 3;

			/*
			 * *** APN STRING IS EXTRACTED HERE ***
			 */

			/* Find the context 3 character */
			while(cfgResponse[SourceStartIndex] != '3')
			{
				SourceStartIndex++;
			}

			/*
			 * Identified the context 3.
			 * Move to the starting position of APN string.
			 */
			SourceStartIndex = SourceStartIndex + 8;
			SourceStartIndexCopy = SourceStartIndex;

			/* Find the length of APN string */
			while(cfgResponse[SourceStartIndexCopy] != '"')
			{
				SourceStartIndexCopy++;
				apnStringLength++;
			}

			SourceStartIndexCopy = 0;

			/* Allocate memory for copying the APN string */
			pApnString = (uint8_t*)pvPortMalloc((apnStringLength)*(sizeof(uint8_t)));

			if(pApnString != NULL)
			{
				/* Copy the APN string */
				while(cfgResponse[SourceStartIndex] != '"')
				{
					pApnString[apnStringCopyIndex] = cfgResponse[SourceStartIndex];
					SourceStartIndex++;
					apnStringCopyIndex++;
				}

				if(apnStringCopyIndex != apnStringLength)
				{
					DEBUG_PRINT("APN String copy failed\r\n");
				}


				/* Copy the the retrieved APN string */
				if(memcmp(pApnString,"VZWINTERNET",11) == STRING_EQUAL)
				{
					DEBUG_PRINT("Retrieved APN - ");
					SerialDebugPrint(pApnString,11);
					clearAPNString();
					memcpy(kHttpAPNDefaultString,kHttp_VZWINTERNET_String,35);
				}
				else
				{
					DEBUG_PRINT("Retrieved APN - ");
					SerialDebugPrint(pApnString,apnStringLength);

					clearAPNString();
					apnStringCopyIndex = 0;
					apnIndex = 0;

					/* Copy the const part */
					for(apnIndex = 0;apnIndex<22;apnIndex++)
					{
						kHttpAPNDefaultString[apnIndex] = kHttp_ApnStringFirstPart[apnIndex];
					}

					/* Copy the APN string */
					strncat(kHttpAPNDefaultString,pApnString,apnStringLength);

					/* Copy the remaining part */
					strncat(kHttpAPNDefaultString,kHttp_ApnStringSecondPart,2);

					/* Deallocate APN string */
					vPortFree(pApnString);
				}
			}
			else
			{
				DEBUG_PRINT("Dynamic Allocation Failed\r\n");
			}

		}
		else
		{
			DEBUG_PRINT("APN string parsing failed in second line.\r\n");
			parseStatus = false;
		}


	}
	else
	{
		DEBUG_PRINT("APN string parsing failed in first line.\r\n");
		parseStatus = false;
	}





	DEBUG_PRINT("\r\n");
}


/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
void clearAPNString(void)
{
	uint8_t index = 0;

	for(index = 0; index < 35; index++)
	{
		kHttpAPNDefaultString[index] = 0;
	}
}
