/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "nr_ddchan.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static int16_t nr_ddchan_saturate16(nr_ddchan_state_t *state, double x)
{
  const long long y = llround(x);
  if (y > INT16_MAX) {
    state->saturation_count++;
    return INT16_MAX;
  }
  if (y < INT16_MIN) {
    state->saturation_count++;
    return INT16_MIN;
  }
  return (int16_t)y;
}

static double nr_ddchan_phase_from_sample(double doppler_hz, double sample_rate_hz, double sample_index)
{
  return remainder(2.0 * M_PI * doppler_hz * sample_index / sample_rate_hz, 2.0 * M_PI);
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static bool nr_ddchan_streq(const char *a, const char *b)
{
  if (a == NULL || b == NULL)
    return false;
  while (*a != '\0' && *b != '\0') {
    if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
      return false;
    a++;
    b++;
  }
  return *a == '\0' && *b == '\0';
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static void nr_ddchan_set_step(nr_ddchan_path_t *path, double sample_rate_hz)
{
  const double step_phase = nr_ddchan_phase_from_sample(path->doppler_hz, sample_rate_hz, 1.0);
  path->step_r = cos(step_phase);
  path->step_i = sin(step_phase);
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static int nr_ddchan_delay_samples(double delay_ns, double sample_rate_hz)
{
  const double samples = delay_ns * 1e-9 * sample_rate_hz;
  if (samples < 0.0 || samples > (double)INT_MAX)
    return -1;
  return (int)llround(samples);
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static int nr_ddchan_add_path_linear(nr_ddchan_state_t *state,
                                     double delay_ns,
                                     int delay_samples,
                                     double doppler_hz,
                                     double gain_r,
                                     double gain_i)
{
  if (state->num_paths >= NR_DDCHAN_MAX_PATHS)
    return -1;

  if (delay_samples < 0)
    delay_samples = nr_ddchan_delay_samples(delay_ns, state->sample_rate_hz);
  if (delay_samples < 0)
    return -1;

  nr_ddchan_path_t *path = &state->paths[state->num_paths++];
  *path = (nr_ddchan_path_t){
      .delay_samples = delay_samples,
      .delay_ns = delay_ns,
      .doppler_hz = doppler_hz,
      .gain_r = gain_r,
      .gain_i = gain_i,
  };
  nr_ddchan_set_step(path, state->sample_rate_hz);
  if (delay_samples > state->max_delay_samples)
    state->max_delay_samples = delay_samples;
  return 0;
}

static int nr_ddchan_add_path(nr_ddchan_state_t *state,
                              double delay_ns,
                              int delay_samples,
                              double doppler_hz,
                              double gain_db,
                              double phase_deg)
{
  const double gain = pow(10.0, gain_db / 20.0);
  const double phase = phase_deg * M_PI / 180.0;
  return nr_ddchan_add_path_linear(state, delay_ns, delay_samples, doppler_hz, gain * cos(phase), gain * sin(phase));
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static int nr_ddchan_parse_paths(nr_ddchan_state_t *state, const char *paths, int max_paths)
{
  if (paths == NULL || paths[0] == '\0')
    return -1;

  char buf[1024];
  if (strlen(paths) >= sizeof(buf))
    return -1;
  snprintf(buf, sizeof(buf), "%s", paths);

  char *saveptr = NULL;
  char *tok = strtok_r(buf, ";", &saveptr);
  while (tok != NULL) {
    double delay_ns = 0.0;
    double doppler_hz = 0.0;
    double gain_db = 0.0;
    double phase_deg = 0.0;
    char extra = '\0';
    if (state->num_paths >= max_paths)
      return -1;
    if (sscanf(tok, " %lf , %lf , %lf , %lf %c", &delay_ns, &doppler_hz, &gain_db, &phase_deg, &extra) != 4)
      return -1;
    if (nr_ddchan_add_path(state, delay_ns, -1, doppler_hz, gain_db, phase_deg) != 0)
      return -1;
    tok = strtok_r(NULL, ";", &saveptr);
  }

  return state->num_paths > 0 ? 0 : -1;
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static void nr_ddchan_normalize_paths(nr_ddchan_state_t *state)
{
  double power = 0.0;
  for (int i = 0; i < state->num_paths; i++)
    power += state->paths[i].gain_r * state->paths[i].gain_r + state->paths[i].gain_i * state->paths[i].gain_i;
  if (power <= 0.0)
    return;

  const double scale = 1.0 / sqrt(power);
  for (int i = 0; i < state->num_paths; i++) {
    state->paths[i].gain_r *= scale;
    state->paths[i].gain_i *= scale;
  }
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static void nr_ddchan_free_buffers(nr_ddchan_state_t *state)
{
  if (state == NULL)
    return;

  if (state->delay_line != NULL) {
    for (int aa = 0; aa < state->nb_antennas; aa++)
      free(state->delay_line[aa]);
    free(state->delay_line);
  }
  if (state->input_work != NULL) {
    for (int aa = 0; aa < state->nb_antennas; aa++)
      free(state->input_work[aa]);
    free(state->input_work);
  }
  if (state->output_work != NULL) {
    for (int aa = 0; aa < state->nb_antennas; aa++)
      free(state->output_work[aa]);
    free(state->output_work);
  }

  state->delay_line = NULL;
  state->input_work = NULL;
  state->output_work = NULL;
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static int nr_ddchan_alloc_buffers(nr_ddchan_state_t *state)
{
  if (state->nb_antennas <= 0 || state->max_block_samples <= 0)
    return -1;

  state->delay_line = calloc(state->nb_antennas, sizeof(*state->delay_line));
  state->input_work = calloc(state->nb_antennas, sizeof(*state->input_work));
  state->output_work = calloc(state->nb_antennas, sizeof(*state->output_work));
  if (state->delay_line == NULL || state->input_work == NULL || state->output_work == NULL)
    return -1;

  for (int aa = 0; aa < state->nb_antennas; aa++) {
    if (state->max_delay_samples > 0) {
      state->delay_line[aa] = calloc(state->max_delay_samples, sizeof(**state->delay_line));
      if (state->delay_line[aa] == NULL)
        return -1;
    }
    state->input_work[aa] = calloc(state->max_block_samples, sizeof(**state->input_work));
    state->output_work[aa] = calloc(state->max_block_samples, sizeof(**state->output_work));
    if (state->input_work[aa] == NULL || state->output_work[aa] == NULL)
      return -1;
  }
  return 0;
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
int nr_ddchan_init(nr_ddchan_state_t *state, const nr_ddchan_config_t *cfg)
{
  if (state == NULL || cfg == NULL)
    return -1;

  *state = (nr_ddchan_state_t){0};
  state->enabled = cfg->enabled;
  state->apply_on_gnb_rx = cfg->apply_on_gnb_rx;
  state->use_timestamp_phase = cfg->use_timestamp_phase;
  // to bypass the RA procedure
  state->start_after_samples = cfg->start_after_ms > 0.0 ? (uint64_t)ceil(cfg->start_after_ms * 1e-3 * cfg->sample_rate_hz) : 0;
  state->awgn_enable = cfg->awgn_enable;
  state->sample_rate_hz = cfg->sample_rate_hz;
  state->snr_db = cfg->snr_db;
  state->fixed_signal_power = cfg->fixed_signal_power;
  state->noise_rng_state = cfg->noise_seed != 0 ? cfg->noise_seed : 1;
  state->use_measured_noise_power = !nr_ddchan_streq(cfg->noise_power_mode, "fixed");
  state->nb_antennas = cfg->nb_antennas;
  state->max_block_samples = cfg->max_block_samples;

  if (!state->enabled || !state->apply_on_gnb_rx)
    return 0;
  if (state->sample_rate_hz <= 0.0 || state->nb_antennas <= 0 || state->max_block_samples <= 0)
    return -1;

  int max_paths = cfg->max_paths > 0 ? cfg->max_paths : NR_DDCHAN_MAX_PATHS;
  if (max_paths > NR_DDCHAN_MAX_PATHS)
    max_paths = NR_DDCHAN_MAX_PATHS;

  const bool multipath = nr_ddchan_streq(cfg->mode, "multipath") || (cfg->paths != NULL && cfg->paths[0] != '\0');
  if (multipath) {
    if (nr_ddchan_parse_paths(state, cfg->paths, max_paths) != 0)
      return -1;
  } else {
    const int delay_samples = cfg->delay_samples >= 0 ? cfg->delay_samples : -1;
    if (nr_ddchan_add_path_linear(state, cfg->delay_ns, delay_samples, cfg->doppler_hz, cfg->amplitude, 0.0) != 0)
      return -1;
  }

  if (cfg->normalize_power)
    nr_ddchan_normalize_paths(state);

  if (nr_ddchan_alloc_buffers(state) != 0) {
    nr_ddchan_free_buffers(state);
    return -1;
  }

  return 0;
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
void nr_ddchan_free(nr_ddchan_state_t *state)
{
  if (state == NULL)
    return;
  nr_ddchan_free_buffers(state);
  *state = (nr_ddchan_state_t){0};
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
bool nr_ddchan_is_active(const nr_ddchan_state_t *state)
{
  return state != NULL && state->enabled && state->apply_on_gnb_rx && state->sample_rate_hz > 0.0 && state->num_paths > 0;
}

// to bypass the RA procedure
static bool nr_ddchan_should_bypass_for_ra(nr_ddchan_state_t *state, int64_t rx_timestamp)
{
  if (state->start_after_samples == 0)
    return false;

  if (!state->first_rx_timestamp_set) {
    state->first_rx_timestamp = rx_timestamp;
    state->first_rx_timestamp_set = true;
  }

  if (rx_timestamp < state->first_rx_timestamp)
    return false;

  return (uint64_t)(rx_timestamp - state->first_rx_timestamp) < state->start_after_samples;
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static bool nr_ddchan_is_exact_identity(const nr_ddchan_state_t *state)
{
  if (state == NULL || state->num_paths != 1 || state->awgn_enable)
    return false;

  const nr_ddchan_path_t *path = &state->paths[0];
  return path->delay_samples == 0 && path->doppler_hz == 0.0 && path->gain_r == 1.0 && path->gain_i == 0.0;
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static c16_t nr_ddchan_get_delayed_sample(const nr_ddchan_state_t *state, int aa, const c16_t *x, int n, int delay_samples)
{
  if (delay_samples == 0)
    return x[n];
  if (n >= delay_samples)
    return x[n - delay_samples];

  const int hist_idx = state->max_delay_samples + n - delay_samples;
  return state->delay_line[aa][hist_idx];
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static void nr_ddchan_update_delay_line(nr_ddchan_state_t *state, int aa, const c16_t *x, int nsamps)
{
  if (state->max_delay_samples == 0)
    return;

  if (nsamps >= state->max_delay_samples) {
    memcpy(state->delay_line[aa], &x[nsamps - state->max_delay_samples], state->max_delay_samples * sizeof(*x));
  } else {
    memmove(state->delay_line[aa],
            &state->delay_line[aa][nsamps],
            (state->max_delay_samples - nsamps) * sizeof(*state->delay_line[aa]));
    memcpy(&state->delay_line[aa][state->max_delay_samples - nsamps], x, nsamps * sizeof(*x));
  }
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static uint64_t nr_ddchan_rng_next(nr_ddchan_state_t *state)
{
  uint64_t x = state->noise_rng_state;
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  state->noise_rng_state = x;
  return x * UINT64_C(2685821657736338717);
}

static double nr_ddchan_uniform_open(nr_ddchan_state_t *state)
{
  const uint64_t x = nr_ddchan_rng_next(state);
  return ((double)(x >> 11) + 0.5) * (1.0 / 9007199254740992.0);
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static double nr_ddchan_normal(nr_ddchan_state_t *state)
{
  double u;
  double v;
  double s;
  do {
    u = 2.0 * nr_ddchan_uniform_open(state) - 1.0;
    v = 2.0 * nr_ddchan_uniform_open(state) - 1.0;
    s = u * u + v * v;
  } while (s <= 0.0 || s >= 1.0);
  return u * sqrt(-2.0 * log(s) / s);
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
static void nr_ddchan_apply_awgn(nr_ddchan_state_t *state, int nsamps)
{
  if (!state->awgn_enable)
    return;

  const double signal_power = state->use_measured_noise_power ? state->last_signal_power : state->fixed_signal_power;
  if (signal_power <= 0.0)
    return;

  const double gamma = pow(10.0, state->snr_db / 10.0);
  if (gamma <= 0.0)
    return;

  const double sigma_iq = sqrt((signal_power / gamma) * 0.5);
  for (int aa = 0; aa < state->nb_antennas; aa++) {
    for (int n = 0; n < nsamps; n++) {
      c16_t *y = &state->output_work[aa][n];
      y->r = nr_ddchan_saturate16(state, (double)y->r + sigma_iq * nr_ddchan_normal(state));
      y->i = nr_ddchan_saturate16(state, (double)y->i + sigma_iq * nr_ddchan_normal(state));
    }
  }
}

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
int nr_ddchan_apply_gnb_rx(nr_ddchan_state_t *state, c16_t **rxdata, int nb_antennas, int nsamps, int64_t rx_timestamp)
{
  if (state == NULL || rxdata == NULL || nb_antennas < 0 || nsamps < 0)
    return -1;
  if (!nr_ddchan_is_active(state))
    return 0;
  // to bypass the RA procedure
  if (nr_ddchan_should_bypass_for_ra(state, rx_timestamp)) {
    for (int aa = 0; aa < nb_antennas; aa++)
      nr_ddchan_update_delay_line(state, aa, rxdata[aa], nsamps);
    if (!state->use_timestamp_phase)
      state->absolute_sample_counter += (uint64_t)nsamps;
    return 0;
  }
  if (nr_ddchan_is_exact_identity(state))
    return 0;
  if (nb_antennas != state->nb_antennas || nsamps > state->max_block_samples)
    return -1;
  for (int aa = 0; aa < nb_antennas; aa++)
    if (rxdata[aa] == NULL)
      return -1;

  for (int aa = 0; aa < nb_antennas; aa++)
    memcpy(state->input_work[aa], rxdata[aa], nsamps * sizeof(*rxdata[aa]));

  const double sample_index = state->use_timestamp_phase ? (double)rx_timestamp : (double)state->absolute_sample_counter;
  double signal_power_sum = 0.0;
  uint64_t signal_power_count = 0;

  for (int aa = 0; aa < nb_antennas; aa++) {
    const c16_t *x = state->input_work[aa];
    c16_t *y = state->output_work[aa];
    double osc_r[NR_DDCHAN_MAX_PATHS];
    double osc_i[NR_DDCHAN_MAX_PATHS];

    for (int p = 0; p < state->num_paths; p++) {
      const nr_ddchan_path_t *path = &state->paths[p];
      if (path->doppler_hz == 0.0) {
        osc_r[p] = 1.0;
        osc_i[p] = 0.0;
      } else {
        const double phase = nr_ddchan_phase_from_sample(path->doppler_hz, state->sample_rate_hz, sample_index);
        osc_r[p] = cos(phase);
        osc_i[p] = sin(phase);
      }
    }

    for (int n = 0; n < nsamps; n++) {
      double acc_r = 0.0;
      double acc_i = 0.0;

      for (int p = 0; p < state->num_paths; p++) {
        const nr_ddchan_path_t *path = &state->paths[p];
        const c16_t xd = nr_ddchan_get_delayed_sample(state, aa, x, n, path->delay_samples);
        const double rot_r = (double)xd.r * osc_r[p] - (double)xd.i * osc_i[p];
        const double rot_i = (double)xd.r * osc_i[p] + (double)xd.i * osc_r[p];
        acc_r += rot_r * path->gain_r - rot_i * path->gain_i;
        acc_i += rot_r * path->gain_i + rot_i * path->gain_r;

        const double next_r = osc_r[p] * path->step_r - osc_i[p] * path->step_i;
        const double next_i = osc_r[p] * path->step_i + osc_i[p] * path->step_r;
        osc_r[p] = next_r;
        osc_i[p] = next_i;
      }

      y[n].r = nr_ddchan_saturate16(state, acc_r);
      y[n].i = nr_ddchan_saturate16(state, acc_i);
      signal_power_sum += acc_r * acc_r + acc_i * acc_i;
      signal_power_count++;
    }
    nr_ddchan_update_delay_line(state, aa, x, nsamps);
  }

  state->last_signal_power = signal_power_count > 0 ? signal_power_sum / (double)signal_power_count : 0.0;
  nr_ddchan_apply_awgn(state, nsamps);

  for (int aa = 0; aa < nb_antennas; aa++)
    memcpy(rxdata[aa], state->output_work[aa], nsamps * sizeof(*rxdata[aa]));

  if (!state->use_timestamp_phase)
    state->absolute_sample_counter += (uint64_t)nsamps;

  return 0;
}
