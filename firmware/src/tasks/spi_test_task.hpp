#ifndef SPI_TEST_TASK_HPP
#define SPI_TEST_TASK_HPP

#include "drivers/spi.hpp"
#include "led.hpp"
#include "task.hpp"

/*
    Connect A6 to A6 -> loopback test
    If it works the onboard led should blink fast
    If it doesn't work the onboard led should stay solid
*/
class SpiTestTask : public Task<SpiTestTask> {
 private:
  Spi spi;
  Led led;

 public:
  SpiTestTask() : spi(SPI1, GPIOA, 4), led(GPIOC, 13){};

  void run() {
    while (true) {
      uint8_t r1 = spi.transfer(0xA5);
      uint8_t r2 = spi.transfer(0x3C);

      if (r1 == 0xA5 && r2 == 0x3C) {
        // Both loopbacks match — blink fast
        led.toggle();
        delay(100);
      } else {
        // Failed — stay solid (LED on)
        led.on();
        delay(1000);
      }
    }
  }
};

#endif