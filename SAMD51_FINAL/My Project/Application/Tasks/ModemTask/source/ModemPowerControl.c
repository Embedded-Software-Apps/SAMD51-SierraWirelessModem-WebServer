/*=======================================================================================
 * ModemPowerControl.c
 *
 * Implements the power strategies for Sierra wireless modem
 *======================================================================================*/

/****************************************************************************************
 INCLUDE FILES
*****************************************************************************************/
#include "thirdparty\RTOS\freertos\FreeRTOSV10.0.0\Source\include\FreeRTOS.h"
#include "Application\Tasks\ModemTask\include\ModemPowerControl.h"
#include "Application\SerialDebug\SerialDebug.h"
#include "thirdparty\RTOS\freertos\FreeRTOSV10.0.0\Source\include\timers.h"
#include "thirdparty\RTOS\freertos\FreeRTOSV10.0.0\Source\portable\GCC\ARM_CM4F\portmacro.h"
#include "atmel_start_pins.h"
#include "Application/Common/Common.h"
#include "driver_init.h"
#include "Application/Tasks/ModemTask/include/ModemPowerControl.h"
#include <stdbool.h>

/******************************************************************************************
************************************STATIC VARIABLES***************************************
*******************************************************************************************/
static MODEM_POWER_STATES_T ModemPwrState;
static MODEM_POWER_ON_SUBSTATES_T ModemPwrOnSubState;
static MODEM_RESET_SUBSTATES_T ModemResetSubState;
static MODEM_FORCED_OFF_SUBSTATES_T ModemForcedOffSubState;
static TimerHandle_t xPowerOnWaitTimer;
static BaseType_t PowerOnWaitTimerStarted;
static bool PowerOnWaitTimerExpired;

/*******************************************************************************
*
* NAME       : getModemPowerState
*
* DESCRIPTION: Gets the current Modem Power State.
*
********************************************************************************/
MODEM_POWER_STATES_T getModemPowerStatus(void)
{
    return ModemPwrState;
}

/*******************************************************************************
*
* NAME       : modemPowerStateInit
*
* DESCRIPTION: Initializes the Modem Power State Machines
*
********************************************************************************/
void modemPowerStateInit(void)
{
    ModemPwrState = MDM_PWR_RESET_MODEM;
    ModemPwrOnSubState = MDM_PWR_ALL_SIG_INIT;
    ModemResetSubState = MDM_PWR_RESET_ASSERT;
    ModemForcedOffSubState = MDM_PWR_FORCED_OFF_CMPLTD;
}

/*******************************************************************************
*
* NAME       : modemPowerSchedule
*
* DESCRIPTION: Modem Power - Main State Machine Function
*
********************************************************************************/
void modemPowerSchedule(void)
{
    const TickType_t ModemSigInitDelay = pdMS_TO_TICKS(500UL);
    const TickType_t ModemOnBurstDelay = pdMS_TO_TICKS(50UL);
    const TickType_t ModemOnWaitDelay = pdMS_TO_TICKS(3000UL);
    const TickType_t ModemResetWaitDelay = pdMS_TO_TICKS(25UL);
    const TickType_t ModemResetToPowerOnWaitDelay = pdMS_TO_TICKS(500UL);

    switch(ModemPwrState)
    {
        case MDM_PWR_SHUTDOWN:
        {
            /* Turn on the HL7588 modem by providing an active low 
             * signal at POWER_ON_N pin of modem.
             */
            switch(ModemPwrOnSubState)
            {
                case MDM_PWR_ALL_SIG_INIT:
                {
                    gpio_set_pin_direction(MODEM_ON, GPIO_DIRECTION_OUT);
                    gpio_set_pin_function(MODEM_ON, GPIO_PIN_FUNCTION_OFF);
                    gpio_set_pin_level(MODEM_ON,false);

                    gpio_set_pin_direction(MODEM_RESET, GPIO_DIRECTION_OUT);
                    gpio_set_pin_function(MODEM_RESET, GPIO_PIN_FUNCTION_OFF);
                    gpio_set_pin_level(MODEM_RESET,false);

                    gpio_set_pin_direction(MODEM_DTR, GPIO_DIRECTION_OUT);
                    gpio_set_pin_function(MODEM_DTR, GPIO_PIN_FUNCTION_OFF);
                    gpio_set_pin_level(MODEM_DTR,false);

                    ModemPwrOnSubState = MDM_PWR_MDM_ON_SIG_LOW;
                    vTaskDelay(ModemSigInitDelay);
                }
                break;

                case MDM_PWR_MDM_ON_SIG_LOW:
                {
                    /* Give a short 50 ms positive pulse on MODEM ON Pin */
                    gpio_set_pin_level(MODEM_ON,true);
                    ModemPwrOnSubState = MDM_PWR_MDM_ON_SIG_HIGH;
                    vTaskDelay(ModemOnBurstDelay);
                }
                break;

                case MDM_PWR_MDM_ON_SIG_HIGH:
                {
                    /* Wait untill the modem is powered on */
                    ModemPwrOnSubState = MDM_PWR_ON_COMPLETED;
                    vTaskDelay(ModemOnWaitDelay);
                }
                break;

                case MDM_PWR_ON_COMPLETED:
                {
                    //DEBUG_PRINT("Modem Powered On");
                    ModemPwrState = MDM_PWR_OPERATIONAL_READY_FOR_AT_CMDS;
                }
                break;

                default:
                break;
            }
        }
        break;
        
        case MDM_PWR_WAIT_FOR_VGPIO_NORMAL:
        {
            /* This state is not used in the current software implementation.
             * The VGPIO pin needs to be populated in PCB for implementing 
             * this functionality.
             *
             * When the modem is successfully turned on, voltage on this PIN 
             * will be 1.8V.
             */
        }
        break;
        
        case MDM_PWR_DETECT_SIM_CARD:
        {
            /* This state is not used in the current software implementation.
             * The UIM1_DET pin needs to be populated in PCB for implementing
             * this functionality.
             *
             * If a SIM card is inserted in the SIM slot,voltage on the 
             * UIM1_DET pin will be transitioned from logic 0 to logic 1.
             */
        }
        break;
        
        case MDM_PWR_UART_LINK_OPERATIONAL:
        {
            /* This state is not used in the current software implementation.
             * The UART1_CTS pin needs to be populated in PCB for implementing
             * this functionality.
             *
             * If the UART link between SAMD and modem is operational,
             * voltage on this pin will be transitioned from logic 0 to logic 1.
             */
            #if 0
            if(gpio_get_pin_level(MODEM_DTR))
            {
                ModemPwrState = MDM_PWR_OPERATIONAL_READY_FOR_AT_CMDS;
            }
            else
            {

            }
            #endif
        }
        break;
        
        case MDM_PWR_OPERATIONAL_READY_FOR_AT_CMDS:
        {
            /* This is the normal operational state of the modem power state.
             * We may need to improve here once the above mentioned pins are 
             * populated in PCB.
             */
            //DEBUG_PRINT("Modem is ready for AT commands");
        }
        break;
                
        case MDM_PWR_RESET_MODEM:
        {
            switch(ModemResetSubState)
            {
                case MDM_PWR_RESET_ASSERT:
                {
                	/* Apply low level pulse on the POWER ON pin */
                    gpio_set_pin_direction(MODEM_ON, GPIO_DIRECTION_OUT);
                    gpio_set_pin_function(MODEM_ON, GPIO_PIN_FUNCTION_OFF);
                    gpio_set_pin_level(MODEM_ON,true);

                    /* Apply low level pulse on the RESET pin for 25 ms */
                    gpio_set_pin_direction(MODEM_RESET, GPIO_DIRECTION_OUT);
                    gpio_set_pin_function(MODEM_RESET, GPIO_PIN_FUNCTION_OFF);
                    gpio_set_pin_level(MODEM_RESET,true);
                    ModemResetSubState = MDM_PWR_RESET_DEASSERT;
                    vTaskDelay(ModemResetWaitDelay);
                }
                break;

                case MDM_PWR_RESET_DEASSERT:
                {
                    gpio_set_pin_direction(MODEM_RESET, GPIO_DIRECTION_OUT);
                    gpio_set_pin_function(MODEM_RESET, GPIO_PIN_FUNCTION_OFF);
                    gpio_set_pin_level(MODEM_RESET,false);

                    /* Power On the Modem after reset */
                    ModemPwrState = MDM_PWR_SHUTDOWN;
                    ModemPwrOnSubState = MDM_PWR_ALL_SIG_INIT;
                    vTaskDelay(ModemResetToPowerOnWaitDelay);
                }
                break;

                default:
                break;
            }
        }
        break;

        case MDM_PWR_NORMAL_POWER_OFF:
        {
            /* Send AT+CPWROFF */
        }
        break;

        case MDM_PWR_FORCED_POWER_OFF:
        {
            switch(ModemForcedOffSubState)
            {
                case MDM_PWR_FORCED_OFF_INIT:
                {
                    gpio_set_pin_direction(MODEM_ON, GPIO_DIRECTION_OUT);
                    gpio_set_pin_function(MODEM_ON, GPIO_PIN_FUNCTION_OFF);
                    gpio_set_pin_level(MODEM_ON,false);

                    gpio_set_pin_direction(MODEM_RESET, GPIO_DIRECTION_OUT);
                    gpio_set_pin_function(MODEM_RESET, GPIO_PIN_FUNCTION_OFF);
                    gpio_set_pin_level(MODEM_RESET,true);
                    ModemForcedOffSubState = MDM_PWR_FORCED_OFF_CMPLTD;
                    vTaskDelay(ModemResetWaitDelay);
                }
                break;

                case MDM_PWR_FORCED_OFF_CMPLTD:
                {
                    /* Wait Here until next power on*/
                }
                break;

                default:
                break;
            }
        }
        break;
                
        case MDM_PWR_FAULTY:
        {
            
        }
        break;
        
        default:
        break;
    }
}
