#ifndef PWM_HPP
#define PWM_HPP

#include "stm32f411xe.h"

class Pwm {
 private:
  TIM_TypeDef* timer;
  uint8_t channels;

  void setupPin(GPIO_TypeDef* port, uint8_t pin, uint8_t af) {
    port->MODER &= ~(0x3 << (pin * 2));
    port->MODER |= (0x2 << (pin * 2));
    if (pin < 8) {
      port->AFR[0] &= ~(0xF << (pin * 4));
      port->AFR[0] |= (af << (pin * 4));
    } else {
      port->AFR[1] &= ~(0xF << ((pin - 8) * 4));
      port->AFR[1] |= (af << ((pin - 8) * 4));
    }
  }

 public:
  Pwm(TIM_TypeDef* timer, uint8_t channels) : timer(timer), channels(channels) {}

  void init() {
    // Enable clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    if (timer == TIM2) {
      RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
      if (channels & 0x01) setupPin(GPIOA, 0, 1);
      if (channels & 0x02) setupPin(GPIOA, 1, 1);
      if (channels & 0x04) setupPin(GPIOA, 2, 1);
      if (channels & 0x08) setupPin(GPIOA, 3, 1);
    }
    if (timer == TIM3) {
      RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
      if (channels & 0x01) setupPin(GPIOB, 4, 2);
      if (channels & 0x02) setupPin(GPIOB, 5, 2);
      if (channels & 0x04) setupPin(GPIOB, 0, 2);
      if (channels & 0x08) setupPin(GPIOB, 1, 2);
    }

    // Prescaler: 16MHz / 16 = 1MHz (1µs per tick)
    timer->PSC = 15;

    // Period: 20000 ticks = 20ms = 50Hz
    timer->ARR = 19999;

    // Configure each enabled channel for PWM mode 1
    if (channels & 0x01) {
      timer->CCMR1 |= (0x6 << 4);
      timer->CCMR1 |= TIM_CCMR1_OC1PE;
      timer->CCER |= TIM_CCER_CC1E;
      timer->CCR1 = 1100;
    }
    if (channels & 0x02) {
      timer->CCMR1 |= (0x6 << 12);
      timer->CCMR1 |= TIM_CCMR1_OC2PE;
      timer->CCER |= TIM_CCER_CC2E;
      timer->CCR2 = 1500;
    }
    if (channels & 0x04) {
      timer->CCMR2 |= (0x6 << 4);
      timer->CCMR2 |= TIM_CCMR2_OC3PE;
      timer->CCER |= TIM_CCER_CC3E;
      timer->CCR3 = 1500;
    }
    if (channels & 0x08) {
      timer->CCMR2 |= (0x6 << 12);
      timer->CCMR2 |= TIM_CCMR2_OC4PE;
      timer->CCER |= TIM_CCER_CC4E;
      timer->CCR4 = 1500;
    }

    // Start the timer
    timer->CR1 |= TIM_CR1_ARPE;
    timer->CR1 |= TIM_CR1_CEN;
  }

  void set(uint8_t channel, uint16_t us) {
    if (us < 1000) us = 1000;
    if (us > 2000) us = 2000;

    switch (channel) {
      case 1:
        timer->CCR1 = us;
        break;
      case 2:
        timer->CCR2 = us;
        break;
      case 3:
        timer->CCR3 = us;
        break;
      case 4:
        timer->CCR4 = us;
        break;
    }
  }
};

#endif
