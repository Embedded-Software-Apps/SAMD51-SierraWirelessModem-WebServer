/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>
#include <hpl_adc_base.h>
#include <hpl_adc_base.h>
#include <hpl_usart_async.h>

#include "Application/SerialDebug/SerialDebug.h"
#include "Application/Tasks/ModemTask/include/ModemController.h"
#include "atmel_start.h"

struct timer_descriptor TIMER_0;
struct timer_descriptor TIMER_1;
struct timer_descriptor TIMER_2;

struct ac_sync_descriptor AC_0;

struct adc_sync_descriptor ADC_0;

struct adc_sync_descriptor ADC_1;

struct dac_sync_descriptor DAC_0;

struct flash_descriptor FLASH_0;

struct usart_sync_descriptor USART_2;

struct usart_sync_descriptor USART_3;

struct pwm_descriptor PWM_0;

struct pwm_descriptor PWM_1;

struct pwm_descriptor PWM_2;

struct timer_descriptor TIMER_3;

struct timer_descriptor TIMER_4;

struct timer_descriptor TIMER_5;

struct timer_descriptor TIMER_6;

struct timer_descriptor TIMER_7;

struct wdt_descriptor WDT_0;

void AC_0_PORT_init(void)
{
}

void AC_0_CLOCK_init(void)
{

	hri_mclk_set_APBCMASK_AC_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, AC_GCLK_ID, CONF_GCLK_AC_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void AC_0_init(void)
{
	AC_0_CLOCK_init();
	ac_sync_init(&AC_0, AC);
	AC_0_PORT_init();
}

void ADC_0_PORT_init(void)
{

	// Disable digital pin circuitry
	gpio_set_pin_direction(PA02, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(PA02, PINMUX_PA02B_ADC0_AIN0);

	// Disable digital pin circuitry
	gpio_set_pin_direction(PA03, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(PA03, PINMUX_PA03B_ADC0_AIN1);

	// Disable digital pin circuitry
	gpio_set_pin_direction(PB08, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(PB08, PINMUX_PB08B_ADC0_AIN2);

	// Disable digital pin circuitry
	gpio_set_pin_direction(PB09, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(PB09, PINMUX_PB09B_ADC0_AIN3);

	// Disable digital pin circuitry
	gpio_set_pin_direction(PA04, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(PA04, PINMUX_PA04B_ADC0_AIN4);

	// Disable digital pin circuitry
	gpio_set_pin_direction(PA05, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(PA05, PINMUX_PA05B_ADC0_AIN5);

	// Disable digital pin circuitry
	gpio_set_pin_direction(PA06, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(PA06, PINMUX_PA06B_ADC0_AIN6);

	// Disable digital pin circuitry
	gpio_set_pin_direction(PA07, GPIO_DIRECTION_OFF);

	gpio_set_pin_function(PA07, PINMUX_PA07B_ADC0_AIN7);
}

void ADC_0_CLOCK_init(void)
{
	hri_mclk_set_APBDMASK_ADC0_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, ADC0_GCLK_ID, CONF_GCLK_ADC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void ADC_0_init(void)
{
	ADC_0_CLOCK_init();
	ADC_0_PORT_init();
	adc_sync_init(&ADC_0, ADC0, (void *)NULL);
}

void ADC_1_PORT_init(void)
{
}

void ADC_1_CLOCK_init(void)
{
	hri_mclk_set_APBDMASK_ADC1_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, ADC1_GCLK_ID, CONF_GCLK_ADC1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void ADC_1_init(void)
{
	ADC_1_CLOCK_init();
	ADC_1_PORT_init();
	adc_sync_init(&ADC_1, ADC1, (void *)NULL);
}

void DAC_0_PORT_init(void)
{
}

void DAC_0_CLOCK_init(void)
{

	hri_mclk_set_APBDMASK_DAC_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, DAC_GCLK_ID, CONF_GCLK_DAC_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void DAC_0_init(void)
{
	DAC_0_CLOCK_init();
	dac_sync_init(&DAC_0, DAC);
	DAC_0_PORT_init();
}

void EXTERNAL_IRQ_0_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, EIC_GCLK_ID, CONF_GCLK_EIC_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_mclk_set_APBAMASK_EIC_bit(MCLK);

	ext_irq_init();
}

void EVENT_SYSTEM_0_init(void)
{

	hri_mclk_set_APBBMASK_EVSYS_bit(MCLK);
	event_system_init();
}

void FLASH_0_CLOCK_init(void)
{

	hri_mclk_set_AHBMASK_NVMCTRL_bit(MCLK);
}

void FLASH_0_init(void)
{
	FLASH_0_CLOCK_init();
	flash_init(&FLASH_0, NVMCTRL);
}

void USART_2_PORT_init(void)
{
	gpio_set_pin_function(PA13, PINMUX_PA13D_SERCOM4_PAD0);

	gpio_set_pin_function(PA15, PINMUX_PA15D_SERCOM4_PAD3);
}

void USART_2_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM4_GCLK_ID_CORE, CONF_GCLK_SERCOM4_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM4_GCLK_ID_SLOW, CONF_GCLK_SERCOM4_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBDMASK_SERCOM4_bit(MCLK);
}

void USART_2_init(void)
{
	USART_2_CLOCK_init();
	usart_sync_init(&USART_2, SERCOM4, (void *)NULL);
	USART_2_PORT_init();
}

void ModemData_PORT_init(void)
{
	gpio_set_pin_function(PA17, PINMUX_PA17D_SERCOM3_PAD0);

	gpio_set_pin_function(PA18, PINMUX_PA18D_SERCOM3_PAD2);
}

void ModemData_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM3_GCLK_ID_CORE, CONF_GCLK_SERCOM3_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM3_GCLK_ID_SLOW, CONF_GCLK_SERCOM3_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBBMASK_SERCOM3_bit(MCLK);
}

void ModemData_init(void)
{
	ModemData_CLOCK_init();
	mdmCtrlr_DataCommInit();
	//usart_sync_init(&USART_3, SERCOM3, (void *)NULL);
	//ModemUsartOsInit();
	ModemData_PORT_init();
}

void SerialDebug_PORT_init(void)
{

	gpio_set_pin_function(PA23, PINMUX_PA23D_SERCOM5_PAD0);

	gpio_set_pin_function(PB23, PINMUX_PB23D_SERCOM5_PAD3);
}

void SerialDebug_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM5_GCLK_ID_CORE, CONF_GCLK_SERCOM5_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM5_GCLK_ID_SLOW, CONF_GCLK_SERCOM5_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBDMASK_SERCOM5_bit(MCLK);
}

void SerialDebug_init(void)
{
	SerialDebug_CLOCK_init();
	SerialDebugPrintInit();
	SerialDebug_PORT_init();
}

void delay_driver_init(void)
{
	delay_init(SysTick);
}

uint32_t getSystemTime(void)
{
	return _system_time_get(SysTick);
}

void PWM_0_PORT_init(void)
{
}

void PWM_0_CLOCK_init(void)
{
	hri_mclk_set_APBAMASK_TC0_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TC0_GCLK_ID, CONF_GCLK_TC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void PWM_0_init(void)
{
	PWM_0_CLOCK_init();
	PWM_0_PORT_init();
	pwm_init(&PWM_0, TC0, _tc_get_pwm());
}

void PWM_1_PORT_init(void)
{
}

void PWM_1_CLOCK_init(void)
{
	hri_mclk_set_APBAMASK_TC1_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TC1_GCLK_ID, CONF_GCLK_TC1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void PWM_1_init(void)
{
	PWM_1_CLOCK_init();
	PWM_1_PORT_init();
	pwm_init(&PWM_1, TC1, _tc_get_pwm());
}

void PWM_2_PORT_init(void)
{
}

void PWM_2_CLOCK_init(void)
{
	hri_mclk_set_APBBMASK_TC2_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TC2_GCLK_ID, CONF_GCLK_TC2_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void PWM_2_init(void)
{
	PWM_2_CLOCK_init();
	PWM_2_PORT_init();
	pwm_init(&PWM_2, TC2, _tc_get_pwm());
}

/**
 * \brief Timer initialization function
 *
 * Enables Timer peripheral, clocks and initializes Timer driver
 */
static void TIMER_0_init(void)
{
	hri_mclk_set_APBBMASK_TC3_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TC3_GCLK_ID, CONF_GCLK_TC3_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	timer_init(&TIMER_0, TC3, _tc_get_timer());
}

/**
 * \brief Timer initialization function
 *
 * Enables Timer peripheral, clocks and initializes Timer driver
 */
static void TIMER_1_init(void)
{
	hri_mclk_set_APBCMASK_TC4_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TC4_GCLK_ID, CONF_GCLK_TC4_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	timer_init(&TIMER_1, TC4, _tc_get_timer());
}

/**
 * \brief Timer initialization function
 *
 * Enables Timer peripheral, clocks and initializes Timer driver
 */
static void TIMER_2_init(void)
{
	hri_mclk_set_APBCMASK_TC5_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TC5_GCLK_ID, CONF_GCLK_TC5_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	timer_init(&TIMER_2, TC5, _tc_get_timer());
}

void TIMER_3_CLOCK_init(void)
{
	hri_mclk_set_APBBMASK_TCC0_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TCC0_GCLK_ID, CONF_GCLK_TCC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void TIMER_3_init(void)
{
	TIMER_3_CLOCK_init();
	timer_init(&TIMER_3, TCC0, _tcc_get_timer());
}

void TIMER_4_CLOCK_init(void)
{
	hri_mclk_set_APBBMASK_TCC1_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TCC1_GCLK_ID, CONF_GCLK_TCC1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void TIMER_4_init(void)
{
	TIMER_4_CLOCK_init();
	timer_init(&TIMER_4, TCC1, _tcc_get_timer());
}

void TIMER_5_CLOCK_init(void)
{
	hri_mclk_set_APBCMASK_TCC2_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TCC2_GCLK_ID, CONF_GCLK_TCC2_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void TIMER_5_init(void)
{
	TIMER_5_CLOCK_init();
	timer_init(&TIMER_5, TCC2, _tcc_get_timer());
}

void TIMER_6_CLOCK_init(void)
{
	hri_mclk_set_APBCMASK_TCC3_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TCC3_GCLK_ID, CONF_GCLK_TCC3_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void TIMER_6_init(void)
{
	TIMER_6_CLOCK_init();
	timer_init(&TIMER_6, TCC3, _tcc_get_timer());
}

void TIMER_7_CLOCK_init(void)
{
	hri_mclk_set_APBDMASK_TCC4_bit(MCLK);
	hri_gclk_write_PCHCTRL_reg(GCLK, TCC4_GCLK_ID, CONF_GCLK_TCC4_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void TIMER_7_init(void)
{
	TIMER_7_CLOCK_init();
	timer_init(&TIMER_7, TCC4, _tcc_get_timer());
}

void WDT_0_CLOCK_init(void)
{
	hri_mclk_set_APBAMASK_WDT_bit(MCLK);
}

void WDT_0_init(void)
{
	WDT_0_CLOCK_init();

	if(0 == wdt_init(&WDT_0, WDT))
	{
		//DEBUG_PRINT("WDT Initialised");
	}
	else
	{
		//DEBUG_PRINT("WDT Failed to initialize");
	}
}

void system_init(void)
{
	init_mcu();

	delay_driver_init();

	AC_0_init();

	//ADC_0_init();

	//ADC_1_init();

	DAC_0_init();

	EXTERNAL_IRQ_0_init();

	EVENT_SYSTEM_0_init();

	FLASH_0_init();

	SerialDebug_init();
	
	ModemData_init();

	PWM_0_init();

	PWM_1_init();

	PWM_2_init();

	TIMER_0_init();
	TIMER_1_init();
	TIMER_2_init();
	TIMER_3_init();

	TIMER_4_init();

	TIMER_5_init();

	TIMER_6_init();

	TIMER_7_init();
	
	WDT_0_init();

	//DEBUG_PRINT("SAMD51 Firmwares are Initialized successfully");
}
