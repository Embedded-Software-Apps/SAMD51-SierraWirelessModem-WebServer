/*
 * ModemResponseHandles.c
 *
 * Created: 5/30/2018 11:58:30 PM
 *  Author: anilj
 */ 
#include "Apps/Tasks/ModemTask/include/ModemResponseHandles.h"
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
