#ifndef DEBUG_TASK_HPP
#define DEBUG_TASK_HPP

#include "core/bus.hpp"
#include "drivers/uart.hpp"
#include "task.hpp"

class DebugTask : public Task<DebugTask> {
 private:
  Uart uart;

 public:
  DebugTask() : uart(USART1, 115200){};

  void run() {
    uart.print("Debug Task Started:\r\n");

    uint32_t lastSensorTs = 0;

    while (true) {
      SensorMsg sensor;

      if (bus.peek(sensor) && sensor.timestamp != lastSensorTs) {
        lastSensorTs = sensor.timestamp;

        uart.print("P:");
        uart.printFloat(sensor.pitch, 2);
        uart.print(" R:");
        uart.printFloat(sensor.roll, 2);
        uart.print("\r\n");
      }

      delay(100);
    }
  }
};

#endif