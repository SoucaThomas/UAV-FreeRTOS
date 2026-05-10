#ifndef SPI_TEST_TASK_HPP
#define SPI_TEST_TASK_HPP

#include "drivers/spi.hpp"
#include "drivers/uart.hpp"
#include "led.hpp"
#include "task.hpp"

/*
    Connect A6 to A7 -> loopback test
    If it works the onboard led should blink fast
    If it doesn't work the onboard led should stay solid
*/
class SpiTestTask : public Task<SpiTestTask> {
 private:
  Spi spi;
  Led led;
  Uart uart;

 public:
  SpiTestTask() : spi(SPI1, GPIOA, 4), led(GPIOC, 13), uart(USART1, 115200) {};

  void run() {
    uart.print("SPI loopback test\r\n");
    uart.print("Connect PA7 (MOSI) to PA6 (MISO)\r\n\r\n");

    while (true) {
      uint8_t r1 = spi.transfer(0xA5);
      uint8_t r2 = spi.transfer(0x3C);

      if (r1 == 0xA5 && r2 == 0x3C) {
        uart.print("[PASS] TX: 0xA5 -> RX: 0x");
        uart.printHex(r1);
        uart.print("  TX: 0x3C -> RX: 0x");
        uart.printHex(r2);
        uart.print("\r\n");
        led.toggle();
      } else {
        uart.print("[FAIL] TX: 0xA5 -> RX: 0x");
        uart.printHex(r1);
        uart.print("  TX: 0x3C -> RX: 0x");
        uart.printHex(r2);
        uart.print("\r\n");
        led.on();
      }

      delay(500);
    }
  }
};

#endif