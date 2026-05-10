#include <gtest/gtest.h>

#include "core/state_machine.hpp"

// --- Boot sequence ---
TEST(StateMachine, StartsInBoot) {
  StateMachine sm;
  EXPECT_EQ(sm.getState(), State::BOOT);
}

TEST(StateMachine, BootToCalibrating) {
  StateMachine sm;
  sm.startCalibration();
  EXPECT_EQ(sm.getState(), State::CALIBRATING);
}

TEST(StateMachine, CalibratingToDisarmed) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  EXPECT_EQ(sm.getState(), State::DISARMED);
}

TEST(StateMachine, CannotSkipCalibration) {
  StateMachine sm;
  sm.calibrationDone();
  EXPECT_EQ(sm.getState(), State::BOOT);
}

// --- Arming ---
TEST(StateMachine, ArmWhenAllConditionsMet) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 1000);
  EXPECT_EQ(sm.getState(), State::ARMED);
}

TEST(StateMachine, CannotArmWithoutCalibration) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(false, true, true, 15.0f, 14.0f, 1000);
  EXPECT_EQ(sm.getState(), State::DISARMED);
}

TEST(StateMachine, CannotArmWithoutRadio) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, false, true, 15.0f, 14.0f, 1000);
  EXPECT_EQ(sm.getState(), State::DISARMED);
}

TEST(StateMachine, CannotArmThrottleNotIdle) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, false, 15.0f, 14.0f, 1000);
  EXPECT_EQ(sm.getState(), State::DISARMED);
}

TEST(StateMachine, CannotArmLowBattery) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 13.0f, 14.0f, 1000);
  EXPECT_EQ(sm.getState(), State::DISARMED);
}

TEST(StateMachine, CannotArmFromBoot) {
  StateMachine sm;
  sm.arm(true, true, true, 15.0f, 14.0f, 1000);
  EXPECT_EQ(sm.getState(), State::BOOT);
}

// --- Disarming ---
TEST(StateMachine, DisarmFromArmed) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 1000);
  sm.disarm(true);
  EXPECT_EQ(sm.getState(), State::DISARMED);
}

TEST(StateMachine, CannotDisarmThrottleHigh) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 1000);
  sm.disarm(false);
  EXPECT_EQ(sm.getState(), State::ARMED);
}

TEST(StateMachine, DisarmFromManual) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 1000);
  sm.setManual();
  sm.disarm(true);
  EXPECT_EQ(sm.getState(), State::DISARMED);
}

// --- Flight modes ---
TEST(StateMachine, ArmedToManual) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 1000);
  sm.setManual();
  EXPECT_EQ(sm.getState(), State::MANUAL);
}

TEST(StateMachine, ArmedToStabilize) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 1000);
  sm.setStabilize();
  EXPECT_EQ(sm.getState(), State::STABILIZE);
}

TEST(StateMachine, ManualToStabilize) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 1000);
  sm.setManual();
  sm.setStabilize();
  EXPECT_EQ(sm.getState(), State::STABILIZE);
}

TEST(StateMachine, StabilizeToManual) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 1000);
  sm.setStabilize();
  sm.setManual();
  EXPECT_EQ(sm.getState(), State::MANUAL);
}

TEST(StateMachine, CannotSetManualFromDisarmed) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.setManual();
  EXPECT_EQ(sm.getState(), State::DISARMED);
}

// --- Failsafe ---
TEST(StateMachine, FailsafeShortAfter100ms) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 0);
  sm.setManual();
  sm.updateRadio(true, 1000);
  sm.updateRadio(false, 1200);
  EXPECT_EQ(sm.getState(), State::FAILSAFE_SHORT);
}

TEST(StateMachine, NoFailsafeUnder100ms) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 0);
  sm.setManual();
  sm.updateRadio(true, 1000);
  sm.updateRadio(false, 1050);
  EXPECT_EQ(sm.getState(), State::MANUAL);
}

TEST(StateMachine, FailsafeLongAfter10s) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 0);
  sm.setManual();
  sm.updateRadio(true, 1000);
  sm.updateRadio(false, 12000);
  EXPECT_EQ(sm.getState(), State::FAILSAFE_LONG);
}

TEST(StateMachine, RecoverFromFailsafeShort) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 0);
  sm.setManual();
  sm.updateRadio(true, 1000);
  sm.updateRadio(false, 1200);
  EXPECT_EQ(sm.getState(), State::FAILSAFE_SHORT);
  sm.updateRadio(true, 1500);
  EXPECT_EQ(sm.getState(), State::MANUAL);
}

TEST(StateMachine, RecoverToStabilizeIfWasStabilize) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 0);
  sm.setStabilize();
  sm.updateRadio(true, 1000);
  sm.updateRadio(false, 1200);
  EXPECT_EQ(sm.getState(), State::FAILSAFE_SHORT);
  sm.updateRadio(true, 1500);
  EXPECT_EQ(sm.getState(), State::STABILIZE);
}

TEST(StateMachine, NoRecoverFromFailsafeLong) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 0);
  sm.setManual();
  sm.updateRadio(true, 1000);
  sm.updateRadio(false, 12000);
  EXPECT_EQ(sm.getState(), State::FAILSAFE_LONG);
  sm.updateRadio(true, 13000);
  EXPECT_EQ(sm.getState(), State::FAILSAFE_LONG);
}

TEST(StateMachine, NoFailsafeWhenDisarmed) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.updateRadio(false, 50000);
  EXPECT_EQ(sm.getState(), State::DISARMED);
}

// --- Emergency ---
TEST(StateMachine, EmergencyFromAnyState) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 1000);
  sm.setManual();
  sm.emergency();
  EXPECT_EQ(sm.getState(), State::EMERGENCY);
}

// --- Auto-disarm ---
TEST(StateMachine, AutoDisarmAfter60sNoThrottle) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 0);
  sm.checkAutoDisarm(61000);
  EXPECT_EQ(sm.getState(), State::DISARMED);
}

TEST(StateMachine, NoAutoDisarmIfThrottleUsed) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 0);
  sm.notifyThrottle(true);
  sm.checkAutoDisarm(61000);
  EXPECT_EQ(sm.getState(), State::ARMED);
}

TEST(StateMachine, NoAutoDisarmBefore60s) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 0);
  sm.checkAutoDisarm(30000);
  EXPECT_EQ(sm.getState(), State::ARMED);
}

// --- Output queries ---
TEST(StateMachine, MotorDisabledWhenDisarmed) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  EXPECT_FALSE(sm.motorEnabled());
}

TEST(StateMachine, MotorEnabledWhenArmed) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 1000);
  EXPECT_TRUE(sm.motorEnabled());
}

TEST(StateMachine, MotorEnabledInFailsafeShort) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 0);
  sm.setManual();
  sm.updateRadio(true, 1000);
  sm.updateRadio(false, 1200);
  EXPECT_TRUE(sm.motorEnabled());
}

TEST(StateMachine, MotorDisabledInFailsafeLong) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 0);
  sm.setManual();
  sm.updateRadio(true, 1000);
  sm.updateRadio(false, 12000);
  EXPECT_FALSE(sm.motorEnabled());
}

TEST(StateMachine, MotorDisabledInEmergency) {
  StateMachine sm;
  sm.emergency();
  EXPECT_FALSE(sm.motorEnabled());
}

TEST(StateMachine, StabilizeEnabledInFailsafeShort) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 0);
  sm.setManual();
  sm.updateRadio(true, 1000);
  sm.updateRadio(false, 1200);
  EXPECT_TRUE(sm.stabilizeEnabled());
}

TEST(StateMachine, IsSafeWhenDisarmed) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  EXPECT_TRUE(sm.isSafe());
}

TEST(StateMachine, NotSafeWhenFlying) {
  StateMachine sm;
  sm.startCalibration();
  sm.calibrationDone();
  sm.arm(true, true, true, 15.0f, 14.0f, 1000);
  sm.setManual();
  EXPECT_FALSE(sm.isSafe());
}

// --- State names ---
TEST(StateMachine, StateNames) {
  StateMachine sm;
  EXPECT_STREQ(sm.getStateName(), "BOOT");
  sm.startCalibration();
  EXPECT_STREQ(sm.getStateName(), "CALIBRATING");
  sm.calibrationDone();
  EXPECT_STREQ(sm.getStateName(), "DISARMED");
}
