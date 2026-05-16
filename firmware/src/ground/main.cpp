#include "core/bus.hpp"
#include "core/fault_handler.hpp"
#include "core/system.hpp"
#include "ground/radio_tx_task.hpp"
#include "log.hpp"

Bus bus;
Uart Logger::uart(USART1, 115200);
LogLevel Logger::minLevel = LogLevel::INFO;
SemaphoreHandle_t Logger::mutex = nullptr;

int main() {
  bus.init();
  Logger::init();

  RadioTxTask radioTxTask;
  radioTxTask.start("radioTx", 512, 2);

  vTaskStartScheduler();
  while (true) {
  }
}
