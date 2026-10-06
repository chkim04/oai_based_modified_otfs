/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

// for the metric
#ifndef __NR_PHY_METRIC_TRACE_H__
#define __NR_PHY_METRIC_TRACE_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include "common/platform_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// for the metric
#define NR_PHY_METRIC_MAGIC 0x4f41494d
#define NR_PHY_METRIC_VERSION 1
#define NR_PHY_METRIC_MAX_PATH 512

// for the metric
typedef enum {
  NR_PHY_METRIC_PRODUCER_UE = 1,
  NR_PHY_METRIC_PRODUCER_GNB = 2
} nr_phy_metric_producer_t;

// for the metric
typedef enum {
  NR_PHY_METRIC_WAVEFORM_DFT_S_OFDM = 0,
  NR_PHY_METRIC_WAVEFORM_MOTFS = 1
} nr_phy_metric_waveform_t;

// for the metric
typedef enum {
  NR_PHY_METRIC_REC_UE_TX_QAM = 1,
  NR_PHY_METRIC_REC_UE_TX_MODULATION_BITS = 2,
  NR_PHY_METRIC_REC_GNB_RX_QAM = 3,
  NR_PHY_METRIC_REC_GNB_RX_HARDBITS = 4,
  NR_PHY_METRIC_REC_GNB_RX_LLR = 5,
  NR_PHY_METRIC_REC_SUMMARY = 6
} nr_phy_metric_record_type_t;

// for the metric
typedef enum {
  NR_PHY_METRIC_PAYLOAD_C16 = 1,
  NR_PHY_METRIC_PAYLOAD_U8 = 2,
  NR_PHY_METRIC_PAYLOAD_I16 = 3,
  NR_PHY_METRIC_PAYLOAD_I32 = 4,
  NR_PHY_METRIC_PAYLOAD_NONE = 5
} nr_phy_metric_payload_type_t;

// for the metric
typedef enum {
  NR_PHY_METRIC_MODE_SUMMARY = 0,
  NR_PHY_METRIC_MODE_SYMBOL = 1,
  NR_PHY_METRIC_MODE_BIT = 2,
  NR_PHY_METRIC_MODE_FULL = 3
} nr_phy_metric_mode_t;

// for the metric
typedef struct __attribute__((packed)) {
  uint32_t magic;
  uint16_t version;
  uint16_t header_bytes;

  uint8_t producer;
  uint8_t record_type;
  uint8_t waveform;
  uint8_t payload_type;

  uint16_t frame;
  uint16_t slot;
  uint16_t rnti;
  uint8_t harq_id;
  uint8_t rv;
  uint8_t ndi;
  uint8_t qam_mod_order;

  uint8_t mcs_index;
  uint8_t nr_layers;
  uint8_t transform_precoding;
  uint8_t reserved0;

  uint16_t rb_start;
  uint16_t rb_size;
  uint8_t start_symbol;
  uint8_t nr_symbols;
  uint16_t ul_dmrs_symb_pos;

  uint8_t dmrs_config_type;
  uint8_t num_dmrs_cdm_grps_no_data;
  uint8_t frequency_hopping;
  uint8_t reserved1;

  uint16_t M;
  uint16_t N;
  uint32_t G;
  uint32_t n_qam;
  uint32_t tb_size;

  uint32_t payload_bytes;
  uint64_t record_index;
} nr_phy_metric_record_hdr_t;

// for the metric
typedef struct {
  int enabled;
  const char *dump_dir;
  const char *mode;
  int rv0_only;
  int new_tx_only;
  const char *rnti_filter;
  int skip_ra;
  int max_records;
  int drop_when_full;
} nr_phy_metric_config_t;

// for the metric
typedef struct {
  uint16_t frame;
  uint16_t slot;
  uint16_t rnti;
  uint8_t harq_id;
  uint8_t rv;
  uint8_t ndi;
  uint8_t qam_mod_order;
  uint8_t mcs_index;
  uint8_t nr_layers;
  uint8_t transform_precoding;
  uint16_t rb_start;
  uint16_t rb_size;
  uint8_t start_symbol;
  uint8_t nr_symbols;
  uint16_t ul_dmrs_symb_pos;
  uint8_t dmrs_config_type;
  uint8_t num_dmrs_cdm_grps_no_data;
  uint8_t frequency_hopping;
  uint16_t M;
  uint16_t N;
  uint32_t G;
  uint32_t n_qam;
  uint32_t tb_size;
  nr_phy_metric_waveform_t waveform;
} nr_phy_metric_pusch_ctx_t;

// for the metric
typedef struct {
  bool configured;
  bool enabled;
  bool lock_initialized;
  nr_phy_metric_mode_t mode;
  bool rv0_only;
  bool new_tx_only;
  uint16_t rnti_filter;
  bool skip_ra;
  uint32_t max_records;
  bool drop_when_full;
  uint64_t record_index;
  uint64_t dropped_records;
  FILE *file;
  char dump_dir[NR_PHY_METRIC_MAX_PATH];
  char dump_path[NR_PHY_METRIC_MAX_PATH];
  char *stdio_buffer;
  pthread_mutex_t lock;
} nr_phy_metric_state_t;

// for the metric
int nr_phy_metric_init(nr_phy_metric_state_t *state, const nr_phy_metric_config_t *cfg);
void nr_phy_metric_close(nr_phy_metric_state_t *state);
bool nr_phy_metric_is_enabled(const nr_phy_metric_state_t *state);
bool nr_phy_metric_should_trace_gnb_rx_qam(const nr_phy_metric_state_t *state, const nr_phy_metric_pusch_ctx_t *ctx);
void nr_phy_metric_trace_gnb_rx_qam(nr_phy_metric_state_t *state, const nr_phy_metric_pusch_ctx_t *ctx, const c16_t *payload);
const char *nr_phy_metric_mode_name(nr_phy_metric_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* __NR_PHY_METRIC_TRACE_H__ */
