/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

// !!!!!!!!! for modified OTFS !!!!!!!!!
#include "nr_motfs.h"

#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// !!!!!!!!! for modified OTFS !!!!!!!!!
static bool nr_motfs_valid_args(const c16_t *in, const c16_t *out, uint16_t M, uint8_t N)
{
  return in != NULL && out != NULL && M > 0 && N >= NR_MOTFS_MIN_N && N <= NR_MOTFS_MAX_N && (N == 1 || in != out);
}

// !!!!!!!!! for modified OTFS !!!!!!!!!
static int16_t nr_motfs_saturate16(int64_t x)
{
  if (x > INT16_MAX)
    return INT16_MAX;
  if (x < INT16_MIN)
    return INT16_MIN;
  return (int16_t)x;
}

// !!!!!!!!! for modified OTFS !!!!!!!!!
static int64_t nr_motfs_round_shift_q15(int64_t x)
{
  const int64_t half = 1LL << (NR_MOTFS_COEFF_Q - 1);
  return x >= 0 ? (x + half) >> NR_MOTFS_COEFF_Q : -(((-x) + half) >> NR_MOTFS_COEFF_Q);
}

// !!!!!!!!! for modified OTFS !!!!!!!!!
static int16_t nr_motfs_coeff(double value)
{
  const long coeff = lround(value * NR_MOTFS_COEFF_SCALE);
  if (coeff > INT16_MAX)
    return INT16_MAX;
  if (coeff < INT16_MIN)
    return INT16_MIN;
  return (int16_t)coeff;
}

// !!!!!!!!! for modified OTFS !!!!!!!!!
static void nr_motfs_unitary_transform(const c16_t *in, c16_t *out, uint16_t M, uint8_t N, bool inverse)
{
  if (N == 1) {
    if (in != out)
      memcpy(out, in, M * sizeof(*out));
    return;
  }

  const double norm = 1.0 / sqrt((double)N);
  const double sign = inverse ? 1.0 : -1.0;
  int16_t wr[NR_MOTFS_MAX_N][NR_MOTFS_MAX_N];
  int16_t wi[NR_MOTFS_MAX_N][NR_MOTFS_MAX_N];

  for (uint8_t dst_t = 0; dst_t < N; dst_t++) {
    for (uint8_t src_t = 0; src_t < N; src_t++) {
      const double phase = sign * 2.0 * M_PI * (double)dst_t * (double)src_t / (double)N;
      wr[dst_t][src_t] = nr_motfs_coeff(norm * cos(phase));
      wi[dst_t][src_t] = nr_motfs_coeff(norm * sin(phase));
    }
  }

  for (uint8_t dst_t = 0; dst_t < N; dst_t++) {
    for (uint16_t m = 0; m < M; m++) {
      int64_t acc_r = 0;
      int64_t acc_i = 0;

      for (uint8_t src_t = 0; src_t < N; src_t++) {
        const c16_t x = in[src_t * M + m];

        acc_r += (int64_t)x.r * wr[dst_t][src_t] - (int64_t)x.i * wi[dst_t][src_t];
        acc_i += (int64_t)x.r * wi[dst_t][src_t] + (int64_t)x.i * wr[dst_t][src_t];
      }

      out[dst_t * M + m].r = nr_motfs_saturate16(nr_motfs_round_shift_q15(acc_r));
      out[dst_t * M + m].i = nr_motfs_saturate16(nr_motfs_round_shift_q15(acc_i));
    }
  }
}

// !!!!!!!!! for modified OTFS !!!!!!!!!
int nr_motfs_time_precoding(const c16_t *in, c16_t *out, uint16_t M, uint8_t N)
{
  if (!nr_motfs_valid_args(in, out, M, N))
    return -1;

  nr_motfs_unitary_transform(in, out, M, N, true);
  return 0;
}

// !!!!!!!!! for modified OTFS !!!!!!!!!
int nr_motfs_time_deprecoding(const c16_t *in, c16_t *out, uint16_t M, uint8_t N)
{
  if (!nr_motfs_valid_args(in, out, M, N))
    return -1;

  nr_motfs_unitary_transform(in, out, M, N, false);
  return 0;
}
