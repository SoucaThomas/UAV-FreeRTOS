#ifndef DEBUG_TASK_HPP
#define DEBUG_TASK_HPP

#include "core/bus.hpp"
#include "log.hpp"
#include "task.hpp"

class DebugTask : public Task<DebugTask> {
 private:
  Logger log;

 public:
  DebugTask() : log("Debug"){};

  void run() {
    log.info("Debug task started");

    uint32_t lastSensorTs = 0;
    uint32_t lastServoTs = 0;

    while (true) {
      SensorMsg sensor;
      if (bus.peek(sensor) && sensor.timestamp != lastSensorTs) {
        lastSensorTs = sensor.timestamp;

        ServoMsg servo;
        if (bus.peek(servo)) {
          lastServoTs = servo.timestamp;
          log.debug("P:%f R:%f | E:%d aL:%d aR:%d",
                    sensor.pitch, sensor.roll,
                    servo.elevator, servo.aileronL, servo.aileronR);
        } else {
          log.debug("P:%f R:%f", sensor.pitch, sensor.roll);
        }
      }

      delay(100);
    }
  }
};

#endif
