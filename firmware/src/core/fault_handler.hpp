#ifndef FAULT_HANDLER_HPP
#define FAULT_HANDLER_HPP

#include "crash_log.hpp"
#include "drivers/spi.hpp"
#include "drivers/w25q64.hpp"
#include "log.hpp"

// Placement new (no <new> header with -nostdlib)
inline void* operator new(unsigned int, void* p) { return p; }

// Raw storage — constructed in initCrashLog() to avoid global init order issues
static uint8_t crashSpiBuf[sizeof(Spi)];
static uint8_t crashFlashBuf[sizeof(W25q64)];
static uint8_t crashLogBuf[sizeof(CrashLog)];
static Spi* crashSpi = nullptr;
static W25q64* crashFlash = nullptr;
static CrashLog* crashLog = nullptr;

inline void initCrashLog() {
  crashSpi = new (crashSpiBuf) Spi(SPI1, GPIOA, 4);
  crashFlash = new (crashFlashBuf) W25q64(crashSpi);
  crashLog = new (crashLogBuf) CrashLog(crashFlash);
}

extern "C" {

__attribute__((naked)) void HardFault_Handler(void) {
  __asm volatile(
      "tst lr, #4          \n"
      "ite eq              \n"
      "mrseq r0, msp       \n"
      "mrsne r0, psp       \n"
      "b HardFault_Handle  \n");
}

void HardFault_Handle(uint32_t* stack) {
  Uart& uart = Logger::getUart();

  uart.print("\r\n\033[31m=== HARD FAULT ===\r\n");
  uart.print("  PC=0x");
  uart.printHex32(stack[6]);
  uart.print(" LR=0x");
  uart.printHex32(stack[5]);
  uart.print("\r\n  SP=0x");
  uart.printHex32((uint32_t)stack);
  uart.print(" CFSR=0x");
  uart.printHex32(*(volatile uint32_t*)0xE000ED28);
  uart.print("\r\n  HFSR=0x");
  uart.printHex32(*(volatile uint32_t*)0xE000ED2C);
  uart.print("\r\n  R0=0x");
  uart.printHex32(stack[0]);
  uart.print(" R1=0x");
  uart.printHex32(stack[1]);
  uart.print(" R2=0x");
  uart.printHex32(stack[2]);
  uart.print(" R3=0x");
  uart.printHex32(stack[3]);
  uart.print("\033[0m\r\n");

  if (crashLog) {
    CrashEntry entry = {};
    entry.magic = CRASH_ENTRY_MAGIC;
    entry.tick = SysTick->VAL;
    entry.r0 = stack[0];
    entry.r1 = stack[1];
    entry.r2 = stack[2];
    entry.r3 = stack[3];
    entry.r12 = stack[4];
    entry.lr = stack[5];
    entry.pc = stack[6];
    entry.sp = (uint32_t)stack;
    entry.cfsr = *(volatile uint32_t*)0xE000ED28;
    entry.hfsr = *(volatile uint32_t*)0xE000ED2C;
    crashLog->write(entry);
  }

  NVIC_SystemReset();
}

}  // extern "C"

#endif
