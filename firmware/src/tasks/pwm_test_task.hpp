#ifndef PWM_TEST_TASK_HPP
#define PWM_TEST_TASK_HPP

#include "drivers/pwm.hpp"
#include "drivers/uart.hpp"
#include "led.hpp"
#include "task.hpp"

class PwmTestTask : public Task<PwmTestTask> {
 private:
  Pwm pwm;
  Led led;
  Uart uart;

 public:
  PwmTestTask() : pwm(TIM2, 0x03), led(GPIOC, 13), uart(USART1, 115200) {};

  void run() {
    uart.print("PWM simple test\r\n");

    // Set up PA0 manually — no Pwm class, raw registers
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // PA0 = alternate function AF1
    GPIOA->MODER &= ~(0x3 << 0);
    GPIOA->MODER |= (0x2 << 0);
    GPIOA->AFR[0] &= ~(0xF << 0);
    GPIOA->AFR[0] |= (0x1 << 0);

    // TIM2: 50Hz, CH1 at 1100us
    TIM2->PSC = 15;        // 16MHz / 16 = 1MHz
    TIM2->ARR = 19999;     // 20ms period
    TIM2->CCMR1 = (0x6 << 4) | TIM_CCMR1_OC1PE;
    TIM2->CCER = TIM_CCER_CC1E;
    TIM2->CCR1 = 2000;
    TIM2->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;

    uart.print("=== ESC CALIBRATION ===\r\n");
    uart.print("Step 1: MAX throttle (2000us)\r\n");
    uart.print(">>> PLUG BATTERY NOW <<<\r\n");

    // Hold max for 10 seconds
    for (int i = 10; i > 0; i--) {
      uart.printInt(i);
      uart.print("...\r\n");
      delay(1000);
    }

    // Drop to min
    TIM2->CCR1 = 1000;
    uart.print("Step 2: MIN throttle (1000us)\r\n");
    uart.print("Listen for confirmation beeps...\r\n\r\n");

    delay(5000);

    uart.print("Calibration done.\r\n");
    uart.print("Slowly ramping to 1200us...\r\n\r\n");

    // Slow ramp from 1000 to 1200 (very low throttle)
    uint16_t us = 1000;
    while (true) {
      if (us < 1200) {
        us += 2;
        TIM2->CCR1 = us;
      }
      uart.print("Throttle: ");
      uart.printInt(us);
      uart.print("us\r\n");
      led.toggle();
      delay(100);
    }
  }
};

#endif
