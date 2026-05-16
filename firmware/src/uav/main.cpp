#include "config_store.hpp"
#include "core/bist.hpp"
#include "core/bus.hpp"
#include "core/fault_handler.hpp"
#include "core/system.hpp"
#include "log.hpp"
#include "tasks/control_task.hpp"
#include "tasks/debug_task.hpp"
#include "tasks/logger_task.hpp"
#include "tasks/radio_rx_task.hpp"
#include "tasks/sensor_task.hpp"

Bus bus;
Uart Logger::uart(USART1, 115200);
LogLevel Logger::minLevel = LogLevel::INFO;
SemaphoreHandle_t Logger::mutex = nullptr;

int main() {
  bus.init();
  Logger::init();

  printBootReason(Logger::getUart());

  initCrashLog();
  crashLog->dump(Logger::getUart());

  Config config = ConfigStore::load(*crashFlash);

  BistResult bist = runBist(Logger::getUart());

  SensorTask sensorTask;
  ControlTask controlTask(config);
  DebugTask debugTask;
  LoggerTask loggerTask;
  RadioRxTask radioRxTask(config);

  // Only start sensor/control tasks if IMU is present
  if (bist.imu) {
    sensorTask.start("sensor", 512, 3);
    controlTask.start("control", 512, 2);
  }

  radioRxTask.start("radioRx", 512, 2);
  debugTask.start("debug", 512, 1);
  loggerTask.start("logger", 512, 1);

  // Start watchdog last — 2s timeout, fed from idle hook
  iwdgInit(2000);

  vTaskStartScheduler();
  while (true) {
  }
}
