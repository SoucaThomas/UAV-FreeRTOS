#ifndef CONTROL_TASK_HPP
#define CONTROL_TASK_HPP

#include "core/bus.hpp"
#include "core/messages.hpp"
#include "core/pid.hpp"
#include "log.hpp"
#include "task.hpp"

class ControlTask : public Task<ControlTask> {
  PID pitchPid;
  PID rollPid;
  Logger log;

 public:
  ControlTask()
      : pitchPid(2.0f, 0.1f, 0.5f, -500.0f, 500.0f),
        rollPid(2.0f, 0.1f, 0.5f, -500.0f, 500.0f),
        log("Control") {}

  void run() {
    log.info("Control task started");

    const float dt = 0.01f;  // 100hz

    while (true) {
      SensorMsg sensor;

      if (bus.wait(sensor)) {
        // TODO: currently we aim target to 0 -> should come from radio input
        float targetPitch = 0.0f;
        float targetRoll = 0.0f;

        float pitchCorrection = pitchPid.compute(targetPitch, sensor.pitch, dt);
        float rollCorrection = rollPid.compute(targetRoll, sensor.roll, dt);

        ServoMsg servos = {
            .elevator = (uint16_t)clamp(1500 + (int)pitchCorrection, 1000, 2000),
            .aileronL = (uint16_t)clamp(1500 + (int)rollCorrection, 1000, 2000),
            .aileronR = (uint16_t)clamp(1500 - (int)rollCorrection, 1000, 2000),
            .rudder = 1500,    // no yaw yet
            .throttle = 1000,  // TODO: pass from the radio
            .timestamp = xTaskGetTickCount(),
        };

        bus.publish(servos);
      }

      delay(10);
    }
  }
};

#endif