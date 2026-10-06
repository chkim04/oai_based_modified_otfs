/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef __NR_DDCHAN_H__
#define __NR_DDCHAN_H__

#include <stdbool.h>
#include <stdint.h>
#include "common/platform_types.h"

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
#define NR_DDCHAN_MAX_PATHS 8

#ifdef __cplusplus
extern "C" {
#endif

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
typedef struct nr_ddchan_path_s {
  int delay_samples;
  double delay_ns;
  double doppler_hz;
  double gain_r;
  double gain_i;
  double step_r;
  double step_i;
} nr_ddchan_path_t;

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
typedef struct nr_ddchan_config_s {
  bool enabled;
  bool apply_on_gnb_rx;
  bool use_timestamp_phase;
  // to bypass the RA procedure
  double start_after_ms;
  const char *mode;
  double doppler_hz;
  double amplitude;
  double delay_ns;
  int delay_samples;
  const char *paths;
  bool normalize_power;
  int max_paths;
  bool awgn_enable;
  double snr_db;
  uint64_t noise_seed;
  const char *noise_power_mode;
  double fixed_signal_power;
  double sample_rate_hz;
  int nb_antennas;
  int max_block_samples;
} nr_ddchan_config_t;

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
typedef struct nr_ddchan_state_s {
  bool enabled;
  bool apply_on_gnb_rx;
  bool use_timestamp_phase;
  // to bypass the RA procedure
  uint64_t start_after_samples;
  bool first_rx_timestamp_set;
  int64_t first_rx_timestamp;
  bool awgn_enable;
  bool use_measured_noise_power;
  double sample_rate_hz;
  double snr_db;
  double fixed_signal_power;
  double last_signal_power;
  uint64_t noise_rng_state;
  uint64_t absolute_sample_counter;
  uint64_t saturation_count;
  int nb_antennas;
  int max_block_samples;
  int max_delay_samples;
  int num_paths;
  nr_ddchan_path_t paths[NR_DDCHAN_MAX_PATHS];
  c16_t **delay_line;
  c16_t **input_work;
  c16_t **output_work;
} nr_ddchan_state_t;

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
int nr_ddchan_init(nr_ddchan_state_t *state, const nr_ddchan_config_t *cfg);

void nr_ddchan_free(nr_ddchan_state_t *state);

bool nr_ddchan_is_active(const nr_ddchan_state_t *state);

int nr_ddchan_apply_gnb_rx(nr_ddchan_state_t *state, c16_t **rxdata, int nb_antennas, int nsamps, int64_t rx_timestamp);

#ifdef __cplusplus
}
#endif

#endif /* __NR_DDCHAN_H__ */
