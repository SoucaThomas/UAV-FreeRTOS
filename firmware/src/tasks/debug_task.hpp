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

    while (true) {
      SensorMsg sensor;

      if (bus.peek(sensor) && sensor.timestamp != lastSensorTs) {
        lastSensorTs = sensor.timestamp;
        log.debug("pitch=%f roll=%f", sensor.pitch, sensor.roll);
      }

      delay(100);
    }
  }
};

#endif
