/*
 * FotaAppTask.c
 *
 * Created: 8/7/2018 8:59:32 PM
 *  Author: anilj
 */ 

#include <hal_delay.h>
#include <stdio.h>
#include <string.h>
#include "Application/SerialDebug/SerialDebug.h"
#include "Application/Common/Common.h"
#include "Application/Tasks/ModemTask/include/ModemPowerControl.h"
#include "Application/Tasks/ModemTask/include/ModemAtCommandSet.h"
#include "Application/Tasks/FotaAppTask/include/FotaAppTask.h"

#define FOTA_FWDL_CHECK_TIMER_LOAD_VALUE pdMS_TO_TICKS(86400000)
#define SERVICE_INDICATION_RESPONSE_LENGTH (12)
#define SERVICE_INDICATION_CMD_LENGTH (9)

static CmdResponseType FotaCommandResponse;
static FOTA_MAIN_STATES_T FotaMainState;
static FOTA_APP_OPERATIONAL_STATE_T FotaOperationalMode;
static USER_AGREEMENT_ACTIVATION_STATES_T FotaUserAgreementActivationState;
static bool bFotaVerificationIsDone;

static void FotaFwDownloadCheckTImerCallBack(void* param);
static void FotaAppInit(void);
static bool initializeFotaFirmwareDownloadCheckTimer(void);
static bool validateCommonCommandResponse(uint8_t* response);
static void FotaAppSchedule(void);
static DEVICE_SERVICE_INDICATION_TYPE getDeviceServiceIndicationType(void);
/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: Gets the current Modem Power State.
*
********************************************************************************/
static void FotaAppInit(void)
{
/*    if(false == initializeFotaFirmwareDownloadCheckTimer())
    {
        DEBUG_PRINT("Error: FOTA timer not started. Rebooting the system...");
        requestWatchDogForcedReset();
    }
    else
    {
        FotaMainState = SET_APN_TO_C0NNECT_WITH_AIRVANTAGE;
        FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
        FotaUserAgreementActivationState = ACTIVATE_USER_AGREEMENT_FOR_FW_DOWNLOAD;
        bFotaVerificationIsDone = false;
    }*/

    FotaMainState = SET_APN_TO_C0NNECT_WITH_AIRVANTAGE;
    FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
    FotaUserAgreementActivationState = ACTIVATE_USER_AGREEMENT_FOR_FW_DOWNLOAD;
    bFotaVerificationIsDone = false;
}

/*******************************************************************************
*
* NAME       : FotaAppTask
*
* DESCRIPTION: Gets the current Modem Power State.
*
********************************************************************************/
void FotaAppTask( void *FotaTaskParam)
{
    const TickType_t xDelayMs = pdMS_TO_TICKS(600UL);
    const TickType_t xDebugPrintDelayMs = pdMS_TO_TICKS(500UL);

    FotaAppInit();

    while(1)
    {
        if((getModemPowerStatus() == MDM_PWR_OPERATIONAL_READY_FOR_AT_CMDS) &&
           (false == isFotaVerificationDone()))
        {
            FotaAppSchedule();
            kickWatchDog();
            vTaskDelay(xDelayMs);
        }
    }
}

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: This function converts a given signed integer(16-bit or 32-bit)
*               into a string and returns the string.
*
********************************************************************************/
static bool initializeFotaFirmwareDownloadCheckTimer(void)
{
    TimerHandle_t xAutoReloadFwDlCheckTimer;
    bool status = false;

    xAutoReloadFwDlCheckTimer   = xTimerCreate("FirmwareDownloadCheckTimer",FOTA_FWDL_CHECK_TIMER_LOAD_VALUE,pdTRUE,0,FotaFwDownloadCheckTImerCallBack);

    if(xAutoReloadFwDlCheckTimer != NULL)
    {
        if(pdPASS == xTimerStart(xAutoReloadFwDlCheckTimer,0))
        {
            DEBUG_PRINT("FOTA Timer started");
            status = true;
        }
    }

    return status;
}

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: This function converts a given signed integer(16-bit or 32-bit)
*               into a string and returns the string.
*
********************************************************************************/
static void FotaFwDownloadCheckTImerCallBack(void* param)
{

}

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: Gets the current Modem Power State.
*
********************************************************************************/
static void FotaAppSchedule(void)
{
    const TickType_t QueuePushDelayMs = pdMS_TO_TICKS(500UL);
    const TickType_t TransmitDelayMs = pdMS_TO_TICKS(500UL);
    const TickType_t ResponseWaitDelayMs = pdMS_TO_TICKS(4000UL);
    const TickType_t startupDelayMs = pdMS_TO_TICKS(6000UL);
    AtTxMsgType TxMsgQueueData;
    BaseType_t TxQueuePushStatus;
    DEVICE_SERVICE_INDICATION_TYPE serviceIndicationReceived =\
    		                 SERVICE_INDICATION_ERROR;

    switch(FotaMainState)
    {
        case SET_APN_TO_C0NNECT_WITH_AIRVANTAGE:
        {
            if(FotaOperationalMode == FOTA_APP_OPERATIONAL_TX_MODE)
            {
                vTaskDelay(startupDelayMs);

                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = FOTA_APP_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_WDSS_SET_APN;
                        TxMsgQueueData.pData = NULL;
                        TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                        if(TxQueuePushStatus == pdPASS)
                        {
                            xSemaphoreGive(AtTxQueueLoadSemaphore);
                            FotaOperationalMode = FOTA_APP_OPERATIONAL_RX_MODE;
                            vTaskDelay(TransmitDelayMs);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to sent the set APN request to Tx Task");
                            vTaskDelay(TransmitDelayMs);
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                    }
                }
                else
                {
                    DEBUG_PRINT("Transmit Queue is not empty");
                }
            }
            else if(FotaOperationalMode == FOTA_APP_OPERATIONAL_RX_MODE)
            {
                if(pdPASS == xQueueReceive( FotaResponseQueue, &FotaCommandResponse, ResponseWaitDelayMs))
                {
                    if(FotaCommandResponse.atCmd == CMD_AT_WDSS_SET_APN)
                    {
                        if(false != validateCommonCommandResponse(FotaCommandResponse.response))
                        {
                            DEBUG_PRINT("FOTA : Set the APN for connecting to Air Vantage");
                            SerialDebugPrint(FotaCommandResponse.response,FotaCommandResponse.length);
                            DEBUG_PRINT("\r\n");
                            FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                            FotaMainState = ACTIVATE_DEVICE_SERVICE_INDICATIONS;
                        }
                        else
                        {
                            DEBUG_PRINT("Error:Expected Response Not Received...");
                            DEBUG_PRINT("\r\n");
                            FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                        }
                        vPortFree(FotaCommandResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Error:Failed to receive connection response in RX mode");
                        FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                        vPortFree(FotaCommandResponse.response);
                    }
                }
                else
                {
                    /* Wait until there is a response received from Rx Task */
                }
            }
            else
            {
                /* This part will never execute */
            }
        }
        break;

        case ACTIVATE_DEVICE_SERVICE_INDICATIONS:
        {
            if(FotaOperationalMode == FOTA_APP_OPERATIONAL_TX_MODE)
            {
                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = FOTA_APP_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_WDSI;
                        TxMsgQueueData.pData = NULL;
                        TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                        if(TxQueuePushStatus == pdPASS)
                        {
                            xSemaphoreGive(AtTxQueueLoadSemaphore);
                            FotaOperationalMode = FOTA_APP_OPERATIONAL_RX_MODE;
                            vTaskDelay(TransmitDelayMs);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to sent the service indication request to Tx Task");
                            vTaskDelay(TransmitDelayMs);
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                    }
                }
                else
                {
                    DEBUG_PRINT("Transmit Queue is not empty");
                }
            }
            else if(FotaOperationalMode == FOTA_APP_OPERATIONAL_RX_MODE)
            {
                if(pdPASS == xQueueReceive( FotaResponseQueue, &FotaCommandResponse, ResponseWaitDelayMs))
                {
                    if(FotaCommandResponse.atCmd == CMD_AT_WDSI)
                    {
                        if(false != validateCommonCommandResponse(FotaCommandResponse.response))
                        {
                            DEBUG_PRINT("FOTA : Set device service indications.");
                            SerialDebugPrint(FotaCommandResponse.response,FotaCommandResponse.length);
                            DEBUG_PRINT("\r\n");
                            FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                            FotaMainState = ACTIVATE_USER_AGREEMENTS_WITH_HOST;
                            FotaUserAgreementActivationState = ACTIVATE_USER_AGREEMENT_FOR_FW_DOWNLOAD;
                        }
                        else
                        {
                            DEBUG_PRINT("Error:Expected Response Not Received...");
                            DEBUG_PRINT("\r\n");
                            FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                        }
                        vPortFree(FotaCommandResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Error:Failed to receive connection response in RX mode");
                        FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                        vPortFree(FotaCommandResponse.response);
                    }
                }
                else
                {
                    /* Wait untill there is a response received from Rx Task */
                }
            }
            else
            {
                /* This part will never execute */
            }
        }
        break;

        case ACTIVATE_USER_AGREEMENTS_WITH_HOST:
        {
            switch(FotaUserAgreementActivationState)
            {
                case ACTIVATE_USER_AGREEMENT_FOR_FW_DOWNLOAD:
                {
                    if(FotaOperationalMode == FOTA_APP_OPERATIONAL_TX_MODE)
                    {
                        if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                        {
                            if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                            {
                                TxMsgQueueData.taskID = FOTA_APP_TASK;
                                TxMsgQueueData.atCmd = CMD_AT_WDSC_FW_DOWNLOAD;
                                TxMsgQueueData.pData = NULL;
                                TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                                if(TxQueuePushStatus == pdPASS)
                                {
                                    xSemaphoreGive(AtTxQueueLoadSemaphore);
                                    FotaOperationalMode = FOTA_APP_OPERATIONAL_RX_MODE;
                                    FotaMainState = ACTIVATE_USER_AGREEMENTS_WITH_HOST;
                                    FotaUserAgreementActivationState = ACTIVATE_USER_AGREEMENT_FOR_FW_DOWNLOAD;
                                    vTaskDelay(TransmitDelayMs);
                                }
                                else
                                {
                                    DEBUG_PRINT("Failed to sent the user agreement activation request for FW download to Tx Task");
                                    vTaskDelay(TransmitDelayMs);
                                }
                            }
                            else
                            {
                                DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                            }
                        }
                        else
                        {
                            DEBUG_PRINT("Transmit Queue is not empty");
                        }
                    }
                    else if(FotaOperationalMode == FOTA_APP_OPERATIONAL_RX_MODE)
                    {
                        if(pdPASS == xQueueReceive( FotaResponseQueue, &FotaCommandResponse, ResponseWaitDelayMs))
                        {
                            if(FotaCommandResponse.atCmd == CMD_AT_WDSC_FW_DOWNLOAD)
                            {
                                if(false != validateCommonCommandResponse(FotaCommandResponse.response))
                                {
                                    DEBUG_PRINT("FOTA : Set user agreement activation for FW download.");
                                    SerialDebugPrint(FotaCommandResponse.response,FotaCommandResponse.length);
                                    DEBUG_PRINT("\r\n");
                                    FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                                    FotaMainState = ACTIVATE_USER_AGREEMENTS_WITH_HOST;
                                    FotaUserAgreementActivationState = ACTIVATE_USER_AGREEMENT_FOR_FW_INSTALLATION;
                                }
                                else
                                {
                                    DEBUG_PRINT("Error:Expected Response Not Received...");
                                    DEBUG_PRINT("\r\n");
                                    FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                                }
                                vPortFree(FotaCommandResponse.response);
                            }
                            else
                            {
                                DEBUG_PRINT("Error:Failed to receive connection response in RX mode");
                                FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                                vPortFree(FotaCommandResponse.response);
                            }
                        }
                        else
                        {
                            /* Wait untill there is a response received from Rx Task */
                        }
                    }
                    else
                    {
                        /* This part will never execute */
                    }

                }
                break;

                case ACTIVATE_USER_AGREEMENT_FOR_FW_INSTALLATION:
                {
                    if(FotaOperationalMode == FOTA_APP_OPERATIONAL_TX_MODE)
                    {
                        if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                        {
                            if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                            {
                                TxMsgQueueData.taskID = FOTA_APP_TASK;
                                TxMsgQueueData.atCmd = CMD_AT_WDSC_FW_INSTALL;
                                TxMsgQueueData.pData = NULL;
                                TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                                if(TxQueuePushStatus == pdPASS)
                                {
                                    xSemaphoreGive(AtTxQueueLoadSemaphore);
                                    FotaOperationalMode = FOTA_APP_OPERATIONAL_RX_MODE;
                                    FotaMainState = ACTIVATE_USER_AGREEMENTS_WITH_HOST;
                                    FotaUserAgreementActivationState = ACTIVATE_USER_AGREEMENT_FOR_FW_INSTALLATION;
                                    vTaskDelay(TransmitDelayMs);
                                }
                                else
                                {
                                    DEBUG_PRINT("Failed to sent the user agreement activation request for FW installation to Tx Task");
                                    vTaskDelay(TransmitDelayMs);
                                }
                            }
                            else
                            {
                                DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                            }
                        }
                        else
                        {
                            DEBUG_PRINT("Transmit Queue is not empty");
                        }
                    }
                    else if(FotaOperationalMode == FOTA_APP_OPERATIONAL_RX_MODE)
                    {
                        if(pdPASS == xQueueReceive( FotaResponseQueue, &FotaCommandResponse, ResponseWaitDelayMs))
                        {
                            if(FotaCommandResponse.atCmd == CMD_AT_WDSC_FW_INSTALL)
                            {
                                if(false != validateCommonCommandResponse(FotaCommandResponse.response))
                                {
                                    DEBUG_PRINT("FOTA : Set user agreement activation for FW installation.");
                                    SerialDebugPrint(FotaCommandResponse.response,FotaCommandResponse.length);
                                    DEBUG_PRINT("\r\n");
                                    FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                                    FotaMainState = START_DM_SESSION_WITH_AIR_VANTAGE;
                                    FotaUserAgreementActivationState = ACTIVATE_USER_AGREEMENT_FOR_FW_DOWNLOAD;
                                }
                                else
                                {
                                    DEBUG_PRINT("Error:Expected Response Not Received...");
                                    DEBUG_PRINT("\r\n");
                                    FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                                }
                                vPortFree(FotaCommandResponse.response);
                            }
                            else
                            {
                                DEBUG_PRINT("Error:Failed to receive connection response in RX mode");
                                FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                                vPortFree(FotaCommandResponse.response);
                            }
                        }
                        else
                        {
                            /* Wait until there is a response received from Rx Task */
                        }
                    }
                    else
                    {
                        /* This part will never execute */
                    }
                }
                break;

                default:
                break;
            }

        }
        break;

        case START_DM_SESSION_WITH_AIR_VANTAGE:
        {
            if(FotaOperationalMode == FOTA_APP_OPERATIONAL_TX_MODE)
            {
                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = FOTA_APP_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_WDSS_START_DM_SESSION;
                        TxMsgQueueData.pData = NULL;
                        TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                        if(TxQueuePushStatus == pdPASS)
                        {
                            xSemaphoreGive(AtTxQueueLoadSemaphore);
                            FotaOperationalMode = FOTA_APP_OPERATIONAL_RX_MODE;
                            vTaskDelay(TransmitDelayMs);
                        }
                        else
                        {
                            DEBUG_PRINT("Failed to sent the start DM session request to Tx Task");
                            vTaskDelay(TransmitDelayMs);
                        }
                    }
                    else
                    {
                        DEBUG_PRINT("Error : Not able to obtain Tx Semapahore");
                    }
                }
                else
                {
                    DEBUG_PRINT("Transmit Queue is not empty");
                }
            }
            else if(FotaOperationalMode == FOTA_APP_OPERATIONAL_RX_MODE)
            {
                if(pdPASS == xQueueReceive( FotaResponseQueue, &FotaCommandResponse, ResponseWaitDelayMs))
                {
                    if(FotaCommandResponse.atCmd == CMD_AT_WDSS_START_DM_SESSION)
                    {
                        if(false != validateCommonCommandResponse(FotaCommandResponse.response))
                        {
                            DEBUG_PRINT("FOTA : Initiating the DM session with Air Vantage.");
                            SerialDebugPrint(FotaCommandResponse.response,FotaCommandResponse.length);
                            DEBUG_PRINT("\r\n");
                            FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                            FotaMainState = SYSTEM_IS_IN_FIRMWARE_DOWNLOAD_MODE;
                        }
                        else
                        {
                            DEBUG_PRINT("Error:Expected Response Not Received...");
                            DEBUG_PRINT("\r\n");
                            FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                        }
                        vPortFree(FotaCommandResponse.response);
                    }
                    else
                    {
                        DEBUG_PRINT("Error:Failed to receive connection response in RX mode");
                        FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                        vPortFree(FotaCommandResponse.response);
                    }
                }
                else
                {
                    /* Wait untill there is a response received from Rx Task */
                }
            }
            else
            {
                /* This part will never execute */
            }
        }
        break;

        case SYSTEM_IS_IN_FIRMWARE_DOWNLOAD_MODE:
        {
        	serviceIndicationReceived = getDeviceServiceIndicationType();

        	if(serviceIndicationReceived != SERVICE_INDICATION_ERROR)
        	{
            	switch(serviceIndicationReceived)
            	{
            		case APN_SET_READY_FOR_DM_SESSION:
            		{
            			DEBUG_PRINT("FOTA : APN IS SET. READY FOR DM SESSION.\r\n");
            		}
            		break;

            		case AIRVANTAGE_ORIGINATED_DM_SESSION_REQUEST:
            		{
            			DEBUG_PRINT("FOTA : AIRVANTAGE ORIGINATED DM SESSION REQUEST.\r\n");
            		}
            		break;

            		case AIRVANTAGE_REQUESTS_FOR_FIRMWARE_DOWNLOAD:
            		{
            			DEBUG_PRINT("FOTA : AIRVANTAGE REQUESTS FOR FIRMWARE DOWNLOAD.\r\n");
            		}
            		break;

            		case AIRVANTAGE_REQUESTS_FOR_FIRMWARE_INSTALLATION:
            		{
            			DEBUG_PRINT("FOTA : AIRVANTAGE REQUESTS FOR FIRMWARE INSTALLATION.\r\n");
            		}
            		break;

            		case STARTING_UP_THE_DM_SESSION_WITH_AIR_VANTAGE:
            		{
            			DEBUG_PRINT("FOTA : STARTING UP THE DM SESSION WITH AIRVANTAGE.\r\n");
            		}
            		break;

            		case DM_SESSION_STARTED_TRANSACTIONS_OCCURED:
            		{
            			DEBUG_PRINT("FOTA : DM SESSION STARTED AND TRANSACTIONS OCCURED.\r\n");
            		}
            		break;

            		case DM_SESSION_WITH_AIRVANTAGE_IS_CLOSED:
            		{
            			DEBUG_PRINT("FOTA : DM SESSION WITH AIRVANTAGE IS CLOSED.\r\n");
            			bFotaVerificationIsDone = true;
            		}
            		break;

            		case FIRMWARE_IS_AVAILABLE_FOR_DOWNLOAD:
            		{
            			DEBUG_PRINT("FOTA : FIRMWARE IS AVAILABLE FOR DOWNLOAD.\r\n");
            		}
            		break;

            		case FIRMWARE_IS_DOWNLOADED_STORED_IN_FLASH:
            		{
            			DEBUG_PRINT("FOTA : FIRMWARE IS DOWNLOADED AND STORED IN FLASH.\r\n");
            		}
            		break;

            		case DOWNLOADED_PACKAGE_IS_VERIFIED_AS_CERTIFIED:
            		{
            			DEBUG_PRINT("FOTA : DOWNLOADED PACKAGE IS VERIFIED AS CERTIFIED.\r\n");
            		}
            		break;

            		case STARTING_THE_FIRMWARE_UPDATE:
            		{
            			DEBUG_PRINT("FOTA : STARTING THE FIRMWARE UPDATE.\r\n");
            		}
            		break;

            		case FAILED_TO_UPDATE_THE_FIRMWARE:
            		{
            			DEBUG_PRINT("FOTA : FAILED TO UPDATE THE FIRMWARE.\r\n");
            		}
            		break;

            		case FIRMWARE_UPDATED_SUCCESSFULLY:
            		{
            			DEBUG_PRINT("FOTA : FIRMWARE UPDATED SUCCESSFULLY.\r\n");
            		}
            		break;

            		case DOWNLOAD_IN_PROGRESS_IN_PERCENTAGE:
            		{
            			DEBUG_PRINT("FOTA : DOWNLOAD IN PROGRESS.\r\n");
            		}
            		break;

            		default:
            		break;
            	}
        	}
        	else
        	{
        		DEBUG_PRINT("Error: Service Indication Parsing is failed");
        	}
        }
        break;

        case FIRMWARE_UPDATE_COMPLETED_SUCCESSFULLY:
        {

        }
        break;

        case ERROR_OCCURED_DURING_FIRMWARE_DOWNLOAD:
        {

        }
        break;

        case REVERT_FIRMWARE_TO_PREVIOUS_VERSION:
        {

        }
        break;

        default:
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
static bool validateCommonCommandResponse(uint8_t* response)
{
    bool status = false;

    if(0==memcmp(response,"OK",2))
    {
        status = true;
    }
    else
    {
        status = false;
    }

    return status;
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
bool isFotaVerificationDone(void)
{
    return bFotaVerificationIsDone;
}

/*============================================================================
**
** Function Name:      mdmCtrlr_FlushRxBuffer
**
** Description:        Flushes the Rx Ring Buffer
**
**===========================================================================*/
static DEVICE_SERVICE_INDICATION_TYPE getDeviceServiceIndicationType(void)
{
    uint32_t unsolicitedResponseLength = 0;
    uint8_t* responseBuffer = NULL;
    uint8_t dataString[2] = {0};
    bool readStatus;
    uint8_t serviceIndicationType = SERVICE_INDICATION_ERROR;
    const uint8_t* serviceIndicationCmdString = "\r\n+WDSI: "; 

    while(mdmCtrlr_GetUnsolicitedResponseLength() < SERVICE_INDICATION_RESPONSE_LENGTH);

    if(mdmCtrlr_GetUnsolicitedResponseLength() > SERVICE_INDICATION_RESPONSE_LENGTH)
    {
    	unsolicitedResponseLength = mdmCtrlr_GetUnsolicitedResponseLength() - SERVICE_INDICATION_RESPONSE_LENGTH;
    }
    else
    {
    	unsolicitedResponseLength = mdmCtrlr_GetUnsolicitedResponseLength();
    }


    responseBuffer = (uint8_t*)pvPortMalloc((unsolicitedResponseLength)*(sizeof(uint8_t)));

    if(responseBuffer != NULL)
    {
        readStatus = mdmCtrlr_ReadResponseFromModem(responseBuffer,unsolicitedResponseLength);

        if(readStatus != false)
        {
            if(VERIFIED_EQUAL == strncmp(serviceIndicationCmdString, responseBuffer, SERVICE_INDICATION_CMD_LENGTH))
            {
                if((responseBuffer[SERVICE_INDICATION_CMD_LENGTH] + 1) != '\r')
                {
                    dataString[0] = responseBuffer[SERVICE_INDICATION_CMD_LENGTH];
                    dataString[1] = responseBuffer[SERVICE_INDICATION_CMD_LENGTH + 1];  
                }
                else
                {
                    dataString[0] = 0;
                    dataString[1] = responseBuffer[SERVICE_INDICATION_CMD_LENGTH];                  
                }
                
                serviceIndicationType = atoi(dataString);
            }
            else
            {
                DEBUG_PRINT("Error: Service Indication response is not identified.");
            }
        }
        else
        {
            DEBUG_PRINT("Error: Failed to read service indication response.");
        }

        vPortFree(responseBuffer);
    }

    return serviceIndicationType;
}
