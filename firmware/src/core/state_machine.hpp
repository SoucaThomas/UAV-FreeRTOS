#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include <stdint.h>

enum class State : uint8_t {
  BOOT,
  CALIBRATING,
  DISARMED,
  ARMED,
  MANUAL,
  STABILIZE,
  FAILSAFE_SHORT,
  FAILSAFE_LONG,
  EMERGENCY
};

class StateMachine {
 private:
  State state;
  State prevFlightMode;
  uint32_t lastRadioMs;
  uint32_t armedAtMs;
  bool throttleWasUsed;

 public:
  StateMachine()
      : state(State::BOOT),
        prevFlightMode(State::MANUAL),
        lastRadioMs(0),
        armedAtMs(0),
        throttleWasUsed(false) {}

  State getState() const { return state; }

  const char* getStateName() const {
    switch (state) {
      case State::BOOT:
        return "BOOT";
      case State::CALIBRATING:
        return "CALIBRATING";
      case State::DISARMED:
        return "DISARMED";
      case State::ARMED:
        return "ARMED";
      case State::MANUAL:
        return "MANUAL";
      case State::STABILIZE:
        return "STABILIZE";
      case State::FAILSAFE_SHORT:
        return "FAILSAFE_SHORT";
      case State::FAILSAFE_LONG:
        return "FAILSAFE_LONG";
      case State::EMERGENCY:
        return "EMERGENCY";
    }
    return "UNKNOWN";
  }

  // --- Guards ---
  bool canArm(bool gyroCalibrated, bool radioValid, bool throttleIdle, float batteryVolt,
              float minVolt) const {
    return state == State::DISARMED && gyroCalibrated && radioValid && throttleIdle &&
           batteryVolt >= minVolt;
  }

  bool canDisarm(bool throttleIdle) const {
    return (state == State::ARMED || state == State::MANUAL || state == State::STABILIZE) &&
           throttleIdle;
  }

  // --- Transitions ---
  void startCalibration() {
    if (state == State::BOOT) {
      state = State::CALIBRATING;
    }
  }

  void calibrationDone() {
    if (state == State::CALIBRATING) {
      state = State::DISARMED;
    }
  }

  void arm(bool gyroCalibrated, bool radioValid, bool throttleIdle, float batteryVolt,
           float minVolt, uint32_t nowMs) {
    if (canArm(gyroCalibrated, radioValid, throttleIdle, batteryVolt, minVolt)) {
      state = State::ARMED;
      armedAtMs = nowMs;
      throttleWasUsed = false;
    }
  }

  void disarm(bool throttleIdle) {
    if (canDisarm(throttleIdle)) {
      state = State::DISARMED;
    }
  }

  void setManual() {
    if (state == State::ARMED || state == State::STABILIZE) {
      state = State::MANUAL;
      prevFlightMode = State::MANUAL;
    }
  }

  void setStabilize() {
    if (state == State::ARMED || state == State::MANUAL) {
      state = State::STABILIZE;
      prevFlightMode = State::STABILIZE;
    }
  }

  // --- Safety ---
  void updateRadio(bool radioValid, uint32_t nowMs) {
    if (radioValid) {
      lastRadioMs = nowMs;

      // Recover from short failsafe
      if (state == State::FAILSAFE_SHORT) {
        state = prevFlightMode;
      }
    } else if (state == State::MANUAL || state == State::STABILIZE) {
      uint32_t lostMs = nowMs - lastRadioMs;

      if (lostMs > 10000) {
        state = State::FAILSAFE_LONG;
      } else if (lostMs > 100) {
        state = State::FAILSAFE_SHORT;
      }
    }
  }

  void emergency() { state = State::EMERGENCY; }

  void notifyThrottle(bool aboveIdle) {
    if (aboveIdle) throttleWasUsed = true;
  }

  // Auto-disarm: armed for 60s but never took off
  void checkAutoDisarm(uint32_t nowMs) {
    if (state == State::ARMED && !throttleWasUsed && (nowMs - armedAtMs) > 60000) {
      state = State::DISARMED;
    }
  }

  // --- Output queries ---
  bool motorEnabled() const {
    return state == State::ARMED || state == State::MANUAL || state == State::STABILIZE ||
           state == State::FAILSAFE_SHORT;
  }

  bool stabilizeEnabled() const {
    return state == State::STABILIZE || state == State::FAILSAFE_SHORT;
  }

  bool isFlying() const {
    return state == State::MANUAL || state == State::STABILIZE || state == State::FAILSAFE_SHORT ||
           state == State::FAILSAFE_LONG;
  }

  bool isSafe() const {
    return state == State::BOOT || state == State::CALIBRATING || state == State::DISARMED ||
           state == State::EMERGENCY;
  }
};

#endif
