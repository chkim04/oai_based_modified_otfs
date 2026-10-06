/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief Top-level threads for eNodeB
 */
#ifndef SOFTMODEM_COMMON_H
#define SOFTMODEM_COMMON_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "common/config/config_load_configmodule.h"

/* help strings definition for command line options, used in CMDLINE_XXX_DESC macros and printed when -h option is used */
#define CONFIG_HLP_RFCFGF        "Configuration file for front-end (e.g. LMS7002M)\n"
#define CONFIG_HLP_TPOOL \
  "Thread pool configuration: \n\
  list of cores, comma separated (negative value is no core affinity)\n\
  example: -1,3 launches two working threads one floating, the second set on core 3\n\
  default 8 floating threads\n\
  use N for no pool (runs in calling thread) recommended with rfsim.\n"
#define CONFIG_HLP_CALUER        "set UE RX calibration\n"
#define CONFIG_HLP_CALUERM       ""
#define CONFIG_HLP_CALUERB       ""
#define CONFIG_HLP_DBGUEPR       "UE run normal prach power ramping, but don't continue random-access\n"
#define CONFIG_HLP_CALPRACH      "UE run normal prach with maximum power, but don't continue random-access\n"
#define CONFIG_HLP_NOL2CN        "bypass L2 and upper layers\n"


#define CONFIG_HLP_DUMPFRAME     "dump UE received frame to rxsig_frame0.dat and exit\n"
#define CONFIG_HLP_PHYTST        "test UE phy layer, mac disabled\n"
#define CONFIG_HLP_DORA          "test gNB  and UE with RA procedures\n"
#define CONFIG_HLP_SL_MODE       "sets the NR sidelink mode (0: not in sidelink mode, 1: in-coverage/gNB, 2: out-of-coverage/no gNB)\n"
#define CONFIG_HLP_CLK           "tells hardware to use a clock reference (0:internal, 1:external, 2:gpsdo)\n"
#define CONFIG_HLP_TME           "tells hardware to use a time reference (0:internal, 1:external, 2:gpsdo)\n"
#define CONFIG_HLP_TUNE_OFFSET   "LO tuning offset to use in Hz\n"
#define CONFIG_HLP_DLF           "Set the downlink frequency for all component carriers\n"
#define CONFIG_HLP_ULF           "Set the uplink frequency offset for all component carriers\n"
#define CONFIG_HLP_SLF           "Set the sidelink frequency for all component carriers\n"
#define CONFIG_HLP_CHOFF         "Channel id offset\n"
#define CONFIG_HLP_SOFTS         "Enable soft scope and L1 and L2 stats (Xforms)\n"
#define CONFIG_HLP_DLMCS         "Set the maximum downlink MCS\n"
#define CONFIG_HLP_STMON         "Enable processing timing measurement of lte softmodem on per subframe basis \n"
#define CONFIG_HLP_CHESTFREQ     "Set channel estimation type in frequency domain. 0-Linear interpolation (default). 1-PRB based averaging of channel estimates in frequency. \n"
#define CONFIG_HLP_CHESTTIME     "Set channel estimation type in time domain. 0-Symbols take estimates of the last preceding DMRS symbol (default). 1-Symbol based averaging of channel estimates in time. \n"
// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
#define CONFIG_HLP_MOTFS_ENABLE  "Enable experimental gNB PUSCH MOTFS matched receiver. Disabled by default.\n"
#define CONFIG_HLP_DDCHAN_ENABLE "Enable experimental gNB RX digital Doppler channel hook. Disabled by default.\n"
#define CONFIG_HLP_DDCHAN_GNB_RX "Apply experimental digital Doppler channel on gNB RX samples. Disabled by default.\n"
// to bypass the RA procedure
#define CONFIG_HLP_DDCHAN_START_AFTER_MS "Bypass experimental DDCHAN on gNB RX for this many milliseconds after first RX sample. 0 disables delayed start.\n"
#define CONFIG_HLP_DDCHAN_MODE "Experimental digital channel mode: doppler or multipath.\n"
#define CONFIG_HLP_DDCHAN_DOPPLER_HZ "Single-path Doppler shift in Hz for experimental gNB RX digital channel hook.\n"
#define CONFIG_HLP_DDCHAN_AMPLITUDE "Single-path linear amplitude for experimental gNB RX digital channel hook.\n"
#define CONFIG_HLP_DDCHAN_DELAY_NS "Single-path integer delay in ns, rounded to nearest sample.\n"
#define CONFIG_HLP_DDCHAN_DELAY_SAMPLES "Single-path integer delay in samples. Overrides ddchan.delay_ns when non-negative.\n"
#define CONFIG_HLP_DDCHAN_PATHS "Multipath list: delay_ns,doppler_hz,gain_db,phase_deg;...\n"
#define CONFIG_HLP_DDCHAN_NORMALIZE "Normalize configured multipath gains to unit total power.\n"
#define CONFIG_HLP_DDCHAN_MAX_PATHS "Maximum number of configured multipath paths, capped internally.\n"
#define CONFIG_HLP_DDCHAN_AWGN_ENABLE "Enable optional experimental software AWGN after delay-Doppler channel.\n"
#define CONFIG_HLP_DDCHAN_SNR_DB "Target AWGN SNR in dB.\n"
#define CONFIG_HLP_DDCHAN_NOISE_SEED "Deterministic AWGN RNG seed.\n"
#define CONFIG_HLP_DDCHAN_NOISE_POWER_MODE "AWGN signal-power basis: measured or fixed.\n"
#define CONFIG_HLP_DDCHAN_FIXED_SIGNAL_POWER "Fixed signal power for AWGN when ddchan.noise_power_mode=fixed.\n"
// for the metric
#define CONFIG_HLP_METRIC_ENABLE "Enable experimental gNB PHY metric binary tracing. Disabled by default.\n"
// for the metric
#define CONFIG_HLP_METRIC_DUMP_DIR "Directory for experimental gNB PHY metric binary trace files.\n"
// for the metric
#define CONFIG_HLP_METRIC_MODE "Experimental gNB PHY metric mode: summary, symbol, bit, or full.\n"
// for the metric
#define CONFIG_HLP_METRIC_RV0_ONLY "Trace only RV0 PUSCH transmissions.\n"
// for the metric
#define CONFIG_HLP_METRIC_NEW_TX_ONLY "Trace only new PUSCH transmissions when the new-transmission indicator is set.\n"
// for the metric
#define CONFIG_HLP_METRIC_RNTI_FILTER "Trace only this RNTI, using decimal or 0x-prefixed hex. 0 disables filtering.\n"
// for the metric
#define CONFIG_HLP_METRIC_SKIP_RA "Skip RA Msg3 and Msg3 retransmission PUSCH metric records.\n"
// for the metric
#define CONFIG_HLP_METRIC_MAX_RECORDS "Maximum number of metric records to write. 0 means unlimited.\n"
// for the metric
#define CONFIG_HLP_METRIC_DROP_WHEN_FULL "Drop metric records after the record cap is reached.\n"
#define CONFIG_HLP_IMSCOPE       "Enable phy scope based on imgui and implot"
#define CONFIG_HLP_IMSCOPE_RECORD "Enable recording scope data to filesystem"

#define CONFIG_HLP_NONSTOP       "Go back to frame sync mode after 100 consecutive PBCH failures\n"

#define CONFIG_HLP_TQFS                                                                                                          \
  "Apply three-quarter of sampling frequency, (example 23.04 Msps for LTE 20MHz) to reduce the data rate on USB/PCIe transfers " \
  "(only valid for some bandwidths)\n"
#define CONFIG_HLP_TPORT         "tracer port\n"
#define CONFIG_HLP_NOTWAIT       "don't wait for tracer, start immediately\n"

#define CONFIG_HLP_NUMEROLOGY    "adding numerology for 5G\n"
#define CONFIG_HLP_BAND          "band index\n"
#define CONFIG_HLP_PARALLEL_CMD  "three config for level of parallelism 'PARALLEL_SINGLE_THREAD', 'PARALLEL_RU_L1_SPLIT', or 'PARALLEL_RU_L1_TRX_SPLIT'\n"
#define CONFIG_HLP_WORKER_CMD    "two option for worker 'WORKER_DISABLE' or 'WORKER_ENABLE'\n"
#define CONFIG_HLP_USRP_THREAD   "having extra thead for usrp tx\n"

#define CONFIG_HLP_NOS1          "Disable s1 interface\n"
#define CONFIG_HLP_RFSIM         "Run in rf simulator mode\n"
#define CONFIG_HLP_DISABLNBIOT   "disable nb-iot, even if defined in config\n"
#define CONFIG_HLP_USRP_THREAD   "having extra thead for usrp tx\n"
#define CONFIG_HLP_NFAPI         "Change the nFAPI mode for NR 'MONOLITHIC', 'PNF', 'VNF', 'AERIAL','UE_STUB_PNF','UE_STUB_OFFNET','STANDALONE_PNF'\n"
#define CONFIG_HLP_CONTINUOUS_TX "perform continuous transmission, even in TDD mode (to work around USRP issues)\n"
#define CONFIG_HLP_STATS_DISABLE "disable globally the stats generation and persistence"
#define CONFIG_HLP_NOITTI        "Do not start itti threads, call queue processing in place, inside the caller thread"
#define CONFIG_HLP_SYNC_REF      "UE acts a Sync Reference in Sidelink. 0-none 1-GNB 2-GNSS 4-localtiming\n"
#define CONFIG_HLP_TADV                                                                                                      \
  "Set RF board timing_advance to compensate fix delay inside the RF board between Rx and Tx timestamps (RF board internal " \
  "issues)\n"

/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                            command line parameters common to eNodeB and UE                                                          */
/*   optname                 helpstr                  paramflags      XXXptr                              defXXXval              type         numelt   */
/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
#define RF_CONFIG_FILE      softmodem_params.rf_config_file
#define TP_CONFIG           softmodem_params.threadPoolConfig
#define CONTINUOUS_TX       softmodem_params.continuous_tx
#define PHY_TEST            softmodem_params.phy_test
#define DO_RA               softmodem_params.do_ra
#define SL_MODE             softmodem_params.sl_mode
#define CHAIN_OFFSET        softmodem_params.chain_offset
#define NUMEROLOGY          softmodem_params.numerology
#define BAND                softmodem_params.band
#define CLOCK_SOURCE        softmodem_params.clock_source
#define TIMING_SOURCE       softmodem_params.timing_source
#define TUNE_OFFSET         softmodem_params.tune_offset
#define CHEST_FREQ          softmodem_params.chest_freq
#define CHEST_TIME          softmodem_params.chest_time
// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
#define MOTFS_ENABLE        softmodem_params.motfs_enable
#define DDCHAN_ENABLE       softmodem_params.ddchan_enable
#define DDCHAN_GNB_RX       softmodem_params.ddchan_apply_on_gnb_rx
// to bypass the RA procedure
#define DDCHAN_START_AFTER_MS softmodem_params.ddchan_start_after_ms
#define DDCHAN_MODE         softmodem_params.ddchan_mode
#define DDCHAN_DOPPLER_HZ   softmodem_params.ddchan_doppler_hz
#define DDCHAN_AMPLITUDE    softmodem_params.ddchan_amplitude
#define DDCHAN_DELAY_NS     softmodem_params.ddchan_delay_ns
#define DDCHAN_DELAY_SAMPLES softmodem_params.ddchan_delay_samples
#define DDCHAN_PATHS        softmodem_params.ddchan_paths
#define DDCHAN_NORMALIZE    softmodem_params.ddchan_normalize_power
#define DDCHAN_MAX_PATHS    softmodem_params.ddchan_max_paths
#define DDCHAN_AWGN_ENABLE  softmodem_params.ddchan_awgn_enable
#define DDCHAN_SNR_DB       softmodem_params.ddchan_snr_db
#define DDCHAN_NOISE_SEED   softmodem_params.ddchan_noise_seed
#define DDCHAN_NOISE_POWER_MODE softmodem_params.ddchan_noise_power_mode
#define DDCHAN_FIXED_SIGNAL_POWER softmodem_params.ddchan_fixed_signal_power
// for the metric
#define METRIC_ENABLE       softmodem_params.metric_enable
// for the metric
#define METRIC_DUMP_DIR     softmodem_params.metric_dump_dir
// for the metric
#define METRIC_MODE         softmodem_params.metric_mode
// for the metric
#define METRIC_RV0_ONLY     softmodem_params.metric_rv0_only
// for the metric
#define METRIC_NEW_TX_ONLY  softmodem_params.metric_new_tx_only
// for the metric
#define METRIC_RNTI_FILTER  softmodem_params.metric_rnti_filter
// for the metric
#define METRIC_SKIP_RA      softmodem_params.metric_skip_ra
// for the metric
#define METRIC_MAX_RECORDS  softmodem_params.metric_max_records
// for the metric
#define METRIC_DROP_WHEN_FULL softmodem_params.metric_drop_when_full
#define NFAPI               softmodem_params.nfapi
#define NSA                 softmodem_params.nsa
#define NODE_NUMBER         softmodem_params.node_number
#define NON_STOP            softmodem_params.non_stop
#define CONTINUOUS_TX       softmodem_params.continuous_tx
#define SYNC_REF            softmodem_params.sync_ref
#define DEFAULT_PDU_ID      softmodem_params.default_pdu_session_id

extern int usrp_tx_thread;
// clang-format off
#define CMDLINE_PARAMS_DESC {  \
  {"rf-config-file",        CONFIG_HLP_RFCFGF,        0,              .strptr=&RF_CONFIG_FILE,                .defstrval=NULL,          TYPE_STRING, 0},  \
  {"thread-pool",           CONFIG_HLP_TPOOL,         0,              .strptr=&TP_CONFIG,                     .defstrval="-1,-1,-1,-1,-1,-1,-1,-1",  TYPE_STRING, 0},     \
  {"phy-test",              CONFIG_HLP_PHYTST,        PARAMFLAG_BOOL, .iptr=&PHY_TEST,                        .defintval=0,             TYPE_INT,    0},  \
  {"do-ra",                 CONFIG_HLP_DORA,          PARAMFLAG_BOOL, .iptr=&DO_RA,                           .defintval=0,             TYPE_INT,    0},  \
  {"sl-mode",               CONFIG_HLP_SL_MODE,       0,              .u8ptr=&SL_MODE,                        .defintval=0,             TYPE_UINT8,  0},  \
  {"clock-source",          CONFIG_HLP_CLK,           0,              .uptr=&CLOCK_SOURCE,                    .defintval=0,             TYPE_UINT,   0},  \
  {"time-source",           CONFIG_HLP_TME,           0,              .uptr=&TIMING_SOURCE,                   .defintval=0,             TYPE_UINT,   0},  \
  {"tune-offset",           CONFIG_HLP_TUNE_OFFSET,   0,              .dblptr=&TUNE_OFFSET,                   .defintval=0,             TYPE_DOUBLE, 0},  \
  {"C" ,                    CONFIG_HLP_DLF,           0,              .u64ptr=&(downlink_frequency[0][0]),    .defuintval=0,            TYPE_UINT64, 0},  \
  {"CO" ,                   CONFIG_HLP_ULF,           0,              .i64ptr=&(uplink_frequency_offset[0][0]), .defintval=0,           TYPE_INT64,  0},  \
  {"a" ,                    CONFIG_HLP_CHOFF,         0,              .iptr=&CHAIN_OFFSET,                    .defintval=0,             TYPE_INT,    0},  \
  {"d" ,                    CONFIG_HLP_SOFTS,         PARAMFLAG_BOOL, .uptr=&do_forms,                        .defintval=0,             TYPE_UINT,   0},  \
  {"q" ,                    CONFIG_HLP_STMON,         PARAMFLAG_BOOL, .iptr=&cpu_meas_enabled,                     .defintval=0,             TYPE_INT,    0},  \
  {"numerology" ,           CONFIG_HLP_NUMEROLOGY,    0,              .iptr=&NUMEROLOGY,                      .defintval=1,             TYPE_INT,    0},  \
  {"band" ,                 CONFIG_HLP_BAND,          0,              .iptr=&BAND,                            .defintval=78,            TYPE_INT,    0},  \
  {"parallel-config",       CONFIG_HLP_PARALLEL_CMD,  0,              .strptr=&parallel_config,               .defstrval=NULL,          TYPE_STRING, 0},  \
  {"worker-config",         CONFIG_HLP_WORKER_CMD,    0,              .strptr=&worker_config,                 .defstrval=NULL,          TYPE_STRING, 0},  \
  {"noS1",                  CONFIG_HLP_NOS1,          PARAMFLAG_BOOL, .uptr=&noS1,                            .defintval=0,             TYPE_UINT,   0},  \
  {"rfsim",                 CONFIG_HLP_RFSIM,         PARAMFLAG_BOOL, .uptr=&rfsim,                           .defintval=0,             TYPE_UINT,   0},  \
  {"nbiot-disable",         CONFIG_HLP_DISABLNBIOT,   PARAMFLAG_BOOL, .uptr=&nonbiot,                         .defuintval=0,            TYPE_UINT,   0},  \
  {"chest-freq",            CONFIG_HLP_CHESTFREQ,     0,              .iptr=&CHEST_FREQ,                      .defintval=0,             TYPE_INT,    0},  \
  {"chest-time",            CONFIG_HLP_CHESTTIME,     0,              .iptr=&CHEST_TIME,                      .defintval=0,             TYPE_INT,    0},  \
  {"nsa",                   CONFIG_HLP_NSA,           PARAMFLAG_BOOL, .iptr=&NSA,                             .defintval=0,             TYPE_INT,    0},  \
  {"node-number",           NULL,                     0,              .u16ptr=&NODE_NUMBER,                   .defuintval=0,            TYPE_UINT16, 0},  \
  {"usrp-tx-thread-config", CONFIG_HLP_USRP_THREAD,   0,              .iptr=&usrp_tx_thread,                  .defstrval=0,             TYPE_INT,    0},  \
  {"nfapi",                 CONFIG_HLP_NFAPI,         0,              .strptr=NULL,                           .defstrval="MONOLITHIC",  TYPE_STRING, 0},  \
  {"non-stop",              CONFIG_HLP_NONSTOP,       PARAMFLAG_BOOL, .iptr=&NON_STOP,                        .defintval=0,             TYPE_INT,    0},  \
  {"continuous-tx",         CONFIG_HLP_CONTINUOUS_TX, PARAMFLAG_BOOL, .iptr=&CONTINUOUS_TX,                   .defintval=0,             TYPE_INT,    0},  \
  {"disable-stats",         CONFIG_HLP_STATS_DISABLE, PARAMFLAG_BOOL, .iptr=&stats_disabled,                  .defintval=0,             TYPE_INT,    0},  \
  {"no-itti-threads",       CONFIG_HLP_NOITTI,        PARAMFLAG_BOOL, .iptr=&softmodem_params.no_itti,        .defintval=0,             TYPE_INT,    0},  \
  {"sync-ref",              CONFIG_HLP_SYNC_REF,      0,              .uptr=&SYNC_REF,                        .defintval=0,             TYPE_UINT,   0},  \
  {"A" ,                    CONFIG_HLP_TADV,          0,             .iptr=&softmodem_params.command_line_sample_advance,.defintval=0,            TYPE_INT,   0},  \
  {"E" ,                    CONFIG_HLP_TQFS,          PARAMFLAG_BOOL, .iptr=&softmodem_params.threequarter_fs, .defintval=0,            TYPE_INT,    0}, \
  {"imscope" ,              CONFIG_HLP_IMSCOPE,       PARAMFLAG_BOOL, .uptr=&enable_imscope,                   .defintval=0,            TYPE_UINT,   0}, \
  {"imscope-record" ,       CONFIG_HLP_IMSCOPE_RECORD,PARAMFLAG_BOOL, .uptr=&enable_imscope_record,            .defintval=0,            TYPE_UINT,   0}, \
  {"default-pdu-id",        NULL,                     0,              .iptr=&DEFAULT_PDU_ID,                   .defintval=-1,           TYPE_INT,    0}, \
  /* // !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!! */ \
  {"motfs.enable",          CONFIG_HLP_MOTFS_ENABLE,  PARAMFLAG_BOOL, .iptr=&MOTFS_ENABLE,                     .defintval=0,            TYPE_INT,    0}, \
  {"ddchan.enable",         CONFIG_HLP_DDCHAN_ENABLE, PARAMFLAG_BOOL, .iptr=&DDCHAN_ENABLE,                    .defintval=0,            TYPE_INT,    0}, \
  {"ddchan.apply_on_gnb_rx", CONFIG_HLP_DDCHAN_GNB_RX, PARAMFLAG_BOOL, .iptr=&DDCHAN_GNB_RX,                   .defintval=0,            TYPE_INT,    0}, \
  /* // to bypass the RA procedure */ \
  {"ddchan.start_after_ms", CONFIG_HLP_DDCHAN_START_AFTER_MS, 0,        .dblptr=&DDCHAN_START_AFTER_MS,         .defdblval=0,            TYPE_DOUBLE, 0}, \
  {"ddchan.mode",           CONFIG_HLP_DDCHAN_MODE,   0,              .strptr=&DDCHAN_MODE,                    .defstrval="doppler",   TYPE_STRING, 0}, \
  {"ddchan.doppler_hz",     CONFIG_HLP_DDCHAN_DOPPLER_HZ, 0,          .dblptr=&DDCHAN_DOPPLER_HZ,              .defdblval=0,            TYPE_DOUBLE, 0}, \
  {"ddchan.amplitude",      CONFIG_HLP_DDCHAN_AMPLITUDE, 0,           .dblptr=&DDCHAN_AMPLITUDE,               .defdblval=1,            TYPE_DOUBLE, 0}, \
  {"ddchan.delay_ns",       CONFIG_HLP_DDCHAN_DELAY_NS, 0,            .dblptr=&DDCHAN_DELAY_NS,                .defdblval=0,            TYPE_DOUBLE, 0}, \
  {"ddchan.delay_samples",  CONFIG_HLP_DDCHAN_DELAY_SAMPLES, 0,       .iptr=&DDCHAN_DELAY_SAMPLES,             .defintval=-1,           TYPE_INT,    0}, \
  {"ddchan.paths",          CONFIG_HLP_DDCHAN_PATHS,  0,              .strptr=&DDCHAN_PATHS,                   .defstrval="",          TYPE_STRING, 0}, \
  {"ddchan.normalize_power", CONFIG_HLP_DDCHAN_NORMALIZE, PARAMFLAG_BOOL, .iptr=&DDCHAN_NORMALIZE,             .defintval=0,            TYPE_INT,    0}, \
  {"ddchan.max_paths",      CONFIG_HLP_DDCHAN_MAX_PATHS, 0,           .iptr=&DDCHAN_MAX_PATHS,                 .defintval=8,            TYPE_INT,    0}, \
  {"ddchan.awgn_enable",    CONFIG_HLP_DDCHAN_AWGN_ENABLE, PARAMFLAG_BOOL, .iptr=&DDCHAN_AWGN_ENABLE,          .defintval=0,            TYPE_INT,    0}, \
  {"ddchan.snr_db",         CONFIG_HLP_DDCHAN_SNR_DB, 0,              .dblptr=&DDCHAN_SNR_DB,                  .defdblval=30,           TYPE_DOUBLE, 0}, \
  {"ddchan.noise_seed",     CONFIG_HLP_DDCHAN_NOISE_SEED, 0,          .u64ptr=&DDCHAN_NOISE_SEED,              .defint64val=1,          TYPE_UINT64, 0}, \
  {"ddchan.noise_power_mode", CONFIG_HLP_DDCHAN_NOISE_POWER_MODE, 0,  .strptr=&DDCHAN_NOISE_POWER_MODE,        .defstrval="measured",  TYPE_STRING, 0}, \
  {"ddchan.fixed_signal_power", CONFIG_HLP_DDCHAN_FIXED_SIGNAL_POWER, 0, .dblptr=&DDCHAN_FIXED_SIGNAL_POWER,   .defdblval=0,            TYPE_DOUBLE, 0}, \
  /* // for the metric */ \
  {"metric.enable",         CONFIG_HLP_METRIC_ENABLE, PARAMFLAG_BOOL, .iptr=&METRIC_ENABLE,                   .defintval=0,            TYPE_INT,    0}, \
  {"metric.dump_dir",       CONFIG_HLP_METRIC_DUMP_DIR, 0,            .strptr=&METRIC_DUMP_DIR,                .defstrval="/tmp/oai_metrics", TYPE_STRING, 0}, \
  {"metric.mode",           CONFIG_HLP_METRIC_MODE, 0,                .strptr=&METRIC_MODE,                    .defstrval="symbol",    TYPE_STRING, 0}, \
  {"metric.rv0_only",       CONFIG_HLP_METRIC_RV0_ONLY, PARAMFLAG_BOOL, .iptr=&METRIC_RV0_ONLY,                .defintval=1,            TYPE_INT,    0}, \
  {"metric.new_tx_only",    CONFIG_HLP_METRIC_NEW_TX_ONLY, PARAMFLAG_BOOL, .iptr=&METRIC_NEW_TX_ONLY,          .defintval=1,            TYPE_INT,    0}, \
  {"metric.rnti_filter",    CONFIG_HLP_METRIC_RNTI_FILTER, 0,         .strptr=&METRIC_RNTI_FILTER,             .defstrval="0",         TYPE_STRING, 0}, \
  {"metric.skip_ra",        CONFIG_HLP_METRIC_SKIP_RA, PARAMFLAG_BOOL, .iptr=&METRIC_SKIP_RA,                  .defintval=1,            TYPE_INT,    0}, \
  {"metric.max_records",    CONFIG_HLP_METRIC_MAX_RECORDS, 0,         .iptr=&METRIC_MAX_RECORDS,               .defintval=10000,        TYPE_INT,    0}, \
  {"metric.drop_when_full", CONFIG_HLP_METRIC_DROP_WHEN_FULL, PARAMFLAG_BOOL, .iptr=&METRIC_DROP_WHEN_FULL,   .defintval=1,            TYPE_INT,    0}, \
}
// clang-format on

// clang-format off
#define CMDLINE_PARAMS_CHECK_DESC {         \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s3a = { config_checkstr_assign_integer, \
               {"MONOLITHIC", "PNF", "VNF", "AERIAL","UE_STUB_PNF","UE_STUB_OFFNET","STANDALONE_PNF"}, \
               {NFAPI_MONOLITHIC, NFAPI_MODE_PNF, NFAPI_MODE_VNF, NFAPI_MODE_AERIAL,NFAPI_UE_STUB_PNF,NFAPI_UE_STUB_OFFNET,NFAPI_MODE_STANDALONE_PNF}, \
               7 } }, \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
    { .s5 = { NULL } },                     \
	    { .s5 = { NULL } },                     \
	    { .s5 = { NULL } },                     \
	    { .s5 = { NULL } },                     \
	    { .s5 = { NULL } },                     \
	    { .s5 = { NULL } },                     \
	    { .s5 = { NULL } },                     \
	    { .s5 = { NULL } },                     \
	    { .s5 = { NULL } },                     \
	    { .s5 = { NULL } },                     \
	    { .s5 = { NULL } },                     \
	    { .s5 = { NULL } },                     \
	    { .s5 = { NULL } },                     \
	  }
// clang-format on

#define CONFIG_HLP_NSA           "Enable NSA mode \n"
#define CONFIG_HLP_FLOG          "Enable online log \n"
#define CONFIG_HLP_LOGL          "Set the global log level, valid options: (4:trace, 3:debug, 2:info, 1:warn, (0:error))\n"
#define CONFIG_HLP_TELN          "Start embedded telnet server \n"
#define CONFIG_HLP_WEB "Start embedded web server \n"
#define CONFIG_FLOG_OPT          "R"
#define CONFIG_LOGL_OPT          "g"
/*-------------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                            command line parameters for LOG utility                                                              */
/*   optname                        helpstr       paramflags        XXXptr                              defXXXval            type           numelt */
/*-------------------------------------------------------------------------------------------------------------------------------------------------*/
// clang-format off
#define CMDLINE_LOGPARAMS_DESC  { \
  {CONFIG_FLOG_OPT, CONFIG_HLP_FLOG, 0,                                      .uptr = &online_log_messages, .defintval = 1,    TYPE_INT,    0}, \
  {CONFIG_LOGL_OPT, CONFIG_HLP_LOGL, 0,                                      .uptr = &glog_level,          .defintval = 0,    TYPE_UINT,   0}, \
  {"telnetsrv",     CONFIG_HLP_TELN, PARAMFLAG_BOOL | PARAMFLAG_CMDLINEONLY, .uptr = &start_telnetsrv,     .defintval = 0,    TYPE_UINT,   0}, \
  {"websrv",        CONFIG_HLP_WEB,  PARAMFLAG_BOOL | PARAMFLAG_CMDLINEONLY, .uptr = &start_websrv,        .defintval = 0,    TYPE_UINT,   0}, \
  {"log-mem",       NULL,            0,                                      .strptr = &logmem_filename,   .defstrval = NULL, TYPE_STRING, 0}, \
  {"telnetclt",     NULL,            0,                                      .uptr = &start_telnetclt,     .defstrval = NULL, TYPE_UINT,   0}, \
}
// clang-format on

/* check function for global log level */
// clang-format off
#define CMDLINE_LOGPARAMS_CHECK_DESC { \
  { .s5= {NULL} } ,                       \
  { .s2= {config_check_intrange, {0,4}}}, \
  { .s5= {NULL} } ,                       \
  { .s5= {NULL} } ,                       \
  { .s5= {NULL} } ,                       \
  { .s5= {NULL} } ,                       \
}
// clang-format on

/***************************************************************************************************************************************/

#define IS_SOFTMODEM_NOS1 (get_softmodem_optmask()->bit.SOFTMODEM_NOS1_BIT)
#define IS_SOFTMODEM_NONBIOT (get_softmodem_optmask()->bit.SOFTMODEM_NONBIOT_BIT)
#define IS_SOFTMODEM_RFSIM (get_softmodem_optmask()->bit.SOFTMODEM_RFSIM_BIT)
#define IS_SOFTMODEM_SIML1 (get_softmodem_optmask()->bit.SOFTMODEM_SIML1_BIT)
#define IS_SOFTMODEM_DLSIM (get_softmodem_optmask()->bit.SOFTMODEM_DLSIM_BIT)
#define IS_SOFTMODEM_DOSCOPE (get_softmodem_optmask()->bit.SOFTMODEM_DOSCOPE_BIT)
#define IS_SOFTMODEM_IQPLAYER (get_softmodem_optmask()->bit.SOFTMODEM_RECPLAY_BIT)
#define IS_SOFTMODEM_IQRECORDER (get_softmodem_optmask()->bit.SOFTMODEM_RECRECORD_BIT)
#define IS_SOFTMODEM_TELNETCLT (get_softmodem_optmask()->bit.SOFTMODEM_TELNETCLT_BIT)
#define IS_SOFTMODEM_ENB (get_softmodem_optmask()->bit.SOFTMODEM_ENB_BIT)
#define IS_SOFTMODEM_GNB (get_softmodem_optmask()->bit.SOFTMODEM_GNB_BIT)
#define IS_SOFTMODEM_4GUE (get_softmodem_optmask()->bit.SOFTMODEM_4GUE_BIT)
#define IS_SOFTMODEM_5GUE (get_softmodem_optmask()->bit.SOFTMODEM_5GUE_BIT)
#define IS_SOFTMODEM_NOSTATS (get_softmodem_optmask()->bit.SOFTMODEM_NOSTATS_BIT)
#define IS_SOFTMODEM_IMSCOPE_ENABLED (get_softmodem_optmask()->bit.SOFTMODEM_IMSCOPE_BIT)
#define IS_SOFTMODEM_IMSCOPE_RECORD_ENABLED (get_softmodem_optmask()->bit.SOFTMODEM_IMSCOPE_RECORD_BIT)
typedef struct optmask_s {
  union {
    struct {
      uint64_t SOFTMODEM_NOS1_BIT: 1;
      uint64_t SOFTMODEM_NOKRNMOD_BIT: 1;
      uint64_t SOFTMODEM_NONBIOT_BIT: 1;
      uint64_t SOFTMODEM_RFSIM_BIT: 1;
      uint64_t SOFTMODEM_SIML1_BIT: 1;
      uint64_t SOFTMODEM_DLSIM_BIT: 1;
      uint64_t SOFTMODEM_DOSCOPE_BIT: 1;
      uint64_t SOFTMODEM_RECPLAY_BIT: 1;
      uint64_t SOFTMODEM_TELNETCLT_BIT: 1;
      uint64_t SOFTMODEM_RECRECORD_BIT: 1;
      uint64_t SOFTMODEM_ENB_BIT: 1;
      uint64_t SOFTMODEM_GNB_BIT: 1;
      uint64_t SOFTMODEM_4GUE_BIT: 1;
      uint64_t SOFTMODEM_5GUE_BIT: 1;
      uint64_t SOFTMODEM_NOSTATS_BIT: 1;
      uint64_t SOFTMODEM_IMSCOPE_BIT: 1;
      uint64_t SOFTMODEM_IMSCOPE_RECORD_BIT : 1;
    } bit;
    uint64_t v; // allow to export entire bit set, force to 64 bit processor atomic size
  };
} optmask_t;
typedef struct {
  optmask_t optmask;
  //THREAD_STRUCT  thread_struct;
  char           *rf_config_file;
  char *threadPoolConfig;
  int            phy_test;
  int            do_ra;
  uint8_t        sl_mode;
  int            chain_offset;
  int            numerology;
  int            band;
  uint32_t       clock_source;
  uint32_t       timing_source;
  double         tune_offset;
  int command_line_sample_advance;
  uint32_t       send_dmrs_sync;
  int            use_256qam_table;
  int            chest_time;
  int            chest_freq;
  // !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
  int            motfs_enable;
  int            ddchan_enable;
  int            ddchan_apply_on_gnb_rx;
  // to bypass the RA procedure
  double         ddchan_start_after_ms;
  char           *ddchan_mode;
  double         ddchan_doppler_hz;
  double         ddchan_amplitude;
  double         ddchan_delay_ns;
  int            ddchan_delay_samples;
  char           *ddchan_paths;
  int            ddchan_normalize_power;
  int            ddchan_max_paths;
  int            ddchan_awgn_enable;
  double         ddchan_snr_db;
  uint64_t       ddchan_noise_seed;
  char           *ddchan_noise_power_mode;
  double         ddchan_fixed_signal_power;
  // for the metric
  int            metric_enable;
  // for the metric
  char           *metric_dump_dir;
  // for the metric
  char           *metric_mode;
  // for the metric
  int            metric_rv0_only;
  // for the metric
  int            metric_new_tx_only;
  // for the metric
  char           *metric_rnti_filter;
  // for the metric
  int            metric_skip_ra;
  // for the metric
  int            metric_max_records;
  // for the metric
  int            metric_drop_when_full;
  uint8_t        nfapi;
  int            nsa;
  uint16_t       node_number;
  int            non_stop;
  int            continuous_tx;
  uint32_t       sync_ref;
  int no_itti;
  int threequarter_fs;
  int default_pdu_session_id;
  int extra_pdu_session_id;
} softmodem_params_t;

#define IS_SA_MODE(sM_params) (!(sM_params)->phy_test && !(sM_params)->do_ra && !(sM_params)->nsa)
void softmodem_verify_mode(const softmodem_params_t *p);

#define get_softmodem_optmask() (&(get_softmodem_params()->optmask))
softmodem_params_t *get_softmodem_params(void);
void get_common_options(configmodule_interface_t *cfg);
char *get_softmodem_function(void);
#define SOFTMODEM_RTSIGNAL  (SIGRTMIN+1)
void set_softmodem_sighandler(void);
extern uint64_t downlink_frequency[MAX_NUM_CCs][4];
extern int64_t uplink_frequency_offset[MAX_NUM_CCs][4];
extern int usrp_tx_thread;
extern int sf_ahead;
extern int oai_exit;

void ru_tx_func(void *param);
void configure_ru(void *, void *arg);
void configure_rru(void *, void *arg);
struct timespec timespec_add(struct timespec lhs, struct timespec rhs);
struct timespec timespec_sub(struct timespec lhs, struct timespec rhs);
extern uint8_t nfapi_mode;
extern char *parallel_config;
extern char *worker_config;
extern double cpuf;

#ifdef __cplusplus
}
#endif
#endif
