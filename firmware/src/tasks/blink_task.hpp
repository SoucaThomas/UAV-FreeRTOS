#ifndef BLINK_TASK_HPP
#define BLINK_TASK_HPP

#include "led.hpp"
#include "task.hpp"

class BlinkTask : public Task<BlinkTask> {
 private:
  Led led;

 public:
  BlinkTask(GPIO_TypeDef* port, uint16_t pin) : led(port, pin) {}

  void run() {
    while (true) {
      led.toggle();
      delay(500);
    }
  }
};

#endif
