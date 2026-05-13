#ifndef W25Q64_HPP
#define W25Q64_HPP

#include <stdio.h>

#include "spi.hpp"

constexpr uint8_t W25Q_WRITE_ENABLE = 0x06;
constexpr uint8_t W25Q_READ_STATUS = 0x05;
constexpr uint8_t W25Q_PAGE_PROGRAM = 0x02;
constexpr uint8_t W25Q_READ_DATA = 0x03;
constexpr uint8_t W25Q_SECTOR_ERASE = 0x20;
constexpr uint8_t W25Q_CHIP_ERASE = 0xC7;
constexpr uint8_t W25Q_JEDEC_ID = 0x9F;

class W25q64 {
 private:
  Spi* spi;

  void writeEnable() {
    spi->csLow();
    spi->transfer(W25Q_WRITE_ENABLE);
    spi->csHigh();
  }

  void waitBusy() {
    spi->csLow();
    spi->transfer(W25Q_READ_STATUS);
    while (spi->transfer(0x00) & 0x01);
    spi->csHigh();
  }

  void sendAddress(uint32_t addr) {
    spi->transfer((addr >> 16) & 0xFF);
    spi->transfer((addr >> 8) & 0xFF);
    spi->transfer(addr & 0xFF);
  }

 public:
  W25q64(Spi* spi) : spi(spi) {}

  void setSpi(Spi* s) { spi = s; }

  uint32_t readId() {
    spi->csLow();
    spi->transfer(W25Q_JEDEC_ID);

    uint8_t mfr = spi->transfer(0x00);
    uint8_t type = spi->transfer(0x00);
    uint8_t cap = spi->transfer(0x00);

    spi->csHigh();

    return ((uint32_t)mfr << 16) | ((uint32_t)type << 8) | cap;
  }

  void read(uint32_t addr, uint8_t* buf, uint32_t len) {
    spi->csLow();
    spi->transfer(W25Q_READ_DATA);

    sendAddress(addr);

    for (uint32_t i = 0; i < len; i++) {
      buf[i] = spi->transfer(0x00);
    }

    spi->csHigh();
  }

  void writePage(uint32_t addr, const uint8_t* buf, uint16_t len) {
    if (len > 256) len = 256;

    writeEnable();

    spi->csLow();
    spi->transfer(W25Q_PAGE_PROGRAM);

    sendAddress(addr);

    for (uint16_t i = 0; i < len; i++) {
      spi->transfer(buf[i]);
    }

    spi->csHigh();

    waitBusy();
  }

  void eraseSector(uint32_t addr) {
    writeEnable();

    spi->csLow();
    spi->transfer(W25Q_SECTOR_ERASE);

    sendAddress(addr);

    spi->csHigh();

    waitBusy();
  }

  void eraseChip() {
    writeEnable();

    spi->csLow();
    spi->transfer(W25Q_CHIP_ERASE);
    spi->csHigh();

    waitBusy();
  }
};

#endif