/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_INCLUDED
#define DRIVER_INIT_INCLUDED

#include "atmel_start_pins.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>

#include <hal_ac_sync.h>

#include <hal_adc_sync.h>

#include <hal_adc_sync.h>

#include <hal_dac_sync.h>

#include <hal_ext_irq.h>

#include <hal_evsys.h>

#include <hal_flash.h>

#include <hal_usart_sync.h>

#include <hal_usart_sync.h>

#include <hal_usart_sync.h>

#include <hpl_usart_async.h>

#include <hal_delay.h>
#include <hal_pwm.h>
#include <hpl_tc_base.h>
#include <hal_pwm.h>
#include <hpl_tc_base.h>
#include <hal_pwm.h>
#include <hpl_tc_base.h>
#include <hal_timer.h>
#include <hpl_tc_base.h>
#include <hal_timer.h>
#include <hpl_tc_base.h>
#include <hal_timer.h>
#include <hpl_tc_base.h>
#include <hal_timer.h>
#include <hpl_tcc.h>
#include <hal_timer.h>
#include <hpl_tcc.h>
#include <hal_timer.h>
#include <hpl_tcc.h>
#include <hal_timer.h>
#include <hpl_tcc.h>
#include <hal_timer.h>
#include <hpl_tcc.h>

#include <hal_wdt.h>

extern struct ac_sync_descriptor AC_0;

extern struct adc_sync_descriptor ADC_0;

extern struct _adc_async_device SENSOR_ADC;

extern struct adc_sync_descriptor ADC_1;

extern struct dac_sync_descriptor DAC_0;

extern struct flash_descriptor FLASH_0;

extern struct usart_sync_descriptor USART_2;

extern struct usart_sync_descriptor USART_3;

extern struct _usart_async_device SERIAL_DEBUG_PRINT;

extern struct wdt_descriptor WDT_0;

extern struct pwm_descriptor PWM_0;

extern struct pwm_descriptor PWM_1;

extern struct pwm_descriptor   PWM_2;
extern struct timer_descriptor TIMER_0;
extern struct timer_descriptor TIMER_1;
extern struct timer_descriptor TIMER_2;

extern struct timer_descriptor TIMER_3;

extern struct timer_descriptor TIMER_4;

extern struct timer_descriptor TIMER_5;

extern struct timer_descriptor TIMER_6;

extern struct timer_descriptor TIMER_7;

void AC_0_PORT_init(void);
void AC_0_CLOCK_init(void);
void AC_0_init(void);

void ADC_0_PORT_init(void);
void ADC_0_CLOCK_init(void);
void ADC_0_init(void);

void ADC_1_PORT_init(void);
void ADC_1_CLOCK_init(void);
void ADC_1_init(void);

void DAC_0_PORT_init(void);
void DAC_0_CLOCK_init(void);
void DAC_0_init(void);

void FLASH_0_init(void);
void FLASH_0_CLOCK_init(void);

void USART_2_PORT_init(void);
void USART_2_CLOCK_init(void);
void USART_2_init(void);

void ModemData__PORT_init(void);
void ModemData__CLOCK_init(void);
void ModemData__init(void);

void SerialDebug_PORT_init(void);
void SerialDebug_CLOCK_init(void);
void SerialDebug_init(void);

void delay_driver_init(void);

void PWM_0_PORT_init(void);
void PWM_0_CLOCK_init(void);
void PWM_0_init(void);

void PWM_1_PORT_init(void);
void PWM_1_CLOCK_init(void);
void PWM_1_init(void);

void PWM_2_PORT_init(void);
void PWM_2_CLOCK_init(void);
void PWM_2_init(void);

void TIMER_3_CLOCK_init(void);
void TIMER_3_init(void);

void TIMER_4_CLOCK_init(void);
void TIMER_4_init(void);

void TIMER_5_CLOCK_init(void);
void TIMER_5_init(void);

void TIMER_6_CLOCK_init(void);
void TIMER_6_init(void);

void TIMER_7_CLOCK_init(void);
void TIMER_7_init(void);

void WDT_0_CLOCK_init(void);
void WDT_0_init(void);
/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_INCLUDED
