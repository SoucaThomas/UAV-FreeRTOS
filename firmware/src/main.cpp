#include "core/bus.hpp"
#include "log.hpp"
#include "tasks/control_task.hpp"
#include "tasks/debug_task.hpp"
#include "tasks/logger_task.hpp"
#include "tasks/sensor_task.hpp"

Bus bus;
Uart Logger::uart(USART1, 115200);
LogLevel Logger::minLevel = LogLevel::INFO;
SemaphoreHandle_t Logger::mutex = nullptr;

int main() {
  bus.init();
  Logger::init();

  SensorTask sensorTask;
  ControlTask controlTask;
  DebugTask debugTask;
  LoggerTask loggerTask;

  sensorTask.start("sensor", 512, 3);
  controlTask.start("control", 512, 2);
  debugTask.start("debug", 512, 1);
  loggerTask.start("logger", 512, 1);

  vTaskStartScheduler();
  while (true) {
  }
}

extern "C" {
void __cxa_pure_virtual() { while (1); }
void vApplicationMallocFailedHook() { while (1); }

void* memset(void* dst, int val, unsigned int len) {
  unsigned char* p = (unsigned char*)dst;
  while (len--) *p++ = (unsigned char)val;
  return dst;
}

void* memcpy(void* dst, const void* src, unsigned int len) {
  unsigned char* d = (unsigned char*)dst;
  const unsigned char* s = (const unsigned char*)src;
  while (len--) *d++ = *s++;
  return dst;
}
}
