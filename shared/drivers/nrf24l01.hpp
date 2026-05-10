#ifndef NRF24L01_HPP
#define NRF24L01_HPP

#include "drivers/spi.hpp"
#include "stm32f411xe.h"

// SPI commands
constexpr uint8_t NRF_R_REGISTER = 0x00;    // read register (OR with address)
constexpr uint8_t NRF_W_REGISTER = 0x20;    // write register (OR with address)
constexpr uint8_t NRF_R_RX_PAYLOAD = 0x61;  // read RX payload
constexpr uint8_t NRF_W_TX_PAYLOAD = 0xA0;  // write TX payload
constexpr uint8_t NRF_FLUSH_TX = 0xE1;
constexpr uint8_t NRF_FLUSH_RX = 0xE2;
constexpr uint8_t NRF_NOP = 0xFF;

// Register addresses
constexpr uint8_t NRF_CONFIG = 0x00;
constexpr uint8_t NRF_EN_AA = 0x01;     // auto-ack
constexpr uint8_t NRF_RF_CH = 0x05;     // channel
constexpr uint8_t NRF_RF_SETUP = 0x06;  // data rate + power
constexpr uint8_t NRF_STATUS = 0x07;
constexpr uint8_t NRF_RX_ADDR_P0 = 0x0A;
constexpr uint8_t NRF_TX_ADDR = 0x10;
constexpr uint8_t NRF_RX_PW_P0 = 0x11;  // payload width pipe 0

// CONFIG register bits
constexpr uint8_t NRF_PWR_UP = (1 << 1);
constexpr uint8_t NRF_PRIM_RX = (1 << 0);
constexpr uint8_t NRF_EN_CRC = (1 << 3);

// STATUS register bits
constexpr uint8_t NRF_RX_DR = (1 << 6);   // data ready in RX FIFO
constexpr uint8_t NRF_TX_DS = (1 << 5);   // TX sent
constexpr uint8_t NRF_MAX_RT = (1 << 4);  // max retransmits

class Nrf24l01 {
 private:
  Spi* spi;
  GPIO_TypeDef* cePort;
  uint8_t cePin;

  void ceHigh() { cePort->BSRR = (1 << cePin); }
  void ceLow() { cePort->BSRR = (1 << (cePin + 16)); }

 public:
  void setSpi(Spi* newSpi) { spi = newSpi; }

  Nrf24l01(Spi* spi, GPIO_TypeDef* cePort, uint8_t cePin) : spi(spi), cePort(cePort), cePin(cePin) {
    // Configure CE pin as output
    cePort->MODER &= ~(0x3 << (cePin * 2));
    cePort->MODER |= (0x1 << (cePin * 2));

    ceLow();
  }

  void csDelay() {
    for (volatile int i = 0; i < 10; i++);
  }

  uint8_t readReg(uint8_t reg) {
    spi->csLow();
    csDelay();

    spi->transfer(NRF_R_REGISTER | reg);   // command byte
    uint8_t val = spi->transfer(NRF_NOP);  // clock out the value

    spi->csHigh();
    csDelay();
    return val;
  }

  void writeReg(uint8_t reg, uint8_t value) {
    spi->csLow();
    spi->transfer(NRF_W_REGISTER | reg);
    spi->transfer(value);
    // Wait for SPI to finish before raising CS
    while (spi->isBusy());
    spi->csHigh();
  }

  void writePayload(const uint8_t* data, uint8_t len) {
    spi->csLow();

    spi->transfer(NRF_W_TX_PAYLOAD);
    for (uint8_t i = 0; i < len; i++) {
      spi->transfer(data[i]);
    }

    spi->csHigh();
  }

  void readPayload(uint8_t* data, uint8_t len) {
    spi->csLow();

    spi->transfer(NRF_R_RX_PAYLOAD);
    for (uint8_t i = 0; i < len; i++) {
      data[i] = spi->transfer(NRF_NOP);
    }

    spi->csHigh();
  }

  void init(bool rxMode, uint8_t channel) {
    // Disable auto-ack (we handle reliability in our protocol)
    writeReg(NRF_EN_AA, 0x00);

    // Set RF channel
    writeReg(NRF_RF_CH, channel);

    // 1 Mbps data rate, 0 dBm TX power
    // RF_SETUP: bit 3 = 0 (1Mbps), bits 2:1 = 11 (0dBm)
    writeReg(NRF_RF_SETUP, 0x06);

    // Fixed 32-byte payload on pipe 0
    writeReg(NRF_RX_PW_P0, 32);

    // Flush both FIFOs
    spi->csLow();
    csDelay();
    spi->transfer(NRF_FLUSH_TX);
    spi->csHigh();
    csDelay();
    spi->csLow();
    csDelay();
    spi->transfer(NRF_FLUSH_RX);
    spi->csHigh();
    csDelay();

    // Clear any pending IRQ flags (write 1 to clear)
    writeReg(NRF_STATUS, NRF_RX_DR | NRF_TX_DS | NRF_MAX_RT);

    // Power on + CRC enabled + RX/TX mode
    uint8_t config = NRF_PWR_UP | NRF_EN_CRC;
    if (rxMode) {
      config |= NRF_PRIM_RX;
    }
    writeReg(NRF_CONFIG, config);

    // CE high to start listening (only matters in RX mode)
    if (rxMode) {
      ceHigh();
    }
  }

  void send(const uint8_t* data, uint8_t len) {
    ceLow();

    writePayload(data, len);
    ceHigh();
    // Pulse CE for >10us to trigger transmission
    for (volatile int i = 0; i < 100; i++);

    ceLow();
  }

  bool available() {
    uint8_t status = readReg(NRF_STATUS);

    return status & NRF_RX_DR;
  }

  void receive(uint8_t* data, uint8_t len) {
    readPayload(data, len);
    // Clear RX_DR flag
    writeReg(NRF_STATUS, NRF_RX_DR);
  }
};

#endif