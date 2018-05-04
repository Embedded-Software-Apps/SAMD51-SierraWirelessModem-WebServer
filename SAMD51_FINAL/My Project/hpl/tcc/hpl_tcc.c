/**
 * \file
 *
 * \brief SAM TCC
 *
 * Copyright (C) 2014-2017 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <compiler.h>
#include <hpl_pwm.h>
#include <hpl_tcc.h>
#include <hpl_tcc_config.h>
#include <hpl_timer.h>
#include <utils.h>
#include <utils_assert.h>

/**
 * \brief Macro is used to fill TCC configuration structure based on its number
 *
 * \param[in] n The number of structures
 */
#define TCC_CONFIGURATION(n)                                                                                           \
	{                                                                                                                  \
		n, TCC_CTRLA_PRESCALER(CONF_TCC##n##_PRESCALER) | (CONF_TCC##n##_RUNSTDBY << TCC_CTRLA_RUNSTDBY_Pos)           \
		       | TCC_CTRLA_PRESCSYNC(CONF_TCC##n##_PRESCSYNC) | (CONF_TCC##n##_ALOCK << TCC_CTRLA_ALOCK_Pos)           \
		       | (CONF_TCC##n##_CPTEN0 << TCC_CTRLA_CPTEN0_Pos) | (CONF_TCC##n##_CPTEN1 << TCC_CTRLA_CPTEN1_Pos)       \
		       | (CONF_TCC##n##_CPTEN2 << TCC_CTRLA_CPTEN2_Pos) | (CONF_TCC##n##_CPTEN3 << TCC_CTRLA_CPTEN3_Pos),      \
		    (CONF_TCC##n##_LUPD << TCC_CTRLBSET_LUPD_Pos), (CONF_TCC##n##_DBGRUN << TCC_DBGCTRL_DBGRUN_Pos),           \
		    TCC_EVCTRL_CNTSEL(CONF_TCC##n##_CNTSEL) | (CONF_TCC##n##_OVFEO << TCC_EVCTRL_OVFEO_Pos)                    \
		        | (CONF_TCC##n##_TRGEO << TCC_EVCTRL_TRGEO_Pos) | (CONF_TCC##n##_CNTEO << TCC_EVCTRL_CNTEO_Pos)        \
		        | (CONF_TCC##n##_MCEO0 << TCC_EVCTRL_MCEO0_Pos) | (CONF_TCC##n##_MCEO1 << TCC_EVCTRL_MCEO1_Pos)        \
		        | (CONF_TCC##n##_MCEO2 << TCC_EVCTRL_MCEO2_Pos) | (CONF_TCC##n##_MCEO3 << TCC_EVCTRL_MCEO3_Pos)        \
		        | (CONF_TCC##n##_MCEI0 << TCC_EVCTRL_MCEI0_Pos) | (CONF_TCC##n##_MCEI1 << TCC_EVCTRL_MCEI1_Pos)        \
		        | (CONF_TCC##n##_MCEI2 << TCC_EVCTRL_MCEI2_Pos) | (CONF_TCC##n##_MCEI3 << TCC_EVCTRL_MCEI3_Pos)        \
		        | (CONF_TCC##n##_TCEI0 << TCC_EVCTRL_TCEI0_Pos) | (CONF_TCC##n##_TCEI1 << TCC_EVCTRL_TCEI1_Pos)        \
		        | (CONF_TCC##n##_TCINV0 << TCC_EVCTRL_TCINV0_Pos) | (CONF_TCC##n##_TCINV1 << TCC_EVCTRL_TCINV1_Pos)    \
		        | TCC_EVCTRL_EVACT1(CONF_TCC##n##_EVACT1) | TCC_EVCTRL_EVACT0(CONF_TCC##n##_EVACT0),                   \
		    CONF_TCC##n##_PER, CONF_TCC##n##_CC0, CONF_TCC##n##_CC1, CONF_TCC##n##_CC2, CONF_TCC##n##_CC3              \
	}
/* Renamed access REG name PERB -> PERBUF */
#define hri_tcc_write_PERB_reg hri_tcc_write_PERBUF_reg
#define hri_tcc_read_PERB_reg hri_tcc_read_PERBUF_reg

/** Renamed access REG name CCB -> CCBUF */
#define hri_tcc_write_CCB_reg hri_tcc_write_CCBUF_reg
#define hri_tcc_read_CCB_reg hri_tcc_read_CCBUF_reg

#ifndef CONF_TCC0_ENABLE
#define CONF_TCC0_ENABLE 0
#endif
#ifndef CONF_TCC1_ENABLE
#define CONF_TCC1_ENABLE 0
#endif
#ifndef CONF_TCC2_ENABLE
#define CONF_TCC2_ENABLE 0
#endif
#ifndef CONF_TCC3_ENABLE
#define CONF_TCC3_ENABLE 0
#endif
#ifndef CONF_TCC4_ENABLE
#define CONF_TCC4_ENABLE 0
#endif

/** Amount of TCC used. */
#define TCC_AMOUNT (CONF_TCC0_ENABLE + CONF_TCC1_ENABLE + CONF_TCC2_ENABLE + CONF_TCC3_ENABLE + CONF_TCC4_ENABLE)
/**
 * \brief TCC configuration type
 */
struct tcc_configuration {
	uint8_t                number;
	hri_tcc_ctrla_reg_t    ctrl_a;
	hri_tcc_ctrlbset_reg_t ctrl_b;
	hri_tcc_dbgctrl_reg_t  dbg_ctrl;
	hri_tcc_evctrl_reg_t   event_ctrl;
	hri_tcc_per_reg_t      per;
	hri_tcc_cc_reg_t       cc0;
	hri_tcc_cc_reg_t       cc1;
	hri_tcc_cc_reg_t       cc2;
	hri_tcc_cc_reg_t       cc3;
};

#if TCC_AMOUNT < 1
/** Dummy array to pass compiling. */
static struct tcc_configuration _tccs[1] = 0;
#else
#ifndef TCC_CONFIGURATION
/* remove compiling error. */
#define TCC_CONFIGURATION(n)                                                                                           \
	{                                                                                                                  \
		0                                                                                                              \
	}
#endif

/**
 * \brief Array of TCC configurations
 */
static struct tcc_configuration _tccs[] = {
#if CONF_TCC0_ENABLE == 1
    TCC_CONFIGURATION(0),
#endif
#if CONF_TCC1_ENABLE == 1
    TCC_CONFIGURATION(1),
#endif
#if CONF_TCC2_ENABLE == 1
    TCC_CONFIGURATION(2),
#endif
#if CONF_TCC3_ENABLE == 1
    TCC_CONFIGURATION(3),
#endif
#if CONF_TCC4_ENABLE == 1
    TCC_CONFIGURATION(4),
#endif
};
#endif

static struct _timer_device *_tcc0_dev = NULL;

static struct _timer_device *_tcc1_dev = NULL;

static struct _timer_device *_tcc2_dev = NULL;

static struct _timer_device *_tcc3_dev = NULL;

static struct _timer_device *_tcc4_dev = NULL;

/**
 * \brief Set of pointer to hal_timer helper functions
 */
static struct _timer_hpl_interface _tcc_timer_functions = {
    _tcc_timer_init,
    _tcc_timer_deinit,
    _tcc_timer_start,
    _tcc_timer_stop,
    _tcc_timer_set_period,
    _tcc_timer_get_period,
    _tcc_timer_is_started,
    _tcc_timer_set_irq,
};
static int8_t get_tcc_index(const void *const hw);

static inline uint8_t _get_hardware_offset(const void *const hw);
static uint8_t _get_irq_num(const void *const hw);
/**
 * \brief Retrieve IRQ number for the given hardware instance
 */
static uint8_t _get_irq_num(const void *const hw)
{
	switch (_get_hardware_offset(hw)) {
	case 0:
		return TCC0_0_IRQn;
	case 1:
		return TCC1_0_IRQn;
	case 2:
		return TCC2_0_IRQn;
	case 3:
		return TCC3_0_IRQn;
	case 4:
		return TCC4_0_IRQn;
	default:
		return PERIPH_COUNT_IRQn;
	}
}

/**
 * \brief Init irq param with the given tcc hardware instance
 */
static void _tcc_init_irq_param(const void *const hw, void *dev)
{
	if (hw == TCC0) {
		_tcc0_dev = (struct _timer_device *)dev;
	}
	if (hw == TCC1) {
		_tcc1_dev = (struct _timer_device *)dev;
	}
	if (hw == TCC2) {
		_tcc2_dev = (struct _timer_device *)dev;
	}
	if (hw == TCC3) {
		_tcc3_dev = (struct _timer_device *)dev;
	}
	if (hw == TCC4) {
		_tcc4_dev = (struct _timer_device *)dev;
	}
}
/**
 * \brief Initialize TCC
 */
int32_t _tcc_timer_init(struct _timer_device *const device, void *const hw)
{
	int8_t i = get_tcc_index(hw);

	device->hw = hw;
	ASSERT(ARRAY_SIZE(_tccs));

	hri_tcc_wait_for_sync(hw, TCC_SYNCBUSY_SWRST);
	if (hri_tcc_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_DENIED;
	}
	hri_tcc_set_CTRLA_SWRST_bit(hw);
	hri_tcc_wait_for_sync(hw, TCC_SYNCBUSY_SWRST);

	hri_tcc_write_CTRLA_reg(hw, _tccs[i].ctrl_a);
	hri_tcc_set_CTRLB_reg(hw, _tccs[i].ctrl_b);
	hri_tcc_write_DBGCTRL_reg(hw, _tccs[i].dbg_ctrl);
	hri_tcc_write_EVCTRL_reg(hw, _tccs[i].event_ctrl);
	hri_tcc_write_PER_reg(hw, _tccs[i].per);
	hri_tcc_set_INTEN_OVF_bit(hw);

	_tcc_init_irq_param(hw, (void *)device);
	NVIC_DisableIRQ((IRQn_Type)_get_irq_num(hw));
	NVIC_ClearPendingIRQ((IRQn_Type)_get_irq_num(hw));
	NVIC_EnableIRQ((IRQn_Type)_get_irq_num(hw));

	return ERR_NONE;
}
/**
 * \brief De-initialize TCC
 */
void _tcc_timer_deinit(struct _timer_device *const device)
{
	void *const hw = device->hw;
	NVIC_DisableIRQ((IRQn_Type)_get_irq_num(hw));

	hri_tcc_clear_CTRLA_ENABLE_bit(hw);
	hri_tcc_set_CTRLA_SWRST_bit(hw);
}
/**
 * \brief Start hardware timer
 */
void _tcc_timer_start(struct _timer_device *const device)
{
	hri_tcc_set_CTRLA_ENABLE_bit(device->hw);
}
/**
 * \brief Stop hardware timer
 */
void _tcc_timer_stop(struct _timer_device *const device)
{
	hri_tcc_clear_CTRLA_ENABLE_bit(device->hw);
}
/**
 * \brief Set timer period
 */
void _tcc_timer_set_period(struct _timer_device *const device, const uint32_t clock_cycles)
{
	hri_tcc_write_PER_reg(device->hw, clock_cycles);
}
/**
 * \brief Retrieve timer period
 */
uint32_t _tcc_timer_get_period(const struct _timer_device *const device)
{
	return hri_tcc_read_PER_reg(device->hw);
}
/**
 * \brief Check if timer is running
 */
bool _tcc_timer_is_started(const struct _timer_device *const device)
{
	return hri_tcc_get_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Retrieve timer helper functions
 */
struct _timer_hpl_interface *_tcc_get_timer(void)
{
	return &_tcc_timer_functions;
}

/**
 * \brief Retrieve pwm helper functions
 */
struct _pwm_hpl_interface *_tcc_get_pwm(void)
{
	return NULL;
}
/**
 * \brief Set timer IRQ
 *
 * \param[in] hw The pointer to hardware instance
 */
void _tcc_timer_set_irq(struct _timer_device *const device)
{
	_irq_set((IRQn_Type)_get_irq_num(device->hw));
}
/**
 * \internal TCC interrupt handler
 *
 * \param[in] instance TCC instance number
 */
static void tcc_interrupt_handler(struct _timer_device *device)
{
	void *const hw = device->hw;

	if (hri_tcc_get_interrupt_OVF_bit(hw)) {
		hri_tcc_clear_interrupt_OVF_bit(hw);
		device->timer_cb.period_expired(device);
	}
}

/**
* \brief TCC interrupt handler
*/
void TCC0_0_Handler(void)
{
	tcc_interrupt_handler(_tcc0_dev);
}

/**
* \brief TCC interrupt handler
*/
void TCC1_0_Handler(void)
{
	tcc_interrupt_handler(_tcc1_dev);
}

/**
* \brief TCC interrupt handler
*/
void TCC2_0_Handler(void)
{
	tcc_interrupt_handler(_tcc2_dev);
}

/**
* \brief TCC interrupt handler
*/
void TCC3_0_Handler(void)
{
	tcc_interrupt_handler(_tcc3_dev);
}

/**
* \brief TCC interrupt handler
*/
void TCC4_0_Handler(void)
{
	tcc_interrupt_handler(_tcc4_dev);
}

/**
 * \internal Retrieve TCC index
 *
 * \param[in] hw The pointer to hardware instance
 *
 * \return The index of selected TCC
 */
static int8_t get_tcc_index(const void *const hw)
{
	uint8_t tcc_offset = _get_hardware_offset(hw);
	uint8_t i;

	for (i = 0; i < ARRAY_SIZE(_tccs); i++) {
		if (_tccs[i].number == tcc_offset) {
			return i;
		}
	}

	ASSERT(false);
	return -1;
}

static inline uint8_t _get_hardware_offset(const void *const hw)
{
	/* List of available TC modules. */
	Tcc *const tcc_modules[TCC_INST_NUM] = TCC_INSTS;

	/* Find index for TC instance. */
	for (uint32_t i = 0; i < TCC_INST_NUM; i++) {
		if ((uint32_t)hw == (uint32_t)tcc_modules[i]) {
			return i;
		}
	}
	return 0;
}
