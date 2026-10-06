/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

// for the metric
#include "nr_phy_metric_trace.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "common/utils/LOG/log.h"

// for the metric
_Static_assert(sizeof(nr_phy_metric_record_hdr_t) == 66, "Unexpected metric record header ABI size");

// for the metric
static nr_phy_metric_mode_t nr_phy_metric_parse_mode(const char *mode)
{
  if (mode == NULL || strcasecmp(mode, "symbol") == 0)
    return NR_PHY_METRIC_MODE_SYMBOL;
  if (strcasecmp(mode, "summary") == 0)
    return NR_PHY_METRIC_MODE_SUMMARY;
  if (strcasecmp(mode, "bit") == 0)
    return NR_PHY_METRIC_MODE_BIT;
  if (strcasecmp(mode, "full") == 0)
    return NR_PHY_METRIC_MODE_FULL;
  return NR_PHY_METRIC_MODE_SYMBOL;
}

// for the metric
const char *nr_phy_metric_mode_name(nr_phy_metric_mode_t mode)
{
  switch (mode) {
    case NR_PHY_METRIC_MODE_SUMMARY:
      return "summary";
    case NR_PHY_METRIC_MODE_SYMBOL:
      return "symbol";
    case NR_PHY_METRIC_MODE_BIT:
      return "bit";
    case NR_PHY_METRIC_MODE_FULL:
      return "full";
    default:
      return "symbol";
  }
}

// for the metric
static uint16_t nr_phy_metric_parse_rnti_filter(const char *filter)
{
  if (filter == NULL || filter[0] == '\0')
    return 0;

  char *end = NULL;
  errno = 0;
  unsigned long value = strtoul(filter, &end, 0);
  if (errno != 0 || end == filter || *end != '\0' || value > UINT16_MAX)
    return 0;
  return (uint16_t)value;
}

// for the metric
static int nr_phy_metric_mkdirs(const char *dir)
{
  if (dir == NULL || dir[0] == '\0')
    return -1;

  char tmp[NR_PHY_METRIC_MAX_PATH];
  size_t len = strnlen(dir, sizeof(tmp));
  if (len == 0 || len >= sizeof(tmp))
    return -1;

  memcpy(tmp, dir, len + 1);
  while (len > 1 && tmp[len - 1] == '/') {
    tmp[len - 1] = '\0';
    len--;
  }

  for (char *p = tmp + 1; *p != '\0'; p++) {
    if (*p != '/')
      continue;
    *p = '\0';
    if (mkdir(tmp, 0775) != 0 && errno != EEXIST)
      return -1;
    *p = '/';
  }

  if (mkdir(tmp, 0775) != 0 && errno != EEXIST)
    return -1;
  return 0;
}

// for the metric
static void nr_phy_metric_disable(nr_phy_metric_state_t *state)
{
  if (state == NULL)
    return;
  state->enabled = false;
  if (state->file != NULL) {
    fclose(state->file);
    state->file = NULL;
  }
}

// for the metric
int nr_phy_metric_init(nr_phy_metric_state_t *state, const nr_phy_metric_config_t *cfg)
{
  if (state == NULL || cfg == NULL)
    return -1;

  nr_phy_metric_close(state);
  memset(state, 0, sizeof(*state));
  state->configured = true;
  state->enabled = cfg->enabled != 0;
  state->mode = nr_phy_metric_parse_mode(cfg->mode);
  state->rv0_only = cfg->rv0_only != 0;
  state->new_tx_only = cfg->new_tx_only != 0;
  state->rnti_filter = nr_phy_metric_parse_rnti_filter(cfg->rnti_filter);
  state->skip_ra = cfg->skip_ra != 0;
  state->max_records = cfg->max_records > 0 ? (uint32_t)cfg->max_records : 0;
  state->drop_when_full = cfg->drop_when_full != 0;

  const char *dump_dir = cfg->dump_dir != NULL && cfg->dump_dir[0] != '\0' ? cfg->dump_dir : "/tmp/oai_metrics";
  snprintf(state->dump_dir, sizeof(state->dump_dir), "%s", dump_dir);

  if (!state->enabled)
    return 0;

  if (pthread_mutex_init(&state->lock, NULL) != 0) {
    state->enabled = false;
    LOG_W(PHY, "[METRIC][gNB] disabled: failed to initialize lock\n");
    return -1;
  }
  state->lock_initialized = true;

  if (nr_phy_metric_mkdirs(state->dump_dir) != 0) {
    state->enabled = false;
    LOG_W(PHY, "[METRIC][gNB] disabled: cannot create dump directory %s\n", state->dump_dir);
    return -1;
  }

  snprintf(state->dump_path, sizeof(state->dump_path), "%s/gnb_phy_trace_%ld.bin", state->dump_dir, (long)getpid());
  state->file = fopen(state->dump_path, "wb");
  if (state->file == NULL) {
    state->enabled = false;
    LOG_W(PHY, "[METRIC][gNB] disabled: cannot open %s\n", state->dump_path);
    return -1;
  }

  state->stdio_buffer = malloc(1 << 20);
  if (state->stdio_buffer != NULL)
    setvbuf(state->file, state->stdio_buffer, _IOFBF, 1 << 20);

  LOG_I(PHY, "[METRIC][gNB] enabled mode=%s dump=%s\n", nr_phy_metric_mode_name(state->mode), state->dump_path);
  return 0;
}

// for the metric
void nr_phy_metric_close(nr_phy_metric_state_t *state)
{
  if (state == NULL)
    return;

  if (state->file != NULL) {
    fclose(state->file);
    state->file = NULL;
  }
  free(state->stdio_buffer);
  state->stdio_buffer = NULL;
  if (state->lock_initialized) {
    pthread_mutex_destroy(&state->lock);
    state->lock_initialized = false;
  }
  state->enabled = false;
}

// for the metric
bool nr_phy_metric_is_enabled(const nr_phy_metric_state_t *state)
{
  return state != NULL && state->enabled && state->file != NULL;
}

// for the metric
bool nr_phy_metric_should_trace_gnb_rx_qam(const nr_phy_metric_state_t *state, const nr_phy_metric_pusch_ctx_t *ctx)
{
  if (!nr_phy_metric_is_enabled(state) || ctx == NULL)
    return false;
  if (state->mode != NR_PHY_METRIC_MODE_SYMBOL && state->mode != NR_PHY_METRIC_MODE_FULL)
    return false;
  if (state->rv0_only && ctx->rv != 0)
    return false;
  if (state->new_tx_only && ctx->ndi == 0)
    return false;
  if (state->rnti_filter != 0 && state->rnti_filter != ctx->rnti)
    return false;
  if (ctx->n_qam == 0 || ctx->M == 0 || ctx->N == 0)
    return false;
  if (state->max_records != 0 && state->record_index >= state->max_records)
    return false;
  return true;
}

// for the metric
void nr_phy_metric_trace_gnb_rx_qam(nr_phy_metric_state_t *state, const nr_phy_metric_pusch_ctx_t *ctx, const c16_t *payload)
{
  if (!nr_phy_metric_should_trace_gnb_rx_qam(state, ctx) || payload == NULL)
    return;

  nr_phy_metric_record_hdr_t hdr = {
      .magic = NR_PHY_METRIC_MAGIC,
      .version = NR_PHY_METRIC_VERSION,
      .header_bytes = sizeof(nr_phy_metric_record_hdr_t),
      .producer = NR_PHY_METRIC_PRODUCER_GNB,
      .record_type = NR_PHY_METRIC_REC_GNB_RX_QAM,
      .waveform = ctx->waveform,
      .payload_type = NR_PHY_METRIC_PAYLOAD_C16,
      .frame = ctx->frame,
      .slot = ctx->slot,
      .rnti = ctx->rnti,
      .harq_id = ctx->harq_id,
      .rv = ctx->rv,
      .ndi = ctx->ndi,
      .qam_mod_order = ctx->qam_mod_order,
      .mcs_index = ctx->mcs_index,
      .nr_layers = ctx->nr_layers,
      .transform_precoding = ctx->transform_precoding,
      .rb_start = ctx->rb_start,
      .rb_size = ctx->rb_size,
      .start_symbol = ctx->start_symbol,
      .nr_symbols = ctx->nr_symbols,
      .ul_dmrs_symb_pos = ctx->ul_dmrs_symb_pos,
      .dmrs_config_type = ctx->dmrs_config_type,
      .num_dmrs_cdm_grps_no_data = ctx->num_dmrs_cdm_grps_no_data,
      .frequency_hopping = ctx->frequency_hopping,
      .M = ctx->M,
      .N = ctx->N,
      .G = ctx->G,
      .n_qam = ctx->n_qam,
      .tb_size = ctx->tb_size,
      .payload_bytes = ctx->n_qam * sizeof(c16_t),
  };

  pthread_mutex_lock(&state->lock);
  if (state->max_records != 0 && state->record_index >= state->max_records) {
    state->dropped_records++;
    pthread_mutex_unlock(&state->lock);
    return;
  }

  hdr.record_index = state->record_index;
  const bool ok = fwrite(&hdr, sizeof(hdr), 1, state->file) == 1
                  && fwrite(payload, sizeof(c16_t), ctx->n_qam, state->file) == ctx->n_qam;
  if (ok)
    state->record_index++;
  else
    nr_phy_metric_disable(state);
  pthread_mutex_unlock(&state->lock);
}
