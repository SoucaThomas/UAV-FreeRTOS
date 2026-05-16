#ifndef CONTROL_TASK_HPP
#define CONTROL_TASK_HPP

#include "config_store.hpp"
#include "core/bus.hpp"
#include "core/messages.hpp"
#include "core/pid.hpp"
#include "log.hpp"
#include "task.hpp"

class ControlTask : public Task<ControlTask> {
  PID pitchPid;
  PID rollPid;
  Logger log;
  uint16_t servoMin;
  uint16_t servoMax;

 public:
  ControlTask(const Config& cfg)
      : pitchPid(cfg.pitchP, cfg.pitchI, cfg.pitchD, -500.0f, 500.0f),
        rollPid(cfg.rollP, cfg.rollI, cfg.rollD, -500.0f, 500.0f),
        log("Control"),
        servoMin(cfg.servoMin),
        servoMax(cfg.servoMax) {}

  void run() {
    log.info("Control task started");

    const float dt = 0.01f;  // 100hz
    uint16_t center = (servoMin + servoMax) / 2;

    while (true) {
      SensorMsg sensor;

      if (bus.wait(sensor)) {
        // TODO: currently we aim target to 0 -> should come from radio input
        float targetPitch = 0.0f;
        float targetRoll = 0.0f;

        float pitchCorrection = pitchPid.compute(targetPitch, sensor.pitch, dt);
        float rollCorrection = rollPid.compute(targetRoll, sensor.roll, dt);

        ServoMsg servos = {
            .elevator = (uint16_t)clamp(center + (int)pitchCorrection, (int)servoMin, (int)servoMax),
            .aileronL = (uint16_t)clamp(center + (int)rollCorrection, (int)servoMin, (int)servoMax),
            .aileronR = (uint16_t)clamp(center - (int)rollCorrection, (int)servoMin, (int)servoMax),
            .rudder = center,      // no yaw yet
            .throttle = servoMin,  // TODO: pass from the radio
            .timestamp = xTaskGetTickCount(),
        };

        bus.publish(servos);
      }

      delay(10);
    }
  }
};

#endif