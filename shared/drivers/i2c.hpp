#ifndef I2C_HPP
#define I2C_HPP

#include "stm32f411xe.h"
class I2c {
 private:
  I2C_TypeDef* i2c;

 public:
  void busRecovery() {
    // If a slave is holding SDA low, toggle SCL manually to free it.
    // Set PB6 (SCL) as GPIO output, PB7 (SDA) as input.
    GPIOB->MODER &= ~(0x3 << (6 * 2));
    GPIOB->MODER |= (0x1 << (6 * 2));   // PB6 output
    GPIOB->MODER &= ~(0x3 << (7 * 2));  // PB7 input

    // Toggle SCL 16 times to unstick slave
    for (int i = 0; i < 16; i++) {
      GPIOB->BSRR = (1 << (6 + 16));  // SCL low
      for (volatile int d = 0; d < 100; d++);
      GPIOB->BSRR = (1 << 6);         // SCL high
      for (volatile int d = 0; d < 100; d++);
    }
  }

  I2c(I2C_TypeDef* i2c) : i2c(i2c) {
    // Enable clocks for GPIOB and I2C1
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // Bus recovery — unstick slave if SDA is held low
    busRecovery();

    // PB6 = SCL, PB7 = SDA — both alternate function AF4
    // PB6
    GPIOB->MODER &= ~(0x3 << (6 * 2));
    GPIOB->MODER |= (0x2 << (6 * 2));  // alternate function
    GPIOB->AFR[0] &= ~(0xF << (6 * 4));
    GPIOB->AFR[0] |= (0x4 << (6 * 4));  // AF4
    GPIOB->OTYPER |= (1 << 6);          // open-drain
    GPIOB->PUPDR &= ~(0x3 << (6 * 2));
    GPIOB->PUPDR |= (0x1 << (6 * 2));  // pull-up

    // PB7
    GPIOB->MODER &= ~(0x3 << (7 * 2));
    GPIOB->MODER |= (0x2 << (7 * 2));  // alternate function
    GPIOB->AFR[0] &= ~(0xF << (7 * 4));
    GPIOB->AFR[0] |= (0x4 << (7 * 4));  // AF4
    GPIOB->OTYPER |= (1 << 7);          // open-drain
    GPIOB->PUPDR &= ~(0x3 << (7 * 2));
    GPIOB->PUPDR |= (0x1 << (7 * 2));  // pull-up

    // Reset I2C peripheral
    i2c->CR1 = I2C_CR1_SWRST;
    i2c->CR1 = 0;

    // Configure I2C1
    i2c->CR2 = 16;           // APB1 clock = 16 MHz
    i2c->CCR = 80;           // 100kHz: 16MHz / (2 * 100kHz)
    i2c->TRISE = 17;         // max rise time: (16MHz / 1MHz) + 1
    i2c->CR1 |= I2C_CR1_PE;  // enable I2C
  }

  void write(uint8_t addr, uint8_t reg, uint8_t value) {
    // Generate START
    i2c->CR1 |= I2C_CR1_START;
    while (!(i2c->SR1 & I2C_SR1_SB));

    // Send slave address + write bit (0)
    i2c->DR = (addr << 1);
    while (!(i2c->SR1 & I2C_SR1_ADDR));
    (void)i2c->SR2;  // clear ADDR flag by reading SR2

    // Send register address
    i2c->DR = reg;
    while (!(i2c->SR1 & I2C_SR1_TXE));

    // Send value
    i2c->DR = value;
    while (!(i2c->SR1 & I2C_SR1_TXE));

    // Generate STOP
    i2c->CR1 |= I2C_CR1_STOP;
  }

  bool waitFlag(volatile uint32_t& reg, uint32_t flag, uint32_t timeout = 100000) {
    while (timeout--) {
      if (reg & flag) return true;
    }
    return false;
  }

  uint8_t read(uint8_t addr, uint8_t reg) {
    // Generate START
    i2c->CR1 |= I2C_CR1_START;
    if (!waitFlag(i2c->SR1, I2C_SR1_SB)) return 0xFF;

    // Send slave address + write bit (0)
    i2c->DR = (addr << 1);
    if (!waitFlag(i2c->SR1, I2C_SR1_ADDR)) return 0xFF;
    (void)i2c->SR2;

    // Send register address
    i2c->DR = reg;
    if (!waitFlag(i2c->SR1, I2C_SR1_TXE)) return 0xFF;

    // Generate repeated START
    i2c->CR1 |= I2C_CR1_START;
    if (!waitFlag(i2c->SR1, I2C_SR1_SB)) return 0xFF;

    // Send slave address + read bit (1)
    i2c->DR = (addr << 1) | 1;
    if (!waitFlag(i2c->SR1, I2C_SR1_ADDR)) return 0xFF;

    // NACK after one byte
    i2c->CR1 &= ~I2C_CR1_ACK;
    (void)i2c->SR2;

    // Generate STOP
    i2c->CR1 |= I2C_CR1_STOP;

    // Wait for data
    if (!waitFlag(i2c->SR1, I2C_SR1_RXNE)) return 0xFF;

    return i2c->DR;
  }
};

#endif