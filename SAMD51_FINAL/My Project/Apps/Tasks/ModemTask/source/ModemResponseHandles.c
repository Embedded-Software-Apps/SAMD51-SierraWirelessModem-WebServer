/*
 * ModemResponseHandles.c
 *
 * Created: 5/30/2018 11:58:30 PM
 *  Author: anilj
 */ 
#include "Apps/Tasks/ModemTask/include/ModemResponseHandles.h"
#include "Apps/SerialDebug/SerialDebug.h"
#include "Apps/Tasks/ModemTask/include/ModemCmdParser.h"

static uint8_t sessionID = 0;
bool headerResponseOk = false;
bool dataPacketSentOk = false;

static void SendEOFPattern(void);
/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_AtRespHandler(uint8_t* response, uint8_t length)
{
	SerialDebugPrint("In AT handler\r\n",15);
	SerialDebugPrint(response,length);
	SerialDebugPrint("\r\n",2);
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_IMEIRespHandler(uint8_t* response, uint8_t length)
{
	SerialDebugPrint("In CGSN handler\r\n",18);
	SerialDebugPrint(response,length);
	SerialDebugPrint("\r\n",2);
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KhttpCloseHandler(uint8_t* response, uint8_t length)
{
	//DEBUG_PRINT("In KHTTP CLOSE handler");

	if(response[0] == 'O')
	{
		SerialDebugPrint(response,length);
		DEBUG_PRINT("\r\nClosed an active connection");
	}
	else
	{
		DEBUG_PRINT("There is no connection established with this session ID...");
		DEBUG_PRINT("Checking for an active connection with next session ID...\n");
	}

	DEBUG_PRINT("\r\n");
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KPatternHandler(uint8_t* response, uint8_t length)
{
	DEBUG_PRINT("In KPATTERN handler");
	SerialDebugPrint(response,length);
	DEBUG_PRINT("\r\n");
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KcnxCfgHandler(uint8_t* response, uint8_t length)
{
	DEBUG_PRINT("In KCNXCFG handler");
	SerialDebugPrint(response,length);
	DEBUG_PRINT("\r\n");
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KcnxTimerHandler(uint8_t* response, uint8_t length)
{
	DEBUG_PRINT("In KCNXTIMER handler");
	SerialDebugPrint(response,length);
	DEBUG_PRINT("\r\n");
}


/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KhttpCfgHandler(uint8_t* response, uint8_t length)
{
	DEBUG_PRINT("In KHTTPCFG handler");
	SerialDebugPrint(response,length);
	DEBUG_PRINT("\r\n");

	sessionID = response[11];
	mdmParser_SetKhttpHeaderString(&sessionID);
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KhttpHeaderHandler(uint8_t* response, uint8_t length)
{
	DEBUG_PRINT("In KHTTP HEADER handler");
	SerialDebugPrint(response,length);
	DEBUG_PRINT("\r\n");

	if(0==memcmp(response,"CONNECT",7))
	{
		headerResponseOk = true;
		DEBUG_PRINT("Header Response Ok");
		mdmCtrlr_SendDataToModem("--EOF--Pattern--",16);
		
		//SendEOFPattern();
		delay_ms(1000);
		mdmCtrlr_FlushRxBuffer();
		dataPacketSentOk = true;
	}
	else
	{
		headerResponseOk = false;
		dataPacketSentOk = false;
		DEBUG_PRINT("Header Response Not Ok");
	}
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_KhttpGetHandler(uint8_t* response, uint8_t length)
{
	DEBUG_PRINT("In KHTTP GET handler");
	DEBUG_PRINT("RECIEVED RESPONSE FROM WEB SERVER");
	SerialDebugPrint(response,length);
	dataPacketSentOk = true;
	DEBUG_PRINT("\r\n\n");
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void mdmResp_TerminateHeaderHandler(uint8_t* response, uint8_t length)
{
	DEBUG_PRINT("In TERMINATE HEADER handler");
	SerialDebugPrint(response,length);
	DEBUG_PRINT("\r\n");
	dataPacketSentOk = true;
}


/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
uint8_t getSessionId(void)
{
	return sessionID;
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
bool getHeaderResponseOkStatus(void)
{
	return headerResponseOk;
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
bool getDataPacketOkStatus(void)
{
	return dataPacketSentOk;
}


/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
void setDataPacketOkStatus(bool status)
{
	dataPacketSentOk = status;
}

/*============================================================================
**
** Function Name:      mdmComms_GetModemResponse
**
** Description:        Gets the parsed modem response
**
**===========================================================================*/
static void SendEOFPattern(void)
{
	mdmParser_SendCommandToModem(CMD_AT_TERMINATE_HEADER);
	delay_ms(2000);
	mdmParser_ProcessModemResponse();
	delay_ms(2000);
}
