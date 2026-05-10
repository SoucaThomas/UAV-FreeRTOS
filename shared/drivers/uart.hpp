#ifndef UART_HPP
#define UART_HPP

#include "stm32f411xe.h"

class Uart {
 private:
  USART_TypeDef* usart;

 public:
  Uart(USART_TypeDef* usart, uint32_t baud) : usart(usart) {
    // Enable clocks
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // PA9 = alternate function AF7
    GPIOA->MODER &= ~(0x3 << (9 * 2));
    GPIOA->MODER |= (0x2 << (9 * 2));  // PA9 = alternate function
    GPIOA->AFR[1] &= ~(0xF << ((9 - 8) * 4));
    GPIOA->AFR[1] |= (0x7 << ((9 - 8) * 4));  // PA9 = AF7 (USART1)

    // PA10 = alternate function AF7 (RX)
    GPIOA->MODER &= ~(0x3 << (10 * 2));
    GPIOA->MODER |= (0x2 << (10 * 2));
    GPIOA->AFR[1] &= ~(0xF << ((10 - 8) * 4));
    GPIOA->AFR[1] |= (0x7 << ((10 - 8) * 4));

    // Set baud
    usart->BRR = 16000000 / baud;

    // Enable TX, RX, and USART
    usart->CR1 |= USART_CR1_TE | USART_CR1_RE;
    usart->CR1 |= USART_CR1_UE;
  };

  void write(uint8_t byte) {
    while (!(usart->SR & USART_SR_TXE));

    usart->DR = byte;
  }

  void print(const char* str) {
    while (*str) {
      write(*str++);
    }
  }

  void printHex(uint8_t byte) {
    const char hex[] = "0123456789ABCDEF";
    write(hex[byte >> 4]);    // high nibble
    write(hex[byte & 0x0F]);  // low nibble
  }

  void printInt(int32_t val) {
    if (val < 0) {
      write('-');
      val = -val;
    }
    if (val == 0) {
      write('0');
      return;
    }
    char buf[11];
    int i = 0;
    while (val > 0) {
      buf[i++] = '0' + (val % 10);
      val /= 10;
    }
    for (int j = i - 1; j >= 0; j--) {
      write(buf[j]);
    }
  }

  void printFloat(float val, uint8_t decimals) {
    if (val < 0.0f) {
      write('-');
      val = -val;
    }
    printInt((int32_t)val);
    write('.');
    float frac = val - (int32_t)val;
    for (uint8_t i = 0; i < decimals; i++) {
      frac *= 10.0f;
      write('0' + (uint8_t)frac);
      frac -= (uint8_t)frac;
    }
  }
};

#endif