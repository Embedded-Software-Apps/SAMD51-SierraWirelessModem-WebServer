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
        if(getModemPowerStatus() == MDM_PWR_OPERATIONAL_READY_FOR_AT_CMDS)
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
    AtTxMsgType TxMsgQueueData;
    BaseType_t TxQueuePushStatus;

    switch(FotaMainState)
    {
        case SET_APN_TO_C0NNECT_WITH_AIRVANTAGE:
        {
            if(FotaOperationalMode == FOTA_APP_OPERATIONAL_TX_MODE)
            {
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
                            DEBUG_PRINT("FOTA : Started the DM session with Air Vantage.");
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
        	bFotaVerificationIsDone = true;
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
