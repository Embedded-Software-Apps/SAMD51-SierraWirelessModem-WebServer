/**
 * \file
 *
 * \brief AC related functionality declaration.
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
#ifndef _HPL_AC_SYNC_H_INCLUDED
#define _HPL_AC_SYNC_H_INCLUDED

#include <compiler.h>
#include "hpl_irq.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief AC sync device structure
 *
 * The AC device structure forward declaration.
 */
struct _ac_sync_device;

/**
 * \brief AC descriptor device structure.
 */
struct _ac_sync_device {
	void *hw; /*!< Hardware instance */
};

/**
 * \brief Initialize synchronous AC.
 *
 * This function does low level AC configuration.
 *
 * \param[in] device The pointer to device instance
 * \param[in] hw The pointer to hardware instance
 *
 * \return Initialization status
 */
int32_t _ac_sync_init(struct _ac_sync_device *const device, void *const hw);

/**
 * \brief Deinitialize AC.
 *
 * \param[in] device The pointer to device instance
 */
void _ac_sync_deinit(struct _ac_sync_device *const device);

/**
 * \brief Enable AC
 *
 * \param[in] device The pointer to device instance
 */
void _ac_sync_enable(struct _ac_sync_device *const device);

/**
 * \brief Disable AC
 *
 * \param[in] device The pointer to device instance
 */
void _ac_sync_disable(struct _ac_sync_device *const device);

/**
 * \brief Start AC Comparison.
 *
 * \param[in] device The pointer to device instance
 * \param[in] comp Comparator index number
 */
void _ac_sync_start_comparison(struct _ac_sync_device *const device, const uint8_t comp);

/**
 * \brief Stop AC Comparison
 *
 * \param[in] device The pointer to device instance
 * \param[in] comp Comparator index number
 */
void _ac_sync_stop_comparison(struct _ac_sync_device *const device, const uint8_t comp);

/**
 * \brief Read Comparison result.
 *
 * \param[in] device The pointer to device instance
 * \param[in] comp Comparator number
 *
 * \return The comparison result or -1 if input parameters are not correct.
 * \retval -1 Passed parameters were invalid or an AC is not initialized
 * \retval 0 The comparison result is 0
 * \retval 1 The comparison result is 1
 */
int32_t _ac_sync_get_result(struct _ac_sync_device *const device, const uint8_t comp);

#ifdef __cplusplus
}
#endif

#endif /* _HPL_AC_H_INCLUDED */
