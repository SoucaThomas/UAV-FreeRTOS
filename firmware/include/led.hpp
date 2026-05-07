#ifndef LED_HPP
#define LED_HPP

#include "stm32f411xe.h"

class Led {
 private:
  GPIO_TypeDef* port;
  uint16_t pin;

 public:
  Led(GPIO_TypeDef* port, uint16_t pin) : port(port), pin(pin) {
    // Enable clock for port (GPIOs are on AHB1 on F4)
    if (port == GPIOA)
      RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    else if (port == GPIOB)
      RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    else if (port == GPIOC)
      RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    // Set pin as general purpose output (MODER = 01)
    port->MODER &= ~(0x3UL << (pin * 2));
    port->MODER |= (0x1UL << (pin * 2));

    // Push-pull (OTYPER = 0)
    port->OTYPER &= ~(1UL << pin);

    // Low speed (OSPEEDR = 00)
    port->OSPEEDR &= ~(0x3UL << (pin * 2));

    // No pull-up/pull-down (PUPDR = 00)
    port->PUPDR &= ~(0x3UL << (pin * 2));
  }

  void on() { port->BSRR = (1UL << (pin + 16)); }
  void off() { port->BSRR = (1UL << pin); }
  void toggle() { port->ODR ^= (1UL << pin); }
};

#endif
