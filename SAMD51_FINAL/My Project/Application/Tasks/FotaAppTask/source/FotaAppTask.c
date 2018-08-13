/*
 * FotaAppTask.c
 *
 * Created: 8/7/2018 8:59:32 PM
 *  Author: anilj
 */ 

#include <hal_delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Application/SerialDebug/SerialDebug.h"
#include "Application/Common/Common.h"
#include "Application/Tasks/ModemTask/include/ModemPowerControl.h"
#include "Application/Tasks/ModemTask/include/ModemAtCommandSet.h"
#include "Application/Tasks/FotaAppTask/include/FotaAppTask.h"
#include "Application/Tasks/ModemTask/include/ModemController.h"

#define FOTA_FWDL_CHECK_TIMER_LOAD_VALUE pdMS_TO_TICKS(86400000) /* Milli second value corresponds to 24 hours */
#define SERVICE_INDICATION_RESPONSE_LENGTH (12)
#define SERVICE_INDICATION_CMD_LENGTH (9)

static CmdResponseType FotaCommandResponse;
static FOTA_MAIN_STATES_T FotaMainState;
static FOTA_APP_OPERATIONAL_STATE_T FotaOperationalMode;
static USER_AGREEMENT_ACTIVATION_STATES_T FotaUserAgreementActivationState;
static bool bFotaVerificationIsDone;
static bool bNewFirmwareInstalled;

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
    if(false == initializeFotaFirmwareDownloadCheckTimer())
    {
        DEBUG_PRINT("Error: FOTA timer not started. Rebooting the system...");
        requestWatchDogForcedReset();
    }
    else
    {
        FotaMainState = INITIALIZE_TO_DEFAULT_FOTA_SETTINGS;
        FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
        FotaUserAgreementActivationState = ACTIVATE_USER_AGREEMENT_FOR_FW_DOWNLOAD;
        bFotaVerificationIsDone = false;
        bNewFirmwareInstalled = false;
    }
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
	DEBUG_PRINT("FOTA : CHECKING FOR MODEM FIRMWARE UPDATE FROM AIR VANTAGE SERVER.");
	requestWatchDogForcedReset();
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
    DEVICE_SERVICE_INDICATION_TYPE serviceIndicationReceived = SERVICE_INDICATION_ERROR;
    static DEVICE_SERVICE_INDICATION_TYPE PrevServiceIndicationReceived = SERVICE_INDICATION_ERROR;

    switch(FotaMainState)
    {
        case INITIALIZE_TO_DEFAULT_FOTA_SETTINGS:
        {
            FotaMainState = SET_APN_TO_C0NNECT_WITH_AIRVANTAGE;
            FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
            FotaUserAgreementActivationState = ACTIVATE_USER_AGREEMENT_FOR_FW_DOWNLOAD;
            bFotaVerificationIsDone = false;
            bNewFirmwareInstalled = false;
            mdmCtrlr_FlushRxBuffer();
        }
        break;

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
                        FotaMainState = ACCEPT_THE_REQUEST_FOR_FIRMWARE_DOWNLOAD;
                        FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
                    }
                    break;

                    case AIRVANTAGE_REQUESTS_FOR_FIRMWARE_INSTALLATION:
                    {
                        DEBUG_PRINT("FOTA : AIRVANTAGE REQUESTS FOR FIRMWARE INSTALLATION.\r\n");
                        FotaMainState = ACCEPT_THE_REQUEST_FOR_FIRMWARE_INSTALL;
                        FotaOperationalMode = FOTA_APP_OPERATIONAL_TX_MODE;
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

                        if((PrevServiceIndicationReceived != DM_SESSION_STARTED_TRANSACTIONS_OCCURED) &&
                           (bFotaVerificationIsDone == false))
                        {
                            mdmCtrlr_FlushRxBuffer();
                            bFotaVerificationIsDone = true;
                            DEBUG_PRINT("FOTA : FIRMWARE UPDATE IS NOT PERFORMED.");
                            DEBUG_PRINT("FOTA : INSTALLED FIRMWARE MATCHES WITH FIRMWARE FROM AIR VANTAGE SERVER.\r\n");
                        }
                        else if((PrevServiceIndicationReceived == DM_SESSION_STARTED_TRANSACTIONS_OCCURED) &&
                                (bFotaVerificationIsDone == false) &&
                                (bNewFirmwareInstalled == true))
                        {
                            mdmCtrlr_FlushRxBuffer();
                            DEBUG_PRINT("FOTA : SUCCESSFULLY INSTALLED THE DOWNLOADED FIRMWARE.");
                            DEBUG_PRINT("\r\n");
                            DEBUG_PRINT("FOTA : REBOOTING THE DEVICE AFTER A SUCCESSFUL FIRMWARE INSTALLATION.\r\n");
                            bNewFirmwareInstalled = false;
                            bFotaVerificationIsDone = true;
                            requestWatchDogForcedReset();
                        }
                        else
                        {
                            /* Please wait for the firmware download to begin */
                        }
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
                        DEBUG_PRINT("FOTA : INSTALLING THE DOWNLOADED FIRMWARE...PLEASE WAIT...\r\n");
                    }
                    break;

                    case STARTING_THE_FIRMWARE_UPDATE:
                    {
                        DEBUG_PRINT("FOTA : STARTING THE FIRMWARE UPDATE.\r\n");
                    }
                    break;

                    case FAILED_TO_UPDATE_THE_FIRMWARE:
                    {
                        mdmCtrlr_FlushRxBuffer();
                        DEBUG_PRINT("FOTA : FAILED TO UPDATE THE FIRMWARE.");
                        DEBUG_PRINT("\r\n");
                        DEBUG_PRINT("FOTA : REBOOTING THE DEVICE AFTER A FAILED ATTEMPT OF FIRMWARE INSTALLATION.\r\n");
                        bFotaVerificationIsDone = true;
                        bNewFirmwareInstalled = false;
                        requestWatchDogForcedReset();
                    }
                    break;

                    case FIRMWARE_UPDATED_SUCCESSFULLY:
                    {
                        mdmCtrlr_FlushRxBuffer();
                        DEBUG_PRINT("FOTA : SUCCESSFULLY INSTALLED THE DOWNLOADED FIRMWARE.");
                        DEBUG_PRINT("\r\n");
                        DEBUG_PRINT("FOTA : REBOOTING THE DEVICE AFTER A SUCCESSFUL FIRMWARE INSTALLATION.\r\n");
                        bFotaVerificationIsDone = true;
                        bNewFirmwareInstalled = true;
                        requestWatchDogForcedReset();
                    }
                    break;

                    case DOWNLOAD_IN_PROGRESS_IN_PERCENTAGE:
                    {
                        DEBUG_PRINT("FOTA : DOWNLOADING THE FIRMWARE FROM AIR VANTAGE SERVER...PLEASE WAIT...\r\n");
                    }
                    break;

                    default:
                    break;
                }

                PrevServiceIndicationReceived = serviceIndicationReceived;
            }
        }
        break;

        case ACCEPT_THE_REQUEST_FOR_FIRMWARE_DOWNLOAD:
        {
            if(FotaOperationalMode == FOTA_APP_OPERATIONAL_TX_MODE)
            {
                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = FOTA_APP_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_WDSR_ACCEPT_FW_DOWNLOAD;
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
                            DEBUG_PRINT("Error: Failed to sent FW download acceptance to Tx Task");
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
                    if(FotaCommandResponse.atCmd == CMD_AT_WDSR_ACCEPT_FW_DOWNLOAD)
                    {
                        if(false != validateCommonCommandResponse(FotaCommandResponse.response))
                        {
                            DEBUG_PRINT("FOTA : Accepted the request for firmware Download and sent the acknowledgment to Airvantage server");
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

        case ACCEPT_THE_REQUEST_FOR_FIRMWARE_INSTALL:
        {
            if(FotaOperationalMode == FOTA_APP_OPERATIONAL_TX_MODE)
            {
                if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
                {
                    if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
                    {
                        TxMsgQueueData.taskID = FOTA_APP_TASK;
                        TxMsgQueueData.atCmd = CMD_AT_WDSR_ACCEPT_FW_INSTALL;
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
                            DEBUG_PRINT("Error: Failed to sent FW install acceptance to Tx Task");
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
                    if(FotaCommandResponse.atCmd == CMD_AT_WDSR_ACCEPT_FW_INSTALL)
                    {
                        if(false != validateCommonCommandResponse(FotaCommandResponse.response))
                        {
                            DEBUG_PRINT("FOTA : Accepted the request for firmware Installation and sent the acknowledgment to Airvantage server");
                            SerialDebugPrint(FotaCommandResponse.response,FotaCommandResponse.length);
                            DEBUG_PRINT("\r\n");
                            DEBUG_PRINT("FOTA : STARTING UP THE FIRMWARE INSTALLATION.\r\n");
                            bNewFirmwareInstalled = true;
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
    DEVICE_SERVICE_INDICATION_TYPE serviceIndicationType = SERVICE_INDICATION_ERROR;
    const int8_t* serviceIndicationCmdString = (int8_t*)"\r\n+WDSI: "; 

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
            if(VERIFIED_EQUAL == strncmp((char*)serviceIndicationCmdString, (char*)responseBuffer, SERVICE_INDICATION_CMD_LENGTH))
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
                
                serviceIndicationType = (DEVICE_SERVICE_INDICATION_TYPE)atoi((char*)dataString);
            }
            else
            {
                /* Service Indication Response is not verified */
            }
        }
        else
        {
               /* Read failure from UART Rx Buffer */
        }

        vPortFree(responseBuffer);
    }

    return serviceIndicationType;
}
