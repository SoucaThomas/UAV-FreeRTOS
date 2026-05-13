#ifndef DEBUG_TASK_HPP
#define DEBUG_TASK_HPP

#include "core/bus.hpp"
#include "log.hpp"
#include "task.hpp"

class DebugTask : public Task<DebugTask> {
 private:
  Logger log;

 public:
  DebugTask() : log("Debug") {};

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
          log.debug("P:%f R:%f | E:%d aL:%d aR:%d", sensor.pitch, sensor.roll, servo.elevator,
                    servo.aileronL, servo.aileronR);
        } else {
          log.debug("P:%f R:%f", sensor.pitch, sensor.roll);
        }

        // Every 5s print task stats
        if (xTaskGetTickCount() % 5000 < 100) {
          TaskStatus_t tasks[10];
          uint32_t totalRunTime;

          UBaseType_t count = uxTaskGetSystemState(tasks, 10, &totalRunTime);

          log.info("--- Task Stats ---");
          for (UBaseType_t i = 0; i < count; i++) {
            uint32_t percent =
                (totalRunTime > 0) ? (tasks[i].ulRunTimeCounter * 100) / totalRunTime : 0;
            log.info("%s: %d%%  stack:%d", tasks[i].pcTaskName, percent,
                     tasks[i].usStackHighWaterMark);
          }
        }
      }

      delay(100);
    }
  }
};

#endif
