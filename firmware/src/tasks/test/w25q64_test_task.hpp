#ifndef W25Q64_TEST_TASK_HPP
#define W25Q64_TEST_TASK_HPP

#include "drivers/spi.hpp"
#include "drivers/w25q64.hpp"
#include "log.hpp"
#include "stm32f411xe.h"
#include "task.hpp"

class W25q64TestTask : public Task<W25q64TestTask> {
 private:
  Spi spi;
  W25q64 flash;
  Logger log;

 public:
  W25q64TestTask() : spi(SPI1, GPIOA, 4), flash(&spi), log("W25q64 Test Task"){};

  void run() {
    flash.setSpi(&spi);

    // Test 1: JEDEC ID
    uint32_t id = flash.readId();
    log.info("JEDEC ID: 0x%x%x%x", (uint8_t)(id >> 16), (uint8_t)(id >> 8), (uint8_t)id);
    // Expect: 0xEF4017

    // Test 2: Erase sector 0
    log.info("Erasing sector 0...");
    flash.eraseSector(0);
    log.info("Done");

    // Test 3: Write
    uint8_t writeData[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x42};
    log.info("Writing 5 bytes...");
    flash.writePage(0, writeData, 5);

    // Test 4: Read back
    uint8_t readData[5] = {};
    flash.read(0, readData, 5);
    log.info("Read: %x %x %x %x %x", readData[0], readData[1], readData[2], readData[3],
             readData[4]);
    // Should match: DE AD BE EF 42
  }
};

#endif