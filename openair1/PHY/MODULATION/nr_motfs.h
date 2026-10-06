/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

// !!!!!!!!! for modified OTFS !!!!!!!!!
#ifndef __NR_MOTFS_H__
#define __NR_MOTFS_H__

#include <stdint.h>
#include "common/platform_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// !!!!!!!!! for modified OTFS !!!!!!!!!
#define NR_MOTFS_MIN_N 1
#define NR_MOTFS_MAX_N 14
#define NR_MOTFS_COEFF_Q 15
#define NR_MOTFS_COEFF_SCALE ((int16_t)32767)

/*
 * Buffers are ordered as buffer[t * M + m].
 * In-place operation is supported only for N == 1. For N > 1, callers must
 * provide distinct input and output buffers.
 */
// !!!!!!!!! for modified OTFS !!!!!!!!!
int nr_motfs_time_precoding(const c16_t *in, c16_t *out, uint16_t M, uint8_t N);
// !!!!!!!!! for modified OTFS !!!!!!!!!
int nr_motfs_time_deprecoding(const c16_t *in, c16_t *out, uint16_t M, uint8_t N);

#ifdef __cplusplus
}
#endif

#endif /* __NR_MOTFS_H__ */
