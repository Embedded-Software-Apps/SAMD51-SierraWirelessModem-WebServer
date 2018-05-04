/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

/**
 * Example of using AC_0 to generate waveform.
 */
void AC_0_example(void)
{
	ac_sync_enable(&AC_0);
	ac_sync_start_comparison(&AC_0, 0);

	while (true) {
		ac_sync_get_result(&AC_0, 0);
	}
}

/**
 * Example of using ADC_0 to generate waveform.
 */
void ADC_0_example(void)
{
	uint8_t buffer[2];

	adc_sync_enable_channel(&ADC_0, 0);

	while (1) {
		adc_sync_read_channel(&ADC_0, 0, buffer, 2);
	}
}

/**
 * Example of using ADC_1 to generate waveform.
 */
void ADC_1_example(void)
{
	uint8_t buffer[2];

	adc_sync_enable_channel(&ADC_1, 0);

	while (1) {
		adc_sync_read_channel(&ADC_1, 0, buffer, 2);
	}
}

/**
 * Example of using DAC_0 to generate waveform.
 */
void DAC_0_example(void)
{
	uint16_t i = 0;

	dac_sync_enable_channel(&DAC_0, 0);

	for (;;) {
		dac_sync_write(&DAC_0, 0, &i, 1);
		i = (i + 1) % 1024;
	}
}

/**
 * Example of using EXTERNAL_IRQ_0
 */
void EXTERNAL_IRQ_0_example(void)
{
}

static uint8_t src_data[512];
static uint8_t chk_data[512];
/**
 * Example of using FLASH_0 to read and write buffer.
 */
void FLASH_0_example(void)
{
	uint32_t page_size;
	uint16_t i;

	/* Init source data */
	page_size = flash_get_page_size(&FLASH_0);

	for (i = 0; i < page_size; i++) {
		src_data[i] = i;
	}

	/* Write data to flash */
	flash_write(&FLASH_0, 0x3200, src_data, page_size);

	/* Read data from flash */
	flash_read(&FLASH_0, 0x3200, chk_data, page_size);
}

/**
 * Example of using USART_2 to write "Hello World" using the IO abstraction.
 */
void USART_2_example(void)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_2, &io);
	usart_sync_enable(&USART_2);

	io_write(io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using USART_3 to write "Hello World" using the IO abstraction.
 */
void USART_3_example(void)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_3, &io);
	usart_sync_enable(&USART_3);

	io_write(io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using USART_1 to write "Hello World" using the IO abstraction.
 */
void USART_1_example(void)
{
#if 0
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_1, &io);
	usart_sync_enable(&USART_1);

	io_write(io, (uint8_t *)"Hello World!", 12);
#endif
}

void delay_example(void)
{
	delay_ms(5000);
}

/**
 * Example of using PWM_0.
 */
void PWM_0_example(void)
{
	pwm_set_parameters(&PWM_0, 10000, 5000);
	pwm_enable(&PWM_0);
}

/**
 * Example of using PWM_1.
 */
void PWM_1_example(void)
{
	pwm_set_parameters(&PWM_1, 10000, 5000);
	pwm_enable(&PWM_1);
}

/**
 * Example of using PWM_2.
 */
void PWM_2_example(void)
{
	pwm_set_parameters(&PWM_2, 10000, 5000);
	pwm_enable(&PWM_2);
}

static struct timer_task TIMER_0_task1, TIMER_0_task2;

/**
 * Example of using TIMER_0.
 */
static void TIMER_0_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_0_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_0_example(void)
{
	TIMER_0_task1.interval = 100;
	TIMER_0_task1.cb       = TIMER_0_task1_cb;
	TIMER_0_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_0_task2.interval = 200;
	TIMER_0_task2.cb       = TIMER_0_task2_cb;
	TIMER_0_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_0, &TIMER_0_task1);
	timer_add_task(&TIMER_0, &TIMER_0_task2);
	timer_start(&TIMER_0);
}

static struct timer_task TIMER_1_task1, TIMER_1_task2;

/**
 * Example of using TIMER_1.
 */
static void TIMER_1_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_1_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_1_example(void)
{
	TIMER_1_task1.interval = 100;
	TIMER_1_task1.cb       = TIMER_1_task1_cb;
	TIMER_1_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_1_task2.interval = 200;
	TIMER_1_task2.cb       = TIMER_1_task2_cb;
	TIMER_1_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_1, &TIMER_1_task1);
	timer_add_task(&TIMER_1, &TIMER_1_task2);
	timer_start(&TIMER_1);
}

static struct timer_task TIMER_2_task1, TIMER_2_task2;

/**
 * Example of using TIMER_2.
 */
static void TIMER_2_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_2_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_2_example(void)
{
	TIMER_2_task1.interval = 100;
	TIMER_2_task1.cb       = TIMER_2_task1_cb;
	TIMER_2_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_2_task2.interval = 200;
	TIMER_2_task2.cb       = TIMER_2_task2_cb;
	TIMER_2_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_2, &TIMER_2_task1);
	timer_add_task(&TIMER_2, &TIMER_2_task2);
	timer_start(&TIMER_2);
}

/**
 * Example of using TIMER_3.
 */
struct timer_task TIMER_3_task1, TIMER_3_task2;

static void TIMER_3_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_3_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_3_example(void)
{
	TIMER_3_task1.interval = 100;
	TIMER_3_task1.cb       = TIMER_3_task1_cb;
	TIMER_3_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_3_task2.interval = 200;
	TIMER_3_task2.cb       = TIMER_3_task2_cb;
	TIMER_3_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_3, &TIMER_3_task1);
	timer_add_task(&TIMER_3, &TIMER_3_task2);
	timer_start(&TIMER_3);
}

/**
 * Example of using TIMER_4.
 */
struct timer_task TIMER_4_task1, TIMER_4_task2;

static void TIMER_4_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_4_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_4_example(void)
{
	TIMER_4_task1.interval = 100;
	TIMER_4_task1.cb       = TIMER_4_task1_cb;
	TIMER_4_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_4_task2.interval = 200;
	TIMER_4_task2.cb       = TIMER_4_task2_cb;
	TIMER_4_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_4, &TIMER_4_task1);
	timer_add_task(&TIMER_4, &TIMER_4_task2);
	timer_start(&TIMER_4);
}

/**
 * Example of using TIMER_5.
 */
struct timer_task TIMER_5_task1, TIMER_5_task2;

static void TIMER_5_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_5_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_5_example(void)
{
	TIMER_5_task1.interval = 100;
	TIMER_5_task1.cb       = TIMER_5_task1_cb;
	TIMER_5_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_5_task2.interval = 200;
	TIMER_5_task2.cb       = TIMER_5_task2_cb;
	TIMER_5_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_5, &TIMER_5_task1);
	timer_add_task(&TIMER_5, &TIMER_5_task2);
	timer_start(&TIMER_5);
}

/**
 * Example of using TIMER_6.
 */
struct timer_task TIMER_6_task1, TIMER_6_task2;

static void TIMER_6_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_6_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_6_example(void)
{
	TIMER_6_task1.interval = 100;
	TIMER_6_task1.cb       = TIMER_6_task1_cb;
	TIMER_6_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_6_task2.interval = 200;
	TIMER_6_task2.cb       = TIMER_6_task2_cb;
	TIMER_6_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_6, &TIMER_6_task1);
	timer_add_task(&TIMER_6, &TIMER_6_task2);
	timer_start(&TIMER_6);
}

/**
 * Example of using TIMER_7.
 */
struct timer_task TIMER_7_task1, TIMER_7_task2;

static void TIMER_7_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_7_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_7_example(void)
{
	TIMER_7_task1.interval = 100;
	TIMER_7_task1.cb       = TIMER_7_task1_cb;
	TIMER_7_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_7_task2.interval = 200;
	TIMER_7_task2.cb       = TIMER_7_task2_cb;
	TIMER_7_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_7, &TIMER_7_task1);
	timer_add_task(&TIMER_7, &TIMER_7_task2);
	timer_start(&TIMER_7);
}

/**
 * Example of using WDT_0.
 */
void WDT_0_example(void)
{
	uint32_t clk_rate;
	uint16_t timeout_period;

	clk_rate       = 1000;
	timeout_period = 4096;
	wdt_set_timeout_period(&WDT_0, clk_rate, timeout_period);
	wdt_enable(&WDT_0);
}
