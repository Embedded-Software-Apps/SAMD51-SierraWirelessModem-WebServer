/*
 * ModemPowerControl.c
 *
 * Created: 4/21/2018 9:07:31 PM
 *  Author: toani
 */ 
#include "thirdparty\RTOS\freertos\FreeRTOSV10.0.0\Source\include\FreeRTOS.h"
#include "apps\Tasks\ModemTask\include\ModemPowerControl.h"
#include "Apps\SerialDebug\SerialDebug.h"
#include "thirdparty\RTOS\freertos\FreeRTOSV10.0.0\Source\include\timers.h"
#include "thirdparty\RTOS\freertos\FreeRTOSV10.0.0\Source\portable\GCC\ARM_CM4F\portmacro.h"
#include "atmel_start_pins.h"
#include "Apps/Common/Common.h"
#include "Apps/Tasks/ModemTask/include/ModemPowerControl.h"
#include <stdbool.h>


#define POWER_ON_WAIT_TIMER pdMS_TO_TICKS(7000) 

static MODEM_POWER_STATES_T ModemPwrState;
static TimerHandle_t xPowerOnWaitTimer;
static BaseType_t PowerOnWaitTimerStarted;
static bool PowerOnWaitTimerExpired;

/* Perform the HL7618RD modem power on sequence */
/* I FEEL LIKE SOMETHING WRONG HERE */

void modemPowerInit(void)
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
	delay_ms(500);
	
	/* Give a short 50 ms positive pulse on MODEM ON Pin */
	gpio_set_pin_level(MODEM_ON,true);
	delay_ms(50);
	
	/* make MODEM ON to default level */
	delay_ms(3000);
	
	SerialDebugPrint((uint8_t*)"Modem Power On initialization Completed\r\n",41);
}

void performModemReset(void)
{
	gpio_set_pin_direction(MODEM_ON, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(MODEM_ON, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_level(MODEM_ON,true);
	delay_ms(500);
	gpio_set_pin_direction(MODEM_RESET, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(MODEM_RESET, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_level(MODEM_RESET,true);
	delay_ms(30);
	DEBUG_PRINT("Modem Reset Completed");
}







/*****************************************************************************************************
********************* Below code not used as of now***************************************************
******************************************************************************************************/
void modemPwrInit(void)
{

    ModemPwrState = MDM_PWR_SHUTDOWN;
    PowerOnSuccessfull = false;
}

static void PowerOnWaitTimerCallBack(void* param);



static void PowerOnWaitTimerCallBack(void* param)
{
	SerialDebugPrint((uint8_t*)"Power On Wait Timer Expired\r\n",29);
	PowerOnWaitTimerExpired = true;
	PowerOnSuccessfull = true;
	ModemPwrState = MDM_PWR_OPERATIONAL_READY_FOR_AT_CMDS;
}

MODEM_POWER_STATES_T getModemPowerStatus(void)
{
	return ModemPwrState;
}


void modemPwrStateSchedule(void)
{
    switch(ModemPwrState)
    {
        case MDM_PWR_SHUTDOWN:
        {
            /* Turn on the HL7588 modem by providing an active low 
             * signal at POWER_ON_N pin of modem.
             */
#if 1
            gpio_set_pin_direction(MODEM_ON, GPIO_DIRECTION_OUT);
            gpio_set_pin_function(MODEM_ON, GPIO_PIN_FUNCTION_OFF);
			gpio_set_pin_pull_mode(MODEM_ON, GPIO_PULL_UP);
            gpio_set_pin_level(MODEM_ON,false); 
            
			gpio_set_pin_direction(MODEM_RESET, GPIO_DIRECTION_OUT);
			gpio_set_pin_function(MODEM_RESET, GPIO_PIN_FUNCTION_OFF);
			gpio_set_pin_pull_mode(MODEM_RESET, GPIO_PULL_UP);
			gpio_set_pin_level(MODEM_RESET,true);
			
			gpio_set_pin_direction(MODEM_DTR, GPIO_DIRECTION_OUT);
			gpio_set_pin_function(MODEM_DTR, GPIO_PIN_FUNCTION_OFF);
			gpio_set_pin_pull_mode(MODEM_DTR, GPIO_PULL_UP);
			gpio_set_pin_level(MODEM_DTR,false);
#endif
            /* As per the technical specification document of HL7588,
             * it takes 7 seconds for the modem to activate the UART link
             * to receive AT commands from SAMD51.
             * Start the 7 seconds timer in this state.
             */
             xPowerOnWaitTimer = xTimerCreate("PowerOnWaitTimer",POWER_ON_WAIT_TIMER,pdFALSE,0,PowerOnWaitTimerCallBack);
             PowerOnWaitTimerExpired = false;
             if(xPowerOnWaitTimer != NULL)
             {
                 PowerOnWaitTimerStarted = xTimerStart(xPowerOnWaitTimer,0);
                 
                 if(PowerOnWaitTimerStarted == pdPASS)
                 {
                     //SerialDebugPrint((uint8_t*)"Power on wait timer is started\r\n",33);
                     ModemPwrState = MDM_PWR_UART_LINK_OPERATIONAL;
                 }
                 else
                 {
                      //SerialDebugPrint((uint8_t*)"Power on wait timer can't be started\r\n",39);
                 }
             }
             else
             {
                 //SerialDebugPrint((uint8_t*)"Power on wait timer can't be created\r\n",39);
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
             * If the UART link between SAMD and modem is operational, volatage
             * on this pin will be transitioned from logic 0 to logic 1.
             */
#if 0
			if(gpio_get_pin_level(MODEM_DTR))
			{
				//SerialDebugPrint((uint8_t*)"Modem UART Link is operational.\r\n",33);
				ModemPwrState = MDM_PWR_OPERATIONAL_READY_FOR_AT_CMDS;
			}
			else
			{
				//SerialDebugPrint((uint8_t*)"Modem UART Link is not operational.\r\n",37);
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
            /* If the power on start timer is expired, hopefully modem should be up */
            if(PowerOnWaitTimerExpired == true)
            {
                SerialDebugPrint((uint8_t*)"Modem Fully Operational.\r\n",26);
				ModemPwrState = MDM_PWR_FULLY_OPERATIONAL;
            }
            else
            {
                //SerialDebugPrint((uint8_t*)"Timer not expired\r\n",19);
            }       
        }
        break;

        case MDM_PWR_FULLY_OPERATIONAL:
        {
	        
        }
        break;
		        
        case MDM_PWR_RESET_MODEM:
        {
            
        }
        break;

        case MDM_PWR_POWER_OFF:
        {
            /* Send AT+CPWROFF */
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
