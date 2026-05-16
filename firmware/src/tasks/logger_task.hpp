#ifndef LOGGER_TASK_HPP
#define LOGGER_TASK_HPP

#include "core/bus.hpp"
#include "core/messages.hpp"
#include "drivers/spi.hpp"
#include "drivers/w25q64.hpp"
#include "flash_map.hpp"
#include "log.hpp"
#include "stm32f411xe.h"
#include "task.hpp"

class LoggerTask : public Task<LoggerTask> {
 private:
  Spi spi;
  W25q64 flash;
  Logger log;

  uint32_t writeAddr;
  uint32_t entryCount;
  bool logging;

  void alignToPage() {
    uint32_t pageOffset = writeAddr % 256;
    if (pageOffset + sizeof(LogEntry) > 256) {
      writeAddr = (writeAddr / 256 + 1) * 256;
    }
  }

  void writeEntry(const LogEntry& entry) {
    // Skip to next page if entry would cross boundary
    alignToPage();

    if (writeAddr + sizeof(LogEntry) >= FLASH_SIZE_END) {
      logging = false;
      log.warn("Flash full — %d entries logged", entryCount);
      return;
    }

    // Erase sector when we hit a new one
    if (writeAddr % 4096 == 0) {
      flash.eraseSector(writeAddr);
    }

    flash.writePage(writeAddr, (const uint8_t*)&entry, sizeof(LogEntry));
    writeAddr += sizeof(LogEntry);
    entryCount++;
  }

 public:
  LoggerTask()
      : spi(SPI1, GPIOA, 4),
        flash(&spi),
        log("Logger Task"),
        writeAddr(FLASH_BLACKBOX_ADDR),
        entryCount(0),
        logging(false) {}

  void run() {
    flash.setSpi(&spi);
    uint32_t id = flash.readId();
    log.info("Flash ID: 0x%x%x%x", (uint8_t)(id >> 16), (uint8_t)(id >> 8), (uint8_t)id);

    // Erase first blackbox sector to be ready
    log.info("Erasing blackbox sector...");
    flash.eraseSector(FLASH_BLACKBOX_ADDR);
    log.info("Ready to log");

    logging = true;
    uint32_t logStartTime = xTaskGetTickCount();

    while (true) {
      // Auto-dump after 10 seconds of logging
      // if (logging && (xTaskGetTickCount() - logStartTime) > 10000) {
      //   logging = false;
      //   log.info("Logging stopped — %d entries", entryCount);
      //   delay(1000);
      //   dumpLog();
      //   while (true) delay(1000);  // done, just idle
      // }

      if (!logging) {
        delay(100);
        continue;
      }

      // Build log entry from bus data
      SensorMsg sensor;
      ServoMsg servo;

      if (!bus.peek(sensor)) {
        delay(10);
        continue;
      }

      LogEntry entry = {
          .timestamp = xTaskGetTickCount(),
          .pitch = sensor.pitch,
          .roll = sensor.roll,
          .gyroX = sensor.gyroX,
          .gyroY = sensor.gyroY,
          .gyroZ = sensor.gyroZ,
          .accelX = sensor.accelX,
          .accelY = sensor.accelY,
          .accelZ = sensor.accelZ,
      };

      if (bus.peek(servo)) {
        entry.elevator = servo.elevator;
        entry.aileronL = servo.aileronL;
        entry.aileronR = servo.aileronR;
        entry.rudder = servo.rudder;
        entry.throttle = servo.throttle;
      }

      // Write entry to flash
      writeEntry(entry);

      delay(10);  // 100Hz logging
    }
  }

  void dumpLog() {
    log.info("Dumping %d entries...", entryCount);

    uint32_t addr = 0;
    for (uint32_t i = 0; i < entryCount; i++) {
      uint32_t pageOffset = addr % 256;
      if (pageOffset + sizeof(LogEntry) > 256) {
        addr = (addr / 256 + 1) * 256;
      }

      LogEntry entry;
      flash.read(addr, (uint8_t*)&entry, sizeof(LogEntry));
      addr += sizeof(LogEntry);

      log.info("[%d] p=%f r=%f E=%d aL=%d aR=%d", entry.timestamp, entry.pitch, entry.roll,
               entry.elevator, entry.aileronL, entry.aileronR);
    }

    log.info("Dump complete");
  }
};

#endif