#ifndef BIST_HPP
#define BIST_HPP

#include "FreeRTOS.h"
#include "drivers/i2c.hpp"
#include "drivers/spi.hpp"
#include "drivers/uart.hpp"
#include "drivers/w25q64.hpp"

struct BistResult {
  bool fpu;
  bool i2c;
  bool imu;
  bool spi;
  bool flash;
  bool heap;

  bool allPassed() const { return fpu && i2c && imu && spi && flash && heap; }
};

// Runs before FreeRTOS scheduler — no tasks, no mutex, no logger.
// Uses UART directly to avoid any RTOS dependencies.
inline BistResult runBist(Uart& uart) {
  BistResult result = {};

  uart.print("=== Pre-Flight Self Test ===\r\n");

  // FPU
  volatile float a = 3.14f;
  volatile float b = 2.0f;
  volatile float c = a * b;
  result.fpu = (c > 6.27f && c < 6.29f);
  uart.print(result.fpu ? "[PASS] FPU\r\n" : "[FAIL] FPU\r\n");

  // I2C + IMU
  I2c i2c(I2C1);
  uint8_t who = i2c.read(0x68, 0x75);
  result.i2c = (who != 0x00 && who != 0xFF);
  result.imu = (who == 0x68 || who == 0x70);
  uart.print(result.i2c ? "[PASS] I2C\r\n" : "[FAIL] I2C\r\n");
  uart.print(result.imu ? "[PASS] IMU\r\n" : "[FAIL] IMU\r\n");

  // SPI + Flash
  Spi spi(SPI1, GPIOA, 4);
  W25q64 flash(&spi);
  uint32_t jedec = flash.readId();
  uint8_t mfr = (jedec >> 16) & 0xFF;
  result.spi = (jedec != 0x000000 && jedec != 0xFFFFFF);
  result.flash = (mfr == 0xEF || mfr == 0x85);
  uart.print(result.spi ? "[PASS] SPI\r\n" : "[FAIL] SPI\r\n");
  uart.print(result.flash ? "[PASS] Flash\r\n" : "[FAIL] Flash\r\n");

  // Heap
  size_t freeHeap = xPortGetFreeHeapSize();
  result.heap = (freeHeap > 8192);
  uart.print(result.heap ? "[PASS] Heap\r\n" : "[FAIL] Heap\r\n");

  uart.print(result.allPassed() ? "=== ALL TESTS PASSED ===\r\n"
                                 : "=== TESTS FAILED ===\r\n");

  return result;
}

#endif
