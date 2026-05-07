#ifndef SPI_HPP
#define SPI_HPP

#include "stm32f411xe.h"

class Spi {
 private:
  SPI_TypeDef* spi;
  GPIO_TypeDef* gpio;
  uint8_t cs_pin;

 public:
  Spi(SPI_TypeDef* spi, GPIO_TypeDef* gpio, uint8_t cs_pin) : spi(spi), gpio(gpio), cs_pin(cs_pin) {
    // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Turn on SPI1 clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Pin 5
    gpio->MODER &= ~(0x3 << (5 * 2));   // clear pin
    gpio->MODER |= (0x2 << (5 * 2));    // set to alternate function
    gpio->AFR[0] &= ~(0xF << (5 * 4));  // clear pin
    gpio->AFR[0] |= (0x5 << (5 * 4));   // set pin 5 to AF5

    // Pin 6
    gpio->MODER &= ~(0x3 << (6 * 2));   // clear pin
    gpio->MODER |= (0x2 << (6 * 2));    // set to alternate function
    gpio->AFR[0] &= ~(0xF << (6 * 4));  // clear pin
    gpio->AFR[0] |= (0x5 << (6 * 4));   // set pin 6 to AF6

    // Pin 7
    gpio->MODER &= ~(0x3 << (7 * 2));   // clear pin
    gpio->MODER |= (0x2 << (7 * 2));    // set to alternate function
    gpio->AFR[0] &= ~(0xF << (7 * 4));  // clear pin
    gpio->AFR[0] |= (0x5 << (7 * 4));   // set pin 7 to AF7

    // Set PA4 as output (MODER = 01)
    gpio->MODER &= ~(0x3 << (cs_pin * 2));
    gpio->MODER |= (0x1 << (cs_pin * 2));

    // Set CS high
    gpio->BSRR = (1 << cs_pin);

    // Configure SPI
    spi->CR1 = 0;

    spi->CR1 |= SPI_CR1_MSTR;  // bit 2: master mode
    spi->CR1 |= (0x3 << 3);    // bits 5:3 = 011: prescaler /16
    spi->CR1 |= SPI_CR1_SSI;   // bit 8: internal slave select
    spi->CR1 |= SPI_CR1_SSM;   // bit 9: software slave management
    spi->CR1 |= SPI_CR1_SPE;   // bit 6: enable SPI
  };

  uint8_t transfer(uint8_t byte) {
    // Wait until TX buffer is empty
    while (!(spi->SR & SPI_SR_TXE));

    // Write byte to register
    spi->DR = byte;

    // Wait until RX buffer has data
    while (!(spi->SR & SPI_SR_RXNE));

    // Read and return the received byte
    return spi->DR;
  };

  void csLow() { gpio->BSRR = (1 << (cs_pin + 16)); }  // reset = pull low
  void csHigh() { gpio->BSRR = (1 << cs_pin); }        // set = pull high
};

#endif