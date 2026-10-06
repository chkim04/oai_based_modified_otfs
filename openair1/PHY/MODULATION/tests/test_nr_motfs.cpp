/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

// !!!!!!!!! for modified OTFS !!!!!!!!!
#include "gtest/gtest.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

extern "C" {
#include "openair1/PHY/MODULATION/nr_motfs.h"
}

namespace {

// !!!!!!!!! for modified OTFS !!!!!!!!!
c16_t sample_value(int t, int m)
{
  const int real = ((37 * t + 19 * m + 11) % 1001) - 500;
  const int imag = ((23 * t - 29 * m + 503) % 1001) - 500;
  return c16_t{static_cast<int16_t>(real), static_cast<int16_t>(imag)};
}

} // namespace

// !!!!!!!!! for modified OTFS !!!!!!!!!
TEST(NrMotfsTest, NEqualsOneIsIdentity)
{
  constexpr uint16_t M = 24;
  constexpr uint8_t N = 1;
  std::vector<c16_t> in(M * N);
  std::vector<c16_t> tmp(M * N);
  std::vector<c16_t> out(M * N);

  for (uint16_t m = 0; m < M; m++)
    in[m] = sample_value(0, m);

  ASSERT_EQ(nr_motfs_time_precoding(in.data(), tmp.data(), M, N), 0);
  ASSERT_EQ(nr_motfs_time_deprecoding(tmp.data(), out.data(), M, N), 0);

  for (uint16_t m = 0; m < M; m++) {
    EXPECT_EQ(tmp[m].r, in[m].r);
    EXPECT_EQ(tmp[m].i, in[m].i);
    EXPECT_EQ(out[m].r, in[m].r);
    EXPECT_EQ(out[m].i, in[m].i);
  }
}

// !!!!!!!!! for modified OTFS !!!!!!!!!
TEST(NrMotfsTest, RoundTripAllSupportedN)
{
  constexpr uint16_t M = 17;
  int max_error = 0;
  double mse = 0.0;
  int samples = 0;

  for (uint8_t N = NR_MOTFS_MIN_N; N <= NR_MOTFS_MAX_N; N++) {
    std::vector<c16_t> in(M * N);
    std::vector<c16_t> precoded(M * N);
    std::vector<c16_t> out(M * N);

    for (uint8_t t = 0; t < N; t++) {
      for (uint16_t m = 0; m < M; m++)
        in[t * M + m] = sample_value(t, m);
    }

    ASSERT_EQ(nr_motfs_time_precoding(in.data(), precoded.data(), M, N), 0);
    ASSERT_EQ(nr_motfs_time_deprecoding(precoded.data(), out.data(), M, N), 0);

    for (size_t i = 0; i < in.size(); i++) {
      const int err_r = std::abs((int)out[i].r - (int)in[i].r);
      const int err_i = std::abs((int)out[i].i - (int)in[i].i);
      max_error = std::max(max_error, std::max(err_r, err_i));
      mse += (double)err_r * (double)err_r;
      mse += (double)err_i * (double)err_i;
      samples += 2;
    }
  }

  mse /= (double)samples;
  std::cout << "MOTFS c16 round-trip max_error=" << max_error << " mse=" << mse << std::endl;
  EXPECT_LE(max_error, 3);
  EXPECT_LE(mse, 1.0);
}

// !!!!!!!!! for modified OTFS !!!!!!!!!
TEST(NrMotfsTest, RejectsInvalidArguments)
{
  c16_t in[NR_MOTFS_MAX_N] = {};
  c16_t out[NR_MOTFS_MAX_N] = {};

  EXPECT_EQ(nr_motfs_time_precoding(nullptr, out, 1, 1), -1);
  EXPECT_EQ(nr_motfs_time_precoding(in, nullptr, 1, 1), -1);
  EXPECT_EQ(nr_motfs_time_precoding(in, out, 0, 1), -1);
  EXPECT_EQ(nr_motfs_time_precoding(in, out, 1, 0), -1);
  EXPECT_EQ(nr_motfs_time_precoding(in, out, 1, NR_MOTFS_MAX_N + 1), -1);
  EXPECT_EQ(nr_motfs_time_precoding(in, in, 1, 2), -1);

  EXPECT_EQ(nr_motfs_time_deprecoding(nullptr, out, 1, 1), -1);
  EXPECT_EQ(nr_motfs_time_deprecoding(in, nullptr, 1, 1), -1);
  EXPECT_EQ(nr_motfs_time_deprecoding(in, out, 0, 1), -1);
  EXPECT_EQ(nr_motfs_time_deprecoding(in, out, 1, 0), -1);
  EXPECT_EQ(nr_motfs_time_deprecoding(in, out, 1, NR_MOTFS_MAX_N + 1), -1);
  EXPECT_EQ(nr_motfs_time_deprecoding(in, in, 1, 2), -1);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
