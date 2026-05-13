#ifndef NRF24L01_TEST_TASK_HPP
#define NRF24L01_TEST_TASK_HPP

#include "drivers/nrf24l01.hpp"
#include "drivers/spi.hpp"
#include "drivers/uart.hpp"
#include "led.hpp"
#include "task.hpp"

class Nrf24l01_test_task : public Task<Nrf24l01_test_task> {
 private:
  Spi spi;
  Nrf24l01 nrf;
  Led led;
  Uart uart;

 public:
  Nrf24l01_test_task()
      : spi(SPI1, GPIOA, 4), nrf(&spi, GPIOA, 3), led(GPIOC, 13), uart(USART1, 115200){};

  void run() {
    // Fix SPI pointer after memcpy
    nrf.setSpi(&spi);

    uart.print("NRF register test\r\n");
    delay(100);

    // Read STATUS
    uart.print("STATUS: 0x");
    uart.printHex(nrf.readReg(NRF_STATUS));
    uart.print("\r\n");

    // Full init (channel 0 — RF_CH is defective on this chip)
    uart.print("Initializing NRF...\r\n");
    nrf.init(true, 0);
    uart.print("Init done\r\n\r\n");

    // Verify all registers
    uart.print("CONFIG:   0x");
    uart.printHex(nrf.readReg(0x00));
    uart.print(" (expect 0x0B)\r\n");
    uart.print("EN_AA:    0x");
    uart.printHex(nrf.readReg(0x01));
    uart.print(" (expect 0x00)\r\n");
    uart.print("RF_CH:    0x");
    uart.printHex(nrf.readReg(0x05));
    uart.print(" (ch 0 - stuck)\r\n");
    uart.print("RF_SETUP: 0x");
    uart.printHex(nrf.readReg(0x06));
    uart.print(" (expect 0x06)\r\n");
    uart.print("STATUS:   0x");
    uart.printHex(nrf.readReg(0x07));
    uart.print("\r\n");
    uart.print("RX_PW_P0: 0x");
    uart.printHex(nrf.readReg(0x11));
    uart.print(" (expect 0x20)\r\n");

    while (true) {
      led.toggle();
      delay(1000);
    }
  }
};

#endif