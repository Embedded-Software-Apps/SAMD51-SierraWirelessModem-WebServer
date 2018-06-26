/*=======================================================================================
 * ModemPowerControl.c
 *
 * Implements the power strategies for Sierra wireless modem
 *======================================================================================*/
#include <hal_delay.h>
#include <stdio.h>
#include <string.h>
#include "Apps/SerialDebug/SerialDebug.h"
#include "Apps/Common/Common.h"
#include "apps/Tasks/ModemTask/include/ModemPowerControl.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/queue.h"
#include "thirdparty/RTOS/freertos/FreeRTOSV10.0.0/Source/include/projdefs.h"
#include "Apps/Tasks/ModemTask/include/ModemDiagTask.h"


#define MODEM_DIAG_CMDS_MAX (9)
/******************************************************************************************
************************************STATIC VARIABLES***************************************
*******************************************************************************************/
MODEM_DIAG_STATES_T ModemDiagState;
TASK_OPERATION_MODE ModemDiagOpMode;

static uint8_t atResponseData[2+1];
static uint8_t atCgsnResponseData[15+1];
static uint8_t atKgsnResponseData[14+1];
static uint8_t atCarrierResponseData[3+1];

static DIAG_RESPONSE_DATABASE DiagResponseDataBase[MODEM_DIAG_CMDS_MAX] = \
{
    {MODEM_DIAG_TEST_AT, &atResponseData},
    {MODEM_DIAG_TEST_AT, &atCgsnResponseData},
    {MODEM_DIAG_TEST_AT, &atKgsnResponseData},
    {MODEM_DIAG_TEST_AT, &atCarrierResponseData}
};

static void ModemDiagInit(void);
static void ModemDiagSchedule(void);
/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: Gets the current Modem Power State.
*
********************************************************************************/
void ModemDiagTask( void *ModemTaskParam)
{
    const TickType_t xDelayMs = pdMS_TO_TICKS(3500UL);
    ModemDiagInit();

    while(1)
    {
        if(getModemPowerStatus() == MDM_PWR_OPERATIONAL_READY_FOR_AT_CMDS)
        {
            ModemDiagSchedule();
            kickWatchDog();
            DEBUG_PRINT("Running Diag Process Task successfully");
            vTaskDelay(xDelayMs);
        }
    }
}

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: Gets the current Modem Power State.
*
********************************************************************************/
static void ModemDiagInit(void)
{
    ModemDiagState = MODEM_DIAG_TEST_AT;
}

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: Gets the current Modem Power State.
*
********************************************************************************/
void ModemDiagUpdateDataBase(CmdResponseType* cmdResponse)
{
	DEBUG_PRINT("In Update data base");
	SerialDebugPrint(cmdResponse->response,cmdResponse->length);
	vPortFree(cmdResponse->response);
	 ConsoleDebugPrint("Heap size after free", xPortGetFreeHeapSize());
}

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: Gets the current Modem Power State.
*
********************************************************************************/
static void ModemDiagSchedule(void)
{
    const TickType_t QueuePushDelayMs = pdMS_TO_TICKS(100UL);
    const TickType_t QueueWaitDelayMs = pdMS_TO_TICKS(2000UL);
    const TickType_t DiagDelayMs = pdMS_TO_TICKS(2000UL);
    const TickType_t powerUpDelayMs = pdMS_TO_TICKS(7000UL);
    BaseType_t TxQueuePushStatus;
    AtTxMsgType TxMsgQueueData;
    CmdResponseType cmdResponse;

    if (uxQueueMessagesWaiting(AtTransmitQueue) == 0)
    {
        if(pdPASS == xSemaphoreTake(AtTxQueueLoadSemaphore, 0))
        {
            switch(ModemDiagState)
            {
                case MODEM_DIAG_TEST_AT:
                {
                    TxMsgQueueData.taskID = MODEM_DIAG_TASK;
                    TxMsgQueueData.atCmd = CMD_AT;
                    TxMsgQueueData.pData = NULL;
                    TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                    if(TxQueuePushStatus == pdPASS)
                    {
                        DEBUG_PRINT("Sent the Diag data to Tx Task");
                        xSemaphoreGive(AtTxQueueLoadSemaphore);
                        vTaskDelay(DiagDelayMs);
                        ModemDiagState = MODEM_DIAG_GET_IMEI;
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to sent the Diag data to Tx Task");
                        vTaskDelay(DiagDelayMs);
                    }
                }
                break;

                case MODEM_DIAG_GET_IMEI:
                {
                    TxMsgQueueData.taskID = MODEM_DIAG_TASK;
                    TxMsgQueueData.atCmd = CMD_AT_CGSN;
                    TxMsgQueueData.pData = NULL;
                    TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                    if(TxQueuePushStatus == pdPASS)
                    {
                        DEBUG_PRINT("Sent the Diag data to Tx Task");
                        xSemaphoreGive(AtTxQueueLoadSemaphore);
                        vTaskDelay(DiagDelayMs);
                        ModemDiagState = MODEM_DIAG_GET_SERIAL;
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to sent the Diag data to Tx Task");
                        vTaskDelay(DiagDelayMs);
                    }
                }
                break;

                case MODEM_DIAG_GET_SERIAL:
                {
                    TxMsgQueueData.taskID = MODEM_DIAG_TASK;
                    TxMsgQueueData.atCmd = CMD_AT_KGSN;
                    TxMsgQueueData.pData = NULL;
                    TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                    if(TxQueuePushStatus == pdPASS)
                    {
                        DEBUG_PRINT("Sent the Diag data to Tx Task");
                        xSemaphoreGive(AtTxQueueLoadSemaphore);
                        vTaskDelay(DiagDelayMs);
                        ModemDiagState = MODEM_DIAG_GET_CARRIER;
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to sent the Diag data to Tx Task");
                        vTaskDelay(DiagDelayMs);
                    }
                }
                break;

                case MODEM_DIAG_GET_CARRIER:
                {
                    TxMsgQueueData.taskID = MODEM_DIAG_TASK;
                    TxMsgQueueData.atCmd = CMD_AT_WCARRIER;
                    TxMsgQueueData.pData = NULL;
                    TxQueuePushStatus = xQueueSendToBack(AtTransmitQueue, &TxMsgQueueData, QueuePushDelayMs);

                    if(TxQueuePushStatus == pdPASS)
                    {
                        DEBUG_PRINT("Sent the Diag data to Tx Task");
                        xSemaphoreGive(AtTxQueueLoadSemaphore);
                        vTaskDelay(DiagDelayMs);
                        ModemDiagState = 100;
                    }
                    else
                    {
                        DEBUG_PRINT("Failed to sent the Diag data to Tx Task");
                        vTaskDelay(DiagDelayMs);
                    }
                }
                break;

                default:
                break;
            }
        }
    }
}
