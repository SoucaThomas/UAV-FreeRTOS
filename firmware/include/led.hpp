#ifndef LED_HPP
#define LED_HPP

#include "stm32f103xb.h"

class Led {
 private:
  GPIO_TypeDef* port;
  uint16_t pin;

 public:
  Led(GPIO_TypeDef* port, uint16_t pin) : port(port), pin(pin) {
    // Enable clock for port
    if (port == GPIOA)
      RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    else if (port == GPIOB)
      RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    else if (port == GPIOC)
      RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // Set pin as output push-pull, 2MHz
    if (pin < 8) {
      uint32_t offset = pin * 4;
      port->CRL &= ~(0xFUL << offset);
      port->CRL |= (0x2UL << offset);
    } else {
      uint32_t offset = (pin - 8) * 4;
      port->CRH &= ~(0xFUL << offset);
      port->CRH |= (0x2UL << offset);
    }
  }

  void on() { port->BSRR = (1UL << (pin + 16)); }
  void off() { port->BSRR = (1UL << pin); }
  void toggle() { port->ODR ^= (1UL << pin); }
};

#endif
