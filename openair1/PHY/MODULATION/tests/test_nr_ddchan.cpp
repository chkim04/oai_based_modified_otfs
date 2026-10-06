/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "gtest/gtest.h"

#include <cmath>
#include <cstdint>
#include <vector>

extern "C" {
// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
#include "openair1/PHY/MODULATION/nr_ddchan.h"
}

namespace {

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
nr_ddchan_config_t base_config()
{
  return nr_ddchan_config_t{
      .enabled = true,
      .apply_on_gnb_rx = true,
      .use_timestamp_phase = true,
      .mode = "doppler",
      .doppler_hz = 0.0,
      .amplitude = 1.0,
      .delay_ns = 0.0,
      .delay_samples = -1,
      .paths = "",
      .normalize_power = false,
      .max_paths = NR_DDCHAN_MAX_PATHS,
      .awgn_enable = false,
      .snr_db = 30.0,
      .noise_seed = 1,
      .noise_power_mode = "measured",
      .fixed_signal_power = 0.0,
      .sample_rate_hz = 1000.0,
      .nb_antennas = 1,
      .max_block_samples = 32,
  };
}

void expect_samples_eq(const std::vector<c16_t> &a, const std::vector<c16_t> &b)
{
  ASSERT_EQ(a.size(), b.size());
  for (size_t i = 0; i < a.size(); i++) {
    EXPECT_EQ(a[i].r, b[i].r) << "real mismatch at " << i;
    EXPECT_EQ(a[i].i, b[i].i) << "imag mismatch at " << i;
  }
}

void apply(nr_ddchan_state_t *state, std::vector<c16_t> &samples, int64_t timestamp)
{
  c16_t *rx[] = {samples.data()};
  ASSERT_EQ(nr_ddchan_apply_gnb_rx(state, rx, 1, samples.size(), timestamp), 0);
}

} // namespace

// !!!!!!!!!!!!!!!!!!!!!for the delay-Doppler channel implementation!!!!!!!!!!!!!!!!!!
TEST(NrDdchanTest, DisabledIsIdentity)
{
  std::vector<c16_t> samples = {{100, -10}, {-200, 30}, {1234, -4321}};
  const std::vector<c16_t> expected = samples;

  nr_ddchan_config_t cfg = base_config();
  cfg.enabled = false;
  nr_ddchan_state_t state;
  ASSERT_EQ(nr_ddchan_init(&state, &cfg), 0);
  apply(&state, samples, 123);
  expect_samples_eq(samples, expected);
  nr_ddchan_free(&state);
}

TEST(NrDdchanTest, ZeroDopplerZeroDelayIsIdentity)
{
  std::vector<c16_t> samples = {{100, -10}, {-200, 30}, {1234, -4321}};
  const std::vector<c16_t> expected = samples;

  nr_ddchan_config_t cfg = base_config();
  nr_ddchan_state_t state;
  ASSERT_EQ(nr_ddchan_init(&state, &cfg), 0);
  apply(&state, samples, 123);
  expect_samples_eq(samples, expected);
  nr_ddchan_free(&state);
}

TEST(NrDdchanTest, QuarterRateDopplerRotatesTone)
{
  std::vector<c16_t> samples(4, c16_t{1000, 0});

  nr_ddchan_config_t cfg = base_config();
  cfg.doppler_hz = 250.0;
  nr_ddchan_state_t state;
  ASSERT_EQ(nr_ddchan_init(&state, &cfg), 0);
  apply(&state, samples, 0);

  EXPECT_NEAR(samples[0].r, 1000, 1);
  EXPECT_NEAR(samples[0].i, 0, 1);
  EXPECT_NEAR(samples[1].r, 0, 1);
  EXPECT_NEAR(samples[1].i, 1000, 1);
  EXPECT_NEAR(samples[2].r, -1000, 1);
  EXPECT_NEAR(samples[2].i, 0, 1);
  EXPECT_NEAR(samples[3].r, 0, 1);
  EXPECT_NEAR(samples[3].i, -1000, 1);
  nr_ddchan_free(&state);
}

TEST(NrDdchanTest, TimestampPhaseIsContinuousAcrossBlocks)
{
  std::vector<c16_t> all(8, c16_t{1000, 0});
  nr_ddchan_config_t cfg = base_config();
  cfg.doppler_hz = 125.0;

  nr_ddchan_state_t all_state;
  ASSERT_EQ(nr_ddchan_init(&all_state, &cfg), 0);
  apply(&all_state, all, 0);
  nr_ddchan_free(&all_state);

  std::vector<c16_t> first(4, c16_t{1000, 0});
  std::vector<c16_t> second(4, c16_t{1000, 0});
  nr_ddchan_state_t split_state;
  ASSERT_EQ(nr_ddchan_init(&split_state, &cfg), 0);
  apply(&split_state, first, 0);
  apply(&split_state, second, 4);
  nr_ddchan_free(&split_state);

  std::vector<c16_t> split;
  split.insert(split.end(), first.begin(), first.end());
  split.insert(split.end(), second.begin(), second.end());
  expect_samples_eq(split, all);
}

TEST(NrDdchanTest, CounterFallbackIsContinuousAcrossBlocks)
{
  std::vector<c16_t> all(8, c16_t{1000, 0});
  nr_ddchan_config_t cfg = base_config();
  cfg.use_timestamp_phase = false;
  cfg.doppler_hz = 125.0;

  nr_ddchan_state_t all_state;
  ASSERT_EQ(nr_ddchan_init(&all_state, &cfg), 0);
  apply(&all_state, all, 0);
  nr_ddchan_free(&all_state);

  std::vector<c16_t> first(4, c16_t{1000, 0});
  std::vector<c16_t> second(4, c16_t{1000, 0});
  nr_ddchan_state_t split_state;
  ASSERT_EQ(nr_ddchan_init(&split_state, &cfg), 0);
  apply(&split_state, first, 99);
  apply(&split_state, second, 123);
  nr_ddchan_free(&split_state);

  std::vector<c16_t> split;
  split.insert(split.end(), first.begin(), first.end());
  split.insert(split.end(), second.begin(), second.end());
  expect_samples_eq(split, all);
}

TEST(NrDdchanTest, IntegerDelayIsContinuousAcrossBlocks)
{
  nr_ddchan_config_t cfg = base_config();
  cfg.delay_samples = 2;
  nr_ddchan_state_t state;
  ASSERT_EQ(nr_ddchan_init(&state, &cfg), 0);

  std::vector<c16_t> first = {{1, 0}, {2, 0}, {3, 0}, {4, 0}};
  std::vector<c16_t> second = {{5, 0}, {6, 0}, {7, 0}, {8, 0}};
  apply(&state, first, 0);
  apply(&state, second, 4);

  expect_samples_eq(first, {{0, 0}, {0, 0}, {1, 0}, {2, 0}});
  expect_samples_eq(second, {{3, 0}, {4, 0}, {5, 0}, {6, 0}});
  nr_ddchan_free(&state);
}

TEST(NrDdchanTest, DelayNsRoundsToSamples)
{
  nr_ddchan_config_t cfg = base_config();
  cfg.delay_ns = 2000000.0;
  nr_ddchan_state_t state;
  ASSERT_EQ(nr_ddchan_init(&state, &cfg), 0);

  std::vector<c16_t> samples = {{11, 0}, {12, 0}, {13, 0}, {14, 0}};
  apply(&state, samples, 0);
  expect_samples_eq(samples, {{0, 0}, {0, 0}, {11, 0}, {12, 0}});
  nr_ddchan_free(&state);
}

TEST(NrDdchanTest, TwoPathImpulseResponse)
{
  nr_ddchan_config_t cfg = base_config();
  cfg.mode = "multipath";
  cfg.paths = "0,0,0,0;2000000,0,0,0";
  nr_ddchan_state_t state;
  ASSERT_EQ(nr_ddchan_init(&state, &cfg), 0);

  std::vector<c16_t> samples = {{100, 0}, {0, 0}, {0, 0}, {0, 0}};
  apply(&state, samples, 0);
  expect_samples_eq(samples, {{100, 0}, {0, 0}, {100, 0}, {0, 0}});
  nr_ddchan_free(&state);
}

TEST(NrDdchanTest, MultipathNormalization)
{
  nr_ddchan_config_t cfg = base_config();
  cfg.mode = "multipath";
  cfg.paths = "0,0,0,0;0,0,0,0";
  cfg.normalize_power = true;
  nr_ddchan_state_t state;
  ASSERT_EQ(nr_ddchan_init(&state, &cfg), 0);

  std::vector<c16_t> samples = {{1000, 0}};
  apply(&state, samples, 0);
  EXPECT_NEAR(samples[0].r, 1414, 1);
  EXPECT_NEAR(samples[0].i, 0, 1);
  nr_ddchan_free(&state);
}

TEST(NrDdchanTest, AwgnIsReproducibleWithFixedSeed)
{
  nr_ddchan_config_t cfg = base_config();
  cfg.awgn_enable = true;
  cfg.snr_db = 10.0;
  cfg.noise_seed = 42;
  cfg.noise_power_mode = "fixed";
  cfg.fixed_signal_power = 1000000.0;

  std::vector<c16_t> a(8, c16_t{1000, 0});
  std::vector<c16_t> b = a;
  nr_ddchan_state_t state_a;
  nr_ddchan_state_t state_b;
  ASSERT_EQ(nr_ddchan_init(&state_a, &cfg), 0);
  ASSERT_EQ(nr_ddchan_init(&state_b, &cfg), 0);
  apply(&state_a, a, 0);
  apply(&state_b, b, 0);
  expect_samples_eq(a, b);
  nr_ddchan_free(&state_a);
  nr_ddchan_free(&state_b);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
