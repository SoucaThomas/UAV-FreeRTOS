#include <gtest/gtest.h>

#include "utils.hpp"

// PID includes utils.hpp which is already covered, but PID also
// includes it via its own header. We need to provide the header
// path so let's include it directly.
#include "core/pid.hpp"

// --- P term ---

TEST(PID, POnlyPositiveError) {
  PID pid(2.0f, 0.0f, 0.0f, -500.0f, 500.0f);
  float out = pid.compute(10.0f, 0.0f, 0.01f);
  EXPECT_FLOAT_EQ(out, 20.0f);  // 2.0 * 10
}

TEST(PID, POnlyNegativeError) {
  PID pid(2.0f, 0.0f, 0.0f, -500.0f, 500.0f);
  float out = pid.compute(0.0f, 10.0f, 0.01f);
  EXPECT_FLOAT_EQ(out, -20.0f);  // 2.0 * -10
}

TEST(PID, POnlyZeroError) {
  PID pid(2.0f, 0.0f, 0.0f, -500.0f, 500.0f);
  float out = pid.compute(5.0f, 5.0f, 0.01f);
  EXPECT_FLOAT_EQ(out, 0.0f);
}

// --- I term ---

TEST(PID, ITermAccumulates) {
  PID pid(0.0f, 1.0f, 0.0f, -500.0f, 500.0f);
  float dt = 0.01f;

  // Error = 10 each step, integral grows
  float out1 = pid.compute(10.0f, 0.0f, dt);  // integral = 10*0.01 = 0.1
  float out2 = pid.compute(10.0f, 0.0f, dt);  // integral = 0.2
  float out3 = pid.compute(10.0f, 0.0f, dt);  // integral = 0.3

  EXPECT_GT(out2, out1);
  EXPECT_GT(out3, out2);
}

TEST(PID, ITermAntiWindup) {
  PID pid(0.0f, 100.0f, 0.0f, -500.0f, 500.0f);
  float dt = 0.01f;

  // Pump the integral way up
  for (int i = 0; i < 10000; i++) {
    pid.compute(100.0f, 0.0f, dt);
  }

  // Output should be clamped, not infinite
  float out = pid.compute(100.0f, 0.0f, dt);
  EXPECT_LE(out, 500.0f);
  EXPECT_GE(out, -500.0f);
}

TEST(PID, ITermNegativeAntiWindup) {
  PID pid(0.0f, 100.0f, 0.0f, -500.0f, 500.0f);
  float dt = 0.01f;

  for (int i = 0; i < 10000; i++) {
    pid.compute(-100.0f, 0.0f, dt);
  }

  float out = pid.compute(-100.0f, 0.0f, dt);
  EXPECT_GE(out, -500.0f);
  EXPECT_LE(out, 500.0f);
}

// --- D term ---

TEST(PID, DTermRespondsToChange) {
  PID pid(0.0f, 0.0f, 1.0f, -500.0f, 500.0f);
  float dt = 0.01f;

  pid.compute(0.0f, 0.0f, dt);               // error=0, prevError=0
  float out = pid.compute(10.0f, 0.0f, dt);  // error=10, derivative = (10-0)/0.01 = 1000

  EXPECT_GT(out, 0.0f);  // should be positive (error increasing)
}

TEST(PID, DTermZeroWhenErrorConstant) {
  PID pid(0.0f, 0.0f, 1.0f, -500.0f, 500.0f);
  float dt = 0.01f;

  pid.compute(10.0f, 0.0f, dt);              // first call sets prevError
  float out = pid.compute(10.0f, 0.0f, dt);  // same error, derivative = 0

  EXPECT_FLOAT_EQ(out, 0.0f);
}

// --- Output clamping ---

TEST(PID, OutputClampedMax) {
  PID pid(100.0f, 0.0f, 0.0f, -500.0f, 500.0f);
  float out = pid.compute(100.0f, 0.0f, 0.01f);  // 100 * 100 = 10000
  EXPECT_FLOAT_EQ(out, 500.0f);
}

TEST(PID, OutputClampedMin) {
  PID pid(100.0f, 0.0f, 0.0f, -500.0f, 500.0f);
  float out = pid.compute(0.0f, 100.0f, 0.01f);  // 100 * -100 = -10000
  EXPECT_FLOAT_EQ(out, -500.0f);
}

// --- Reset ---

TEST(PID, ResetClearsState) {
  PID pid(0.0f, 1.0f, 0.0f, -500.0f, 500.0f);
  float dt = 0.01f;

  // Accumulate some integral
  for (int i = 0; i < 100; i++) {
    pid.compute(10.0f, 0.0f, dt);
  }
  float before = pid.compute(10.0f, 0.0f, dt);
  EXPECT_GT(before, 0.0f);

  pid.reset();

  // After reset, I term starts from zero
  float after = pid.compute(10.0f, 0.0f, dt);
  EXPECT_LT(after, before);
}

// --- setGains ---

TEST(PID, SetGainsChangesResponse) {
  PID pid(1.0f, 0.0f, 0.0f, -500.0f, 500.0f);
  float out1 = pid.compute(10.0f, 0.0f, 0.01f);

  pid.reset();
  pid.setGains(5.0f, 0.0f, 0.0f);
  float out2 = pid.compute(10.0f, 0.0f, 0.01f);

  EXPECT_FLOAT_EQ(out1, 10.0f);  // kp=1 * error=10
  EXPECT_FLOAT_EQ(out2, 50.0f);  // kp=5 * error=10
}

// --- Combined PID ---

TEST(PID, ConvergesToTarget) {
  PID pid(1.0f, 0.1f, 0.3f, -500.0f, 500.0f);
  float dt = 0.01f;
  float current = 0.0f;
  float target = 100.0f;

  // Simulate 500 steps of a simple system where
  // current moves toward PID output
  for (int i = 0; i < 500; i++) {
    float correction = pid.compute(target, current, dt);
    current += correction * 0.01f;  // simple plant model
  }

  // Should be close to target (simple plant model overshoots slightly)
  EXPECT_NEAR(current, target, 10.0f);
}

TEST(PID, SymmetricResponse) {
  PID pid(2.0f, 0.0f, 0.0f, -500.0f, 500.0f);

  float pos = pid.compute(10.0f, 0.0f, 0.01f);
  pid.reset();
  float neg = pid.compute(-10.0f, 0.0f, 0.01f);

  EXPECT_FLOAT_EQ(pos, -neg);
}
