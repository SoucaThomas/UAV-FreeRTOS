#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "FreeRTOS.h"
#include "log.hpp"
#include "stm32f411xe.h"
#include "task.h"

// --- Boot reason ---

enum class BootReason : uint8_t {
  CLEAN,
  WATCHDOG,
  SOFTWARE_RESET,
  PIN_RESET,
  BROWNOUT,
  UNKNOWN,
};

inline BootReason getBootReason() {
  uint32_t csr = RCC->CSR;
  // Clear reset flags for next boot
  RCC->CSR |= RCC_CSR_RMVF;

  if (csr & RCC_CSR_IWDGRSTF) return BootReason::WATCHDOG;
  if (csr & RCC_CSR_SFTRSTF) return BootReason::SOFTWARE_RESET;
  if (csr & RCC_CSR_PINRSTF) return BootReason::PIN_RESET;
  if (csr & RCC_CSR_BORRSTF) return BootReason::BROWNOUT;
  if (csr & RCC_CSR_LPWRRSTF) return BootReason::CLEAN;
  return BootReason::UNKNOWN;
}

inline void printBootReason(Uart& uart) {
  BootReason reason = getBootReason();
  const char* name;
  const char* color;

  switch (reason) {
    case BootReason::CLEAN:
      name = "clean boot";
      color = "\033[32m";
      break;
    case BootReason::WATCHDOG:
      name = "WATCHDOG RESET";
      color = "\033[31m";
      break;
    case BootReason::SOFTWARE_RESET:
      name = "software reset";
      color = "\033[33m";
      break;
    case BootReason::PIN_RESET:
      name = "pin reset";
      color = "\033[32m";
      break;
    case BootReason::BROWNOUT:
      name = "BROWNOUT";
      color = "\033[31m";
      break;
    default:
      name = "unknown";
      color = "\033[33m";
      break;
  }

  uart.print("  ");
  uart.print(color);
  uart.print("[BOOT] ");
  uart.print(name);
  uart.print("\033[0m\r\n");
}

// --- IWDG watchdog ---

inline void iwdgInit(uint16_t timeoutMs) {
  // Unlock IWDG registers
  IWDG->KR = 0x5555;

  // Prescaler /64 → LSI 32kHz / 64 = 500Hz → 2ms per tick
  IWDG->PR = 4;  // prescaler /64

  // Reload value: timeoutMs / 2
  uint16_t reload = timeoutMs / 2;
  if (reload > 0xFFF) reload = 0xFFF;
  IWDG->RLR = reload;

  // Start watchdog
  IWDG->KR = 0xCCCC;
}

inline void iwdgFeed() { IWDG->KR = 0xAAAA; }

// --- extern "C" hooks and stubs ---

extern "C" {

void vConfigureTimerForRunTimeStats(void) {
  RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
  TIM5->PSC = 1599;
  TIM5->ARR = 0xFFFFFFFF;
  TIM5->CR1 |= TIM_CR1_CEN;
}

uint32_t ulGetRunTimeCounterValue(void) { return TIM5->CNT; }

void vApplicationIdleHook(void) { iwdgFeed(); }

void __cxa_pure_virtual() { while (1); }
void vApplicationMallocFailedHook() { while (1); }

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
  (void)xTask;
  Uart& uart = Logger::getUart();
  uart.print("\r\n\033[31m=== STACK OVERFLOW === task: ");
  uart.print(pcTaskName);
  uart.print("\033[0m\r\n");
  NVIC_SystemReset();
}

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

}  // extern "C"

#endif
