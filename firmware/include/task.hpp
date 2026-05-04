#ifndef TASK_HPP
#define TASK_HPP

#include "FreeRTOS.h"
#include "task.h"

template <typename Derived>
class Task {
 private:
  TaskHandle_t handle;

  static void trampoline(void* param) { static_cast<Derived*>(param)->run(); }

 public:
  Task() : handle(nullptr) {}

  void start(const char* name, uint16_t stackSize, UBaseType_t priority) {
    // Copy this object onto the FreeRTOS heap so it outlives the caller's stack
    Derived* copy = static_cast<Derived*>(pvPortMalloc(sizeof(Derived)));
    if (copy == nullptr) return;
    memcpy(copy, static_cast<Derived*>(this), sizeof(Derived));
    xTaskCreate(trampoline, name, stackSize, copy, priority, &copy->handle);
  }

  void delay(uint32_t ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }
};

#endif
