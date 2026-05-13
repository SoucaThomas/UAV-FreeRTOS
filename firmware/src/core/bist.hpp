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

// ANSI colors for BIST (can't use Logger before scheduler)
namespace BistColor {
constexpr const char* GREEN = "\033[32m";
constexpr const char* RED = "\033[31m";
constexpr const char* YELLOW = "\033[33m";
constexpr const char* CYAN = "\033[36m";
constexpr const char* RESET = "\033[0m";
}  // namespace BistColor

inline void bistReport(Uart& uart, const char* name, bool passed) {
  uart.print(passed ? BistColor::GREEN : BistColor::RED);
  uart.print(passed ? "  [PASS] " : "  [FAIL] ");
  uart.print(BistColor::RESET);
  uart.print(name);
  uart.print("\r\n");
}

inline BistResult runBist(Uart& uart) {
  BistResult result = {};

  uart.print(BistColor::CYAN);
  uart.print("=== Pre-Flight Self Test ===\r\n");
  uart.print(BistColor::RESET);

  // FPU
  volatile float a = 3.14f;
  volatile float b = 2.0f;
  volatile float c = a * b;
  result.fpu = (c > 6.27f && c < 6.29f);
  bistReport(uart, "FPU", result.fpu);

  // I2C + IMU
  I2c i2c(I2C1);
  uint8_t who = i2c.read(0x68, 0x75);
  result.i2c = (who != 0x00 && who != 0xFF);
  result.imu = (who == 0x68 || who == 0x70);
  bistReport(uart, "I2C", result.i2c);
  bistReport(uart,
             who == 0x68   ? "IMU (MPU6050)"
             : who == 0x70 ? "IMU (MPU6500)"
                           : "IMU (unknown)",
             result.imu);

  // SPI + Flash
  Spi spi(SPI1, GPIOA, 4);
  W25q64 flash(&spi);
  uint32_t jedec = flash.readId();
  uint8_t mfr = (jedec >> 16) & 0xFF;
  result.spi = (jedec != 0x000000 && jedec != 0xFFFFFF);
  result.flash = (mfr == 0xEF || mfr == 0x85);
  bistReport(uart, "SPI", result.spi);
  bistReport(uart, "Flash", result.flash);

  // Heap
  size_t freeHeap = xPortGetFreeHeapSize();
  result.heap = (freeHeap > 8192);
  bistReport(uart, "Heap", result.heap);

  uart.print(result.allPassed() ? BistColor::GREEN : BistColor::RED);
  uart.print(result.allPassed() ? "=== ALL TESTS PASSED ===\r\n" : "=== TESTS FAILED ===\r\n");
  uart.print(BistColor::RESET);

  return result;
}

#endif
