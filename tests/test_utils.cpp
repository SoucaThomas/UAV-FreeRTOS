#include <gtest/gtest.h>

#include "utils.hpp"

// --- Basic clamping ---

TEST(Clamp, ValueWithinRange) {
  EXPECT_EQ(clamp(5, 0, 10), 5);
  EXPECT_FLOAT_EQ(clamp(0.5f, 0.0f, 1.0f), 0.5f);
}

TEST(Clamp, ValueBelowMin) {
  EXPECT_EQ(clamp(-5, 0, 10), 0);
  EXPECT_FLOAT_EQ(clamp(-1.0f, 0.0f, 1.0f), 0.0f);
}

TEST(Clamp, ValueAboveMax) {
  EXPECT_EQ(clamp(15, 0, 10), 10);
  EXPECT_FLOAT_EQ(clamp(2.0f, 0.0f, 1.0f), 1.0f);
}

TEST(Clamp, ValueAtMin) { EXPECT_EQ(clamp(0, 0, 10), 0); }

TEST(Clamp, ValueAtMax) { EXPECT_EQ(clamp(10, 0, 10), 10); }

// --- Edge cases ---

TEST(Clamp, MinEqualsMax) {
  EXPECT_EQ(clamp(5, 3, 3), 3);
  EXPECT_EQ(clamp(1, 3, 3), 3);
  EXPECT_EQ(clamp(3, 3, 3), 3);
}

TEST(Clamp, NegativeRange) {
  EXPECT_EQ(clamp(-5, -10, -1), -5);
  EXPECT_EQ(clamp(-15, -10, -1), -10);
  EXPECT_EQ(clamp(0, -10, -1), -1);
}

TEST(Clamp, FloatPrecision) {
  EXPECT_FLOAT_EQ(clamp(0.0001f, 0.0f, 0.001f), 0.0001f);
  EXPECT_FLOAT_EQ(clamp(-0.0001f, 0.0f, 0.001f), 0.0f);
}

TEST(Clamp, LargeValues) {
  EXPECT_EQ(clamp(1000000, -500, 500), 500);
  EXPECT_EQ(clamp(-1000000, -500, 500), -500);
}
