#ifndef LOG_HPP
#define LOG_HPP

#include "FreeRTOS.h"
#include "drivers/uart.hpp"
#include "task.h"

namespace Color {
constexpr const char* RESET = "\033[0m";
constexpr const char* RED = "\033[31m";
constexpr const char* GREEN = "\033[32m";
constexpr const char* YELLOW = "\033[33m";
constexpr const char* CYAN = "\033[36m";
constexpr const char* GRAY = "\033[90m";
}  // namespace Color

enum class LogLevel : uint8_t {
  DEBUG = 0,
  INFO = 1,
  WARN = 2,
  ERROR = 3,
};

class Logger {
 private:
  static Uart uart;
  static LogLevel minLevel;
  const char* tag;

  void printTimestamp() {
    uint32_t ms = xTaskGetTickCount();
    uint32_t s = ms / 1000;
    uint32_t frac = ms % 1000;

    uart.print(Color::GREEN);
    uart.print("[");
    uart.printInt(s);
    uart.print(".");
    if (frac < 100) uart.print("0");
    if (frac < 10) uart.print("0");
    uart.printInt(frac);
    uart.print("] ");
    uart.print(Color::RESET);
  }

  void printTag() {
    uart.print(Color::YELLOW);
    uart.print(tag);
    uart.print(": ");
    uart.print(Color::RESET);
  }

  void printLevel(LogLevel level) {
    switch (level) {
      case LogLevel::DEBUG:
        uart.print(Color::CYAN);
        uart.print("DBG ");
        break;
      case LogLevel::INFO:
        uart.print(Color::GREEN);
        uart.print("INF ");
        break;
      case LogLevel::WARN:
        uart.print(Color::YELLOW);
        uart.print("WRN ");
        break;
      case LogLevel::ERROR:
        uart.print(Color::RED);
        uart.print("ERR ");
        break;
    }
    uart.print(Color::RESET);
  }

  // Type-safe print helpers
  void printValue(int32_t v) { uart.printInt(v); }
  void printValue(uint32_t v) { uart.printInt(v); }
  void printValue(int16_t v) { uart.printInt(v); }
  void printValue(uint16_t v) { uart.printInt(v); }
  void printValue(float v) { uart.printFloat(v, 2); }
  void printValue(uint8_t v) { uart.printHex(v); }
  void printValue(const char* v) { uart.print(v); }

  void printFmt(const char* fmt) {
    while (*fmt) {
      uart.write(*fmt++);
    }
  }

  template <typename T, typename... Args>
  void printFmt(const char* fmt, T value, Args... args) {
    while (*fmt) {
      if (*fmt == '%' && *(fmt + 1)) {
        fmt++;
        if (*fmt == '%') {
          uart.write('%');
          fmt++;
          continue;
        }
        // Skip the format character — type is determined by the argument
        fmt++;
        printValue(value);
        printFmt(fmt, args...);
        return;
      }
      uart.write(*fmt++);
    }
  }

  template <typename... Args>
  void log(LogLevel level, const char* fmt, Args... args) {
    if (level < minLevel) return;
    printTimestamp();
    printLevel(level);
    printTag();
    printFmt(fmt, args...);
    uart.print("\r\n");
  }

 public:
  Logger(const char* tag) : tag(tag) {}

  static void setLevel(LogLevel level) { minLevel = level; }

  template <typename... Args>
  void debug(const char* fmt, Args... args) {
    log(LogLevel::DEBUG, fmt, args...);
  }

  template <typename... Args>
  void info(const char* fmt, Args... args) {
    log(LogLevel::INFO, fmt, args...);
  }

  template <typename... Args>
  void warn(const char* fmt, Args... args) {
    log(LogLevel::WARN, fmt, args...);
  }

  template <typename... Args>
  void error(const char* fmt, Args... args) {
    log(LogLevel::ERROR, fmt, args...);
  }
};

#endif
