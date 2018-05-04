/**
 * \file
 *
 * \brief AC functionality declaration.
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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

#ifndef HAL_AC_SYNC_H_INCLUDED
#define HAL_AC_SYNC_H_INCLUDED

#include <hpl_ac_sync.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_ac_sync
 *
 *@{
 */

/**
 * \brief AC Synchronous descriptor
 *
 * The AC descriptor forward declaration.
 */
struct ac_sync_descriptor;

/**
 * \brief AC descriptor
 */
struct ac_sync_descriptor {
	struct _ac_sync_device device; /*!< AC HPL device descriptor */
};

/**
 * \brief Initialize AC.
 *
 * This function initializes the given AC descriptor.
 * It checks if the given hardware is not initialized and if the given hardware
 * is permitted to be initialized.
 *
 * \param[out] descr An AC descriptor to initialize
 * \param[in] hw The pointer to hardware instance
 *
 * \return Initialization status.
 */
int32_t ac_sync_init(struct ac_sync_descriptor *const descr, void *const hw);

/**
 * \brief Deinitialize AC.
 *
 * This function deinitializes the given AC descriptor.
 * It checks if the given hardware is initialized and if the given hardware is
 * permitted to be deinitialized.
 *
 * \param[in] descr An AC descriptor to deinitialize
 *
 * \return De-initialization status.
 */
int32_t ac_sync_deinit(struct ac_sync_descriptor *const descr);

/**
 * \brief Enable AC
 *
 * This function enables the AC by the given AC descriptor.
 *
 * \param[in] descr An AC descriptor to enable
 *
 * \return Enabling status.
 */
int32_t ac_sync_enable(struct ac_sync_descriptor *const descr);

/**
 * \brief Disable AC
 *
 * This function disables the AC by the given AC descriptor.
 *
 * \param[in] descr An AC descriptor to disable
 *
 * \return Disabling status.
 */
int32_t ac_sync_disable(struct ac_sync_descriptor *const descr);

/**
 * \brief Read Comparison result.
 *
 * \param[in] descr The pointer to AC descriptor
 * \param[in] comp Comparator number
 *
 * \return The comparison result.
 *
 * \retval 0 The comparison result is 0
 * \retval 1 The comparison result is 1
 * \retval ERR_NOT_READY  The comparison result is not ready or input parameters are not correct.
 */
int32_t ac_sync_get_result(struct ac_sync_descriptor *const descr, const uint8_t comp);

/**
 * \brief Start conversion
 *
 * This function starts single-short comparison if signal-shot mode is enabled.
 *
 * \param[in] descr The pointer to AC descriptor
 * \param[in] comp Comparator number
 *
 * \return Start Comparator n start Comparison.
 */
int32_t ac_sync_start_comparison(struct ac_sync_descriptor *const descr, uint8_t comp);

/**
 * \brief Stop conversion.
 *
 * This function stops single-short comparison if signal-shot mode is enabled.
 *
 * \param[in] descr The pointer to AC descriptor
 * \param[in] comp Comparator number
 *
 * \return Start Comparator n start Comparison.
 */
int32_t ac_sync_stop_comparison(struct ac_sync_descriptor *const descr, uint8_t comp);

/**
 * \brief Retrieve the current driver version
 *
 * \return Current driver version.
 */
uint32_t ac_sync_get_version(void);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif
