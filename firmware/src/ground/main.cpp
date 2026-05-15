#include "core/bus.hpp"
#include "log.hpp"

Bus bus;
Uart Logger::uart(USART1, 115200);
LogLevel Logger::minLevel = LogLevel::INFO;
SemaphoreHandle_t Logger::mutex = nullptr;

int main() {
  bus.init();
  Logger::init();

  // TODO: create ground station tasks (radio TX, USB/UART bridge)

  vTaskStartScheduler();
  while (true) {
  }
}

extern "C" {
void vConfigureTimerForRunTimeStats(void) {
  RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
  TIM5->PSC = 1599;
  TIM5->ARR = 0xFFFFFFFF;
  TIM5->CR1 |= TIM_CR1_CEN;
}

uint32_t ulGetRunTimeCounterValue(void) { return TIM5->CNT; }

void __cxa_pure_virtual() { while (1); }
void vApplicationMallocFailedHook() { while (1); }

void* memset(void* dst, int val, unsigned int len) {
  unsigned char* p = (unsigned char*)dst;
  while (len--) *p++ = (unsigned char)val;
  return dst;
}

void* memcpy(void* dst, const void* src, unsigned int len) {
  unsigned char* d = (unsigned char*)dst;
  const unsigned char* s = (const unsigned char*)src;
  while (len--) *d++ = *s++;
  return dst;
}
}
