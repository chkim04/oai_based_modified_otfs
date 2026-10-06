<!-- SPDX-License-Identifier: CC-BY-4.0 -->

<h1 align="center">
    <a href="https://lfnetworking.org/projects/duranta/"><img src="https://raw.githubusercontent.com/duranta-project/governance/main/logos/Duranta-OAI-Combined.png" alt="Duranta OAI" width="550"></a>
</h1>

<p align="center">
    <a href="https://github.com/duranta-project/openairinterface5g/blob/develop/LICENSE"><img src="https://img.shields.io/badge/license-CSSL--v1.0-blue" alt="License"></a>
    <a href="https://releases.ubuntu.com/22.04/"><img src="https://img.shields.io/badge/OS-Ubuntu22-Green" alt="Supported OS Ubuntu 22"></a>
    <a href="https://releases.ubuntu.com/24.04/"><img src="https://img.shields.io/badge/OS-Ubuntu24-Green" alt="Supported OS Ubuntu 24"></a>
    <a href="https://releases.ubuntu.com/26.04/"><img src="https://img.shields.io/badge/OS-Ubuntu26-Green" alt="Supported OS Ubuntu 26"></a>
    <a href="https://www.redhat.com/en/technologies/linux-platforms/enterprise-linux"><img src="https://img.shields.io/badge/OS-RHEL9-Green" alt="Supported OS RHEL9"></a>
    <a href="https://getfedora.org/en/workstation/"><img src="https://img.shields.io/badge/OS-Fedora44-Green" alt="Supported OS Fedora 44"></a>
</p>

<p align="center">
    <a href="https://jenkins-oai.eurecom.fr/job/RAN-Ubuntu18-Image-Builder/"><img src="https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fjenkins-oai.eurecom.fr%2Fjob%2FRAN-Ubuntu18-Image-Builder%2F&label=build-Ubuntu-x86%20Images"></a>
    <a href="https://jenkins-oai.eurecom.fr/job/RAN-RHEL8-Cluster-Image-Builder/"><img src="https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fjenkins-oai.eurecom.fr%2Fjob%2FRAN-RHEL8-Cluster-Image-Builder%2F&label=build-UBI-x86%20Images"></a>
    <a href="https://jenkins-oai.eurecom.fr/job/RAN-Ubuntu-ARM-Image-Builder/"><img src="https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fjenkins-oai.eurecom.fr%2Fjob%2FRAN-Ubuntu-ARM-Image-Builder%2F&label=build-Ubuntu-ARM%20Images"></a>
</p>

<p align="center">
  <a href="https://hub.docker.com/r/oaisoftwarealliance/oai-gnb"><img alt="Docker Pulls" src="https://img.shields.io/docker/pulls/oaisoftwarealliance/oai-gnb?label=gNB%20docker%20pulls"></a>
  <a href="https://hub.docker.com/r/oaisoftwarealliance/oai-nr-ue"><img alt="Docker Pulls" src="https://img.shields.io/docker/pulls/oaisoftwarealliance/oai-nr-ue?label=NR-UE%20docker%20pulls"></a>
  <a href="https://hub.docker.com/r/oaisoftwarealliance/oai-enb"><img alt="Docker Pulls" src="https://img.shields.io/docker/pulls/oaisoftwarealliance/oai-enb?label=eNB%20docker%20pulls"></a>
  <a href="https://hub.docker.com/r/oaisoftwarealliance/oai-lte-ue"><img alt="Docker Pulls" src="https://img.shields.io/docker/pulls/oaisoftwarealliance/oai-lte-ue?label=LTE-UE%20docker%20pulls"></a>
  <a href="https://hub.docker.com/r/oaisoftwarealliance/oai-nr-cuup"><img alt="Docker Pulls" src="https://img.shields.io/docker/pulls/oaisoftwarealliance/oai-nr-cuup?label=NR-CUUP%20docker%20pulls"></a>
</p>

# OAI-based Modified OTFS: gNB Customizations

This repository extends Duranta/OpenAirInterface with an experimental modified
OTFS-like (MOTFS) uplink PUSCH receiver, a digital receive-channel emulator, and
PHY symbol tracing. The target testbed is a separate UE PC with a USRP B210
connected through an attenuator/cable to this gNB PC with a USRP X410.

MOTFS is a non-standard research waveform. This customization implements the
gNB receiver; the matching UE transmit-chain integration must be provided on
the separate UE system. Standard DMRS processing is retained. The experimental
features are disabled by default.

## What is customized

| Component | Customization | Main implementation |
| --- | --- | --- |
| Time-axis transform | Unitary N-point IDFT/DFT on complex PUSCH data symbols | [nr_motfs.c](openair1/PHY/MODULATION/nr_motfs.c) |
| gNB PUSCH receiver | Buffer post-IDFT symbols, apply the time-axis DFT, then generate QPSK LLRs | [nr_ulsch_demodulation.c](openair1/PHY/NR_TRANSPORT/nr_ulsch_demodulation.c) |
| Digital RX channel | Doppler, integer delay, multipath, and optional AWGN before the OFDM FFT | [nr_ddchan.c](openair1/PHY/MODULATION/nr_ddchan.c), [nr-ru.c](executables/nr-ru.c) |
| PHY measurements | Binary traces of recovered PUSCH QAM symbols and allocation metadata | [nr_phy_metric_trace.c](openair1/PHY/MODULATION/nr_phy_metric_trace.c) |
| Runtime configuration | Independent `motfs.*`, `ddchan.*`, and `metric.*` options | [softmodem-common.h](executables/softmodem-common.h) |
| Buffer lifecycle | Allocate MOTFS and trace buffers during initialization and release them during teardown | [nr_init.c](openair1/PHY/INIT/nr_init.c) |
| RA trace filtering | Mark Msg3 and retransmissions so measurements can exclude them | [gNB_scheduler_RA.c](openair2/LAYER2/NR_MAC_gNB/gNB_scheduler_RA.c) |

### MOTFS waveform and matched receiver

For one PUSCH allocation, `M = 12 * allocated_PRBs` and `N` is the number of
data-bearing OFDM symbols, excluding DMRS symbols. Symbols are stored as
`buffer[t * M + m]`, where `t` is the logical data-symbol index and `m` is the
subcarrier index within the allocation.

```text
Required matching UE TX:
QAM / layer mapping -> N-point unitary IDFT across data-symbol index
                    -> existing M-point DFT -> RE mapping -> OFDM TX

Implemented gNB RX:
OFDM FFT -> standard DMRS channel estimation -> extraction / equalization
         -> existing M-point IDFT -> buffer all data-bearing symbols
         -> N-point unitary DFT -> QPSK LLRs -> descrambling / LDPC
```

`nr_motfs_time_precoding()` and `nr_motfs_time_deprecoding()` implement the
IDFT and DFT respectively. They use OAI `c16_t` samples, runtime `M`, and
`N = 1..14`. The `1/sqrt(N)` normalization is included in Q15 coefficients;
64-bit accumulation, rounded Q15 scaling, and 16-bit saturation are explicit.
`N = 1` is an identity operation; larger transforms require distinct input and
output buffers. No heap allocation is performed by the transform.

The RX hook is `nr_rx_pusch_motfs()` in `nr_ulsch_demodulation.c`, selected by
`--motfs.enable 1`. With the flag disabled, the existing PUSCH processing path
is selected. Enabled MOTFS reception asserts the following restrictions:

- Uplink PUSCH with transform precoding enabled, rank 1, and one spatial stream.
- QPSK only (`Qm = 2`); PTRS, UCI on PUSCH, and frequency hopping disabled.
- DMRS-bearing symbols contain no data REs; each selected data symbol contains
  exactly `M` data REs.

Unsupported grants terminate through assertions. The flag does not configure
the UE or automatically constrain all scheduler grants to these restrictions.
It applies to PUSCH reception, including Msg3; `metric.skip_ra` only filters
measurement records and does not bypass MOTFS processing.

### Digital receive-channel emulator (DDCHAN)

In `nr-ru.c`, `nr_ddchan_apply_gnb_rx()` runs after `trx_read_func()` and before
OFDM processing, including the slot-alignment read path. It modifies the gNB
receive samples, so its effect includes reference signals and other received
uplink channels, not just PUSCH data. It does not require RFsimulator channelmod
or `--channelmod`.

For a single path without delay or noise, the model is
`y[n] = amplitude * x[n] * exp(j * 2*pi*fD*(rx_timestamp+n)/Fs)`.
The RF hook supplies the RU-adjusted RX timestamp. Delay lines and working
buffers are allocated during channel initialization.

| Options | Meaning / default |
| --- | --- |
| `--ddchan.enable`, `--ddchan.apply_on_gnb_rx` | Both must be `1` to apply the channel; both default to `0` |
| `--ddchan.mode` | `doppler` (default) or `multipath` |
| `--ddchan.doppler_hz`, `--ddchan.amplitude` | Single-path Doppler in Hz (default `0`) and linear amplitude (default `1`) |
| `--ddchan.delay_ns`, `--ddchan.delay_samples` | Delay rounded to integer samples; nonnegative `delay_samples` overrides `delay_ns` |
| `--ddchan.paths` | Semicolon-separated `delay_ns,doppler_hz,gain_db,phase_deg` paths; quote the argument in the shell |
| `--ddchan.normalize_power`, `--ddchan.max_paths` | Optional unit-total-path-power normalization (default off); at most 8 paths |
| `--ddchan.start_after_ms` | Bypass the channel for this duration after the first RX timestamp; default `0` |
| `--ddchan.awgn_enable`, `--ddchan.snr_db` | Optional software AWGN (default off), target SNR (default `30` dB) |
| `--ddchan.noise_seed` | Reproducible noise seed (default `1`) |
| `--ddchan.noise_power_mode`, `--ddchan.fixed_signal_power` | `measured` (default) or `fixed` signal-power basis for AWGN |

`start_after_ms` is a time-based startup bypass, not an RA-completion detector.
The emulator supports integer-sample delays, not fractional-delay filtering.

### PHY measurement traces

`--metric.enable 1` writes recovered QAM symbols after the existing M-point
IDFT for standard DFT-s-OFDM, or after the additional N-point DFT for MOTFS.
The default output is `/tmp/oai_metrics/gnb_phy_trace_<pid>.bin`; override the
directory with `--metric.dump_dir`. The record format and allocation metadata
are defined in [nr_phy_metric_trace.h](openair1/PHY/MODULATION/nr_phy_metric_trace.h).
QAM payloads use `c16_t` in logical `t * M + m` order.

- `--metric.mode symbol` is the default QAM dump mode. `summary` omits the QAM
  payload. `bit`/LLR tracing is not implemented; `full` currently dumps QAM only.
- `--metric.rv0_only 1`, `--metric.new_tx_only 1`, and `--metric.skip_ra 1` are
  the defaults. `--metric.rnti_filter` accepts decimal or hexadecimal RNTIs;
  `0` disables that filter.
- `--metric.max_records` defaults to `10000` (`0` means unlimited), with
  `--metric.drop_when_full 1` by default.
- Standard DFT-s-OFDM QAM tracing supports rank 1, transform precoding enabled,
  and PTRS disabled. MOTFS tracing inherits the MOTFS receiver restrictions.
- The writer uses a mutex and buffered `fwrite()` in the processing path;
  there is no asynchronous writer thread. Tracing can affect real-time timing.

### Build and run examples

Follow the upstream [build instructions](doc/BUILD.md) for dependencies and
initial configuration. For the already-configured gNB build directory:

```bash
cmake --build cmake_targets/ran_build/build --target nr-softmodem -j2
```

Replace `/path/to/gnb.conf` with a configuration for your hardware and network.
Run from the build directory:

```bash
cd cmake_targets/ran_build/build

# Standard receiver with all experimental features disabled
sudo ./nr-softmodem -O /path/to/gnb.conf \
  --motfs.enable 0 --ddchan.enable 0 --metric.enable 0

# MOTFS receiver: requires a matching UE transmitter and supported grants
sudo ./nr-softmodem -O /path/to/gnb.conf \
  --motfs.enable 1 --metric.enable 1 --metric.mode symbol

# MOTFS with a 500 Hz RX Doppler shift, activated after 5 seconds
sudo ./nr-softmodem -O /path/to/gnb.conf \
  --motfs.enable 1 --ddchan.enable 1 --ddchan.apply_on_gnb_rx 1 \
  --ddchan.mode doppler --ddchan.doppler_hz 500 --ddchan.start_after_ms 5000
```

DDCHAN and tracing can also be used with MOTFS disabled. Establish a clean
static link with the matching waveform before introducing channel impairments.

Two lab configuration files are customized:

- [gnb.sa.band78.fr1.106PRB.usrpb210.conf](targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.sa.band78.fr1.106PRB.usrpb210.conf):
  changes include Msg3 transform precoding, `ul_max_mcs = 9`, network and SDR
  addresses, and CSI-RS/SRS configuration.
- [gnb.band78.sa.fr1.106PRB.usrpn310_mod.conf](targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.band78.sa.fr1.106PRB.usrpn310_mod.conf):
  an additional lab configuration.

These contain machine-specific settings. Their filenames do not establish
compatibility with your SDR; review addresses, RF settings, and scheduler
constraints before use. Disabling experimental flags does not undo changes
in a selected configuration file.

### Validation and current limitations

The gNB `nr-softmodem` build passed for customization commit `3774fd7c98`.
This is compile/link validation, not proof of clean B2B decoding or real-time
performance with MOTFS and channel impairments enabled.

The repository includes [MOTFS tests](openair1/PHY/MODULATION/tests/test_nr_motfs.cpp)
for identity, round-trip accuracy over all supported `N`, and invalid inputs,
and [DDCHAN tests](openair1/PHY/MODULATION/tests/test_nr_ddchan.cpp) for bypass,
Doppler rotation, block continuity, integer delay, multipath normalization,
and reproducible AWGN. They are registered as `test_nr_motfs` and
`test_nr_ddchan` when CMake `ENABLE_TESTS=ON`. They were not rerun as part of
the above gNB build (`ENABLE_TESTS=OFF`). In a build configured with tests:

```bash
cmake --build <test-build-dir> --target test_nr_motfs test_nr_ddchan -j2
ctest --test-dir <test-build-dir> -R '^test_nr_(motfs|ddchan)$' --output-on-failure
```

Higher-order modulation, multiple layers, and the other unsupported MOTFS
grant configurations remain outside the implemented receiver scope. No
matching UE PUSCH transmit-chain integration is included in this customization.

# Duranta - OpenAirInterface

Duranta OpenAirInterface RAN delivers and maintains an open-source cellular
wireless software stack for 4G, 5G and future networking technologies. It
supports simulation, prototyping, and end-to-end deployments on
Commercial-Off-The-Shelf (COTS) hardware. Built for research and
experimentation, it provides standard-compliant interfaces and is released
under the Collaborative Standards Software License (CSSL).

## License

 *  [OAI License Model](http://www.openairinterface.org/?page_id=101)
 *  [CSSL v1.0](http://www.openairinterface.org/?page_id=698)

The source code is distributed under [**CSSL v1.0**](LICENSE).
Some files, such as for orchestration, are distributed under
[MIT license](./LICENSES/preferred/MIT.txt). Documentation is distributed under
[Creative Commons Attribution 4.0 International license](LICENSES/preferred/CC-BY-4.0.txt).

All the files without an explicit copyright header have an implicit "Copyright
of OpenAirInterface Authors".

Please see [NOTICE](NOTICE) for other licenses which are used in the software.

In the past OAI source code has been re-licensed sometimes, here is the
history:

1. CSSL v1.0 starting tag 2026.w14
2. OAI Public License v1.1 starting tag v1.0 till af4b0d53
3. OAI Public License v1.0: starting tag v.04 till v1.0
4. GPL 3: starting tag v.0 till v.04 (only initial implementation of 4G)

## Where to Start

 *  [General overview of documentation](./doc/README.md)
 *  [The implemented features](./doc/FEATURE_SET.md)
 *  [System Requirements for Using OAI Stack](./doc/system_requirements.md)
 *  [How to build](./doc/BUILD.md)
 *  [How to run the modems](./doc/RUNMODEM.md)

Not all information is available in a central place, and information for
specific sub-systems might be available in the corresponding sub-directories.
To find all READMEs, this command might be handy:

```
find . -iname "readme*"
```

## RAN repository structure

The OpenAirInterface (OAI) software is composed of the following parts: 

```
openairinterface5g
├── charts
├── ci-scripts        : Meta-scripts used by the OSA CI process. Contains also configuration files used day-to-day by CI.
├── CMakeLists.txt    : Top-level CMakeLists.txt for building
├── cmake_targets     : Build utilities to compile (simulation, emulation and real-time platforms), and generated build files.
├── common            : Some common OAI utilities, some other tools can be found at openair2/UTILS.
├── doc               : Documentation
├── docker            : Dockerfiles to build for Ubuntu and RHEL
├── executables       : Top-level executable source files (gNB, eNB, ...)
├── maketags          : Script to generate emacs tags.
├── nfapi             : (n)FAPI code for MAC-PHY interface
├── openair1          : Layer 1 (3GPP LTE Rel-10/12 PHY, NR Rel-15 PHY)
├── openair2          : Layer 2 (3GPP LTE Rel-10 MAC/RLC/PDCP/RRC/X2AP, LTE Rel-14 M2AP, NR Rel-15+ MAC/RLC/PDCP/SDAP/RRC/X2AP/F1AP/E1AP), E2AP
├── openair3          : Layer 3 (3GPP LTE Rel-10 S1AP/GTP, NR Rel-15 NGAP/GTP)
├── openshift         : OpenShift helm charts for some deployment options of OAI
├── radio             : Drivers for various radios such as USRP, AW2S, RFsim, 7.2 FHI, ...
├── targets           : Some configuration files; only historical relevance, and might be deleted in the future
└── tools             : Tools for use by the developers/ci machines: code analysis and formatting
```

## How to get support from the Community

You can ask your question on the [mailing lists](https://github.com/duranta-project/openairinterface5g/wiki/MailingList).

Your email should contain below information:

- A clear subject in your email.
- For all the queries there should be [Query\] in the subject of the email and for problems there should be [Problem\].
- In case of a problem, add a small description.
- Do not share any photos unless you want to share a diagram.
- OAI gNB/DU/CU/CU-CP/CU-UP configuration file in `.conf` format only.
- Logs of OAI gNB/DU/CU/CU-CP/CU-UP in `.log` or `.txt` format only.
- In case your question is related to performance, include a small description of the machine (Operating System, Kernel version, CPU, RAM and networking card) and diagram of your testing environment.
- Known/open issues are present on [Github](https://github.com/duranta-project/openairinterface5g/issues), so keep checking.

Always remember a structured email will help us understand your issues quickly.
