#include "core/bus.hpp"
#include "tasks/debug_task.hpp"
#include "tasks/sensor_task.hpp"

Bus bus;

int main() {
  bus.init();

  SensorTask sensorTask;
  DebugTask debugTask;

  sensorTask.start("sensor_test", 512, 2);
  debugTask.start("debug_task", 512, 1);

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
