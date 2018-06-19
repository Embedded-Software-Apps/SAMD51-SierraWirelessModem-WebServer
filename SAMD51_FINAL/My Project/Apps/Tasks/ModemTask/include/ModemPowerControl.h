/*
 * ModemPowerControl.h
 *
 * Created: 4/21/2018 9:07:06 PM
 *  Author: toani
 */ 


#ifndef MODEMPOWERCONTROL_H_
#define MODEMPOWERCONTROL_H_

/* Power states for modem controller */
typedef enum
{
	MDM_PWR_SHUTDOWN,
	MDM_PWR_WAIT_FOR_VGPIO_NORMAL,
	MDM_PWR_DETECT_SIM_CARD,
	MDM_PWR_UART_LINK_OPERATIONAL,
	MDM_PWR_OPERATIONAL_READY_FOR_AT_CMDS,
	MDM_PWR_POWER_OFF,
	MDM_PWR_RESET_MODEM,
	MDM_PWR_FAULTY
}MODEM_POWER_STATES_T;

/* Modem Power On Sub States */
typedef enum
{
	MDM_PWR_ALL_SIG_INIT,
	MDM_PWR_MDM_ON_SIG_LOW,
	MDM_PWR_MDM_ON_SIG_HIGH,
	MDM_PWR_ON_COMPLETED

}MODEM_POWER_ON_SUBSTATES_T;

/* Modem Control Pins */
#define MODEM_ON GPIO(GPIO_PORTB, 16)
#define MODEM_RESET GPIO(GPIO_PORTA, 19)
#define MODEM_DTR GPIO(GPIO_PORTA, 16)

void EnableWatchDog(void);
void modemPowerSchedule(void);
MODEM_POWER_STATES_T getModemPowerStatus(void);
void modemPowerStateInit(void);
void performModemReset(void);

#endif /* MODEMPOWERCONTROL_H_ */
