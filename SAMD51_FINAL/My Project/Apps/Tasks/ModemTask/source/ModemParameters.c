/*
 * ModemParameters.c
 *
 * Created: 5/22/2018 7:39:23 PM
 *  Author: anilj
 */ 
#include "Apps/Tasks/ModemTask/include/ModemCmdParser.h"
#include "Apps/Tasks/ModemTask/include/ModemParameters.h"
#include "Apps/Tasks/ModemTask/include/ModemResponseHandles.h"
#include "Apps/SerialDebug/SerialDebug.h"

#define RESPONSE_BUFFER_SIZE (40)

uint8_t connectResponse[620];


static CONNECT_DATA connectData[6] = \
{
	{AT_KPATTERN,"AT+KPATTERN=\"--EOF--Pattern--\"\r\n",36,36},
	{AT_KCNXCFG,"AT+KCNXCFG=3, \"GPRS\",\"VZWINTERNET\"\r\n",42,40},
	{AT_KCNXTIMER,"AT+KCNXTIMER=3,60,2,70,60\r\n",27,31},
	{AT_KHTTPCFG,"AT+KHTTPCFG=3,\"ingest1.response.cloud\"\r\n",40,79},
	{AT_KHTTPHEADER,"AT+KHTTPHEADER=2\r\n",18,27},
	{AT_KHTTPGET,"AT+KHTTPGET=2,\"?i=359998070228764&d=A1Y52XA2Y36&b=42&s=2\"\r\n",59,620},
}; 

static void sendAT_KPATTERN(void);
static void sendAT_KCNXCFG(void);
static void sendAT_KCNXTIMER(void);
static void sendAT_KHTTPCFG(void);
static void sendAT_KHTTPHEADER(void);
static void closeExistingConnections(void);
/*============================================================================
**
** Function Name:      MdmParam_CustomizeGeneralSettings
**
** Description:        Customizes the default modem settings.
**
**===========================================================================*/
bool MdmParam_CustomizeGeneralSettings(void)
{
	uint8_t responseBuffer[RESPONSE_BUFFER_SIZE];
	uint8_t respLength;
	
	SendATCommandToModem(CMD_AT_ATE);
	delay_ms(500);
			
	memset(responseBuffer,0,RESPONSE_BUFFER_SIZE);
	mdmParser_GetModemResponse(CMD_AT_ATE,&responseBuffer,&respLength);
	
}

/*============================================================================
**
** Function Name:      MdmParam_ModemPowerOnSelfTest
**
** Description:        Performs the basic power on self test for the modem.
**
**===========================================================================*/
	
bool MdmParam_TestModemBaseParameters(void)
{
	static TEST_MODEM_BASE_PARAMS testMdmBasicParam = TEST_UART_BAUDARTE;
	uint8_t responseBuffer[RESPONSE_BUFFER_SIZE];
	uint8_t respLength;
	int8_t debugPrintBuff[50];
	
	switch(testMdmBasicParam)
	{
		case TEST_UART_COMM:
		{
			SendATCommandToModem(CMD_AT);
			delay_ms(500);
			
			memset(responseBuffer,0,RESPONSE_BUFFER_SIZE);
			mdmParser_GetModemResponse(CMD_AT,&responseBuffer,&respLength);
			
			if(memcmp(&responseBuffer,"OK",respLength) == 0)
			{
				sprintf(debugPrintBuff,"Verified the UART Communication with Modem.\r\n");
				SerialDebugPrint(debugPrintBuff,sizeof(debugPrintBuff));
			}
			else
			{
				sprintf(debugPrintBuff,"Not able to verify the UART Communication with Modem.\r\n");
				SerialDebugPrint(debugPrintBuff,sizeof(debugPrintBuff));				
			}
			
			testMdmBasicParam = TEST_NETWORK_REG_STATUS;
			delay_ms(1000);
		}
		break;
		
		case TEST_NETWORK_REG_STATUS:
		{
			SendATCommandToModem(CMD_AT_CGREG);
			delay_ms(500);
			
			memset(responseBuffer,0,RESPONSE_BUFFER_SIZE);
			mdmParser_GetModemResponse(CMD_AT_CGREG,&responseBuffer,&respLength);
			
			if(memcmp(&responseBuffer,"READY",respLength) == 0)
			{
				sprintf(debugPrintBuff,"Verified the Network Registration.\r\n");
				SerialDebugPrint(debugPrintBuff,sizeof(debugPrintBuff));
			}
			else
			{
				sprintf(debugPrintBuff,"Not able to verify the Network Registration.\r\n");
				SerialDebugPrint(debugPrintBuff,sizeof(debugPrintBuff));
			}
			
			testMdmBasicParam = TEST_UART_BAUDARTE;
			delay_ms(1000);
		}
		break;
	
	} 
	
}



























void mdmParam_InitiateConnection(void)
{
	closeExistingConnections();
	sendAT_KPATTERN();
	sendAT_KCNXCFG();
	sendAT_KCNXTIMER();
	sendAT_KHTTPCFG();
	sendAT_KHTTPHEADER();
	mdmCtrlr_FlushRxBuffer();
}


static void closeExistingConnections(void)
{
	mdmParser_SendCommandToModem(CMD_AT_KHTTP_CLOSE_1);
	delay_ms(1000);
	mdmParser_ProcessModemResponse();
	delay_ms(1000);

	mdmParser_SendCommandToModem(CMD_AT_KHTTP_CLOSE_2);
	delay_ms(1000);
	mdmParser_ProcessModemResponse();
	delay_ms(1000);

	mdmParser_SendCommandToModem(CMD_AT_KHTTP_CLOSE_3);
	delay_ms(1000);
	mdmParser_ProcessModemResponse();
	delay_ms(1000);

	mdmParser_SendCommandToModem(CMD_AT_KHTTP_CLOSE_4);
	delay_ms(1000);
	mdmParser_ProcessModemResponse();
	delay_ms(1000);

	mdmParser_SendCommandToModem(CMD_AT_KHTTP_CLOSE_5);
	delay_ms(1000);
	mdmParser_ProcessModemResponse();
	delay_ms(1000);

	mdmParser_SendCommandToModem(CMD_AT_KHTTP_CLOSE_6);
	delay_ms(1000);
	mdmParser_ProcessModemResponse();
	delay_ms(1000);

	mdmParser_SendCommandToModem(CMD_AT_KHTTP_CLOSE_7);
	delay_ms(1000);
	mdmParser_ProcessModemResponse();
	delay_ms(1000);

	mdmParser_SendCommandToModem(CMD_AT_KHTTP_CLOSE_8);
	delay_ms(1000);
	mdmParser_ProcessModemResponse();
	delay_ms(1000);

	mdmParser_SendCommandToModem(CMD_AT_KHTTP_CLOSE_9);
	delay_ms(1000);
	mdmParser_ProcessModemResponse();
	delay_ms(1000);

	mdmParser_SendCommandToModem(CMD_AT_KHTTP_CLOSE_10);
	delay_ms(1000);
	mdmParser_ProcessModemResponse();
	delay_ms(1000);
}


void sendPacketToServer(void)
{
	if (getHeaderResponseOkStatus() == true)
	{
		mdmParser_SendCommandToModem(CMD_AT_KHTTP_GET);
		delay_ms(9000);
		setDataPacketOkStatus(false);
		DEBUG_PRINT("Posted the data to server...Waiting for Web Server Response");
		mdmParser_ProcessModemResponse();
		delay_ms(2000);
	}
	else
	{
		//DEBUG_PRINT("No Response from Web Sever. Posting data to sever is failed");
		delay_ms(10000);
	}
	
}

static void sendAT_KPATTERN(void)
{
	mdmParser_SendCommandToModem(CMD_AT_KPATTERN);
	delay_ms(2000);
	mdmParser_ProcessModemResponse();
	delay_ms(2000);
}

static void sendAT_KCNXCFG(void)
{
	mdmParser_SendCommandToModem(CMD_AT_KCNXCFG);
	delay_ms(2000);
	mdmParser_ProcessModemResponse();
	delay_ms(2000);
}

static void sendAT_KCNXTIMER(void)
{
	mdmParser_SendCommandToModem(CMD_AT_KCNXTIMER);
	delay_ms(2000);
	mdmParser_ProcessModemResponse();
	delay_ms(2000);
}

static void sendAT_KHTTPCFG(void)
{
	mdmParser_SendCommandToModem(CMD_AT_KHTTP_CFG);
	delay_ms(2000);
	mdmParser_ProcessModemResponse();
	delay_ms(2000);
}

static void sendAT_KHTTPHEADER(void)
{
	mdmParser_SendCommandToModem(CMD_AT_KHTTP_HEADER);
	delay_ms(2000);
	mdmParser_ProcessModemResponse();
	delay_ms(2000);
}


