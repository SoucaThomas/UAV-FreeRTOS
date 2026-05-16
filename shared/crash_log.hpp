#ifndef CRASH_LOG_HPP
#define CRASH_LOG_HPP

#include <stdint.h>

#include "drivers/uart.hpp"
#include "drivers/w25q64.hpp"
#include "flash_map.hpp"

constexpr uint32_t CRASH_ENTRY_MAGIC = 0xDEAD;
constexpr uint32_t CRASH_DATA_OFFSET = FLASH_CRASH_ADDR + 4;
constexpr uint32_t CRASH_SECTOR_SIZE = 4096;
constexpr uint32_t CRASH_DATA_SIZE = CRASH_SECTOR_SIZE - 4;

struct __attribute__((packed)) CrashEntry {
  uint32_t magic;
  uint32_t tick;
  uint32_t pc;
  uint32_t lr;
  uint32_t sp;
  uint32_t cfsr;
  uint32_t hfsr;
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
};

constexpr uint32_t CRASH_MAX_ENTRIES = CRASH_DATA_SIZE / sizeof(CrashEntry);

class CrashLog {
 private:
  W25q64* flash;

  // Scan for first empty slot (no valid magic)
  uint32_t findNextSlot() {
    for (uint32_t i = 0; i < CRASH_MAX_ENTRIES; i++) {
      uint32_t addr = CRASH_DATA_OFFSET + (i * sizeof(CrashEntry));
      uint32_t magic = 0;

      flash->read(addr, (uint8_t*)&magic, 4);
      if (magic != CRASH_ENTRY_MAGIC) {
        return i;
      }
    }
    // Sector full -> erase sector
    flash->eraseSector(FLASH_CRASH_ADDR);
    return 0;
  }

 public:
  CrashLog(W25q64* flash) : flash(flash) {}

  void setFlash(W25q64* f) { flash = f; }

  // Called from HardFault handler
  void write(const CrashEntry& entry) {
    uint32_t slot = findNextSlot();
    uint32_t addr = CRASH_DATA_OFFSET + (slot * sizeof(CrashEntry));

    flash->writePage(addr, (const uint8_t*)&entry, sizeof(CrashEntry));
  }

  // Called at boot, prints crash history over UART directly
  void dump(Uart& uart) {
    uint32_t count = 0;

    for (uint32_t i = 0; i < CRASH_MAX_ENTRIES; i++) {
      uint32_t addr = CRASH_DATA_OFFSET + (i * sizeof(CrashEntry));
      CrashEntry entry;
      flash->read(addr, (uint8_t*)&entry, sizeof(CrashEntry));

      if (entry.magic != CRASH_ENTRY_MAGIC) break;

      count++;
      uart.print("\033[31m");
      uart.print("  CRASH #");
      uart.printInt(i);
      uart.print(" tick=");
      uart.printInt(entry.tick);
      uart.print(" PC=0x");
      uart.printHex32(entry.pc);
      uart.print(" LR=0x");
      uart.printHex32(entry.lr);
      uart.print("\r\n");
      uart.print("    CFSR=0x");
      uart.printHex32(entry.cfsr);
      uart.print(" HFSR=0x");
      uart.printHex32(entry.hfsr);
      uart.print(" SP=0x");
      uart.printHex32(entry.sp);
      uart.print("\033[0m\r\n");
    }

    if (count == 0) {
      uart.print("\033[32m");
      uart.print("  [PASS] No crash logs\r\n");
      uart.print("\033[0m");
    } else {
      uart.print("\033[31m");
      uart.print("  [WARN] ");
      uart.printInt(count);
      uart.print(" crash(es) recorded\r\n");
      uart.print("\033[0m");
    }
  }

  void clear() { flash->eraseSector(FLASH_CRASH_ADDR); }
};

#endif
