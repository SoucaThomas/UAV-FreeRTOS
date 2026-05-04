#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

// Cortex-M3 runs at 8MHz HSI by default (no PLL configured yet)
#define configCPU_CLOCK_HZ 8000000UL
#define configTICK_RATE_HZ 1000
#define configMAX_PRIORITIES 5
#define configMINIMAL_STACK_SIZE 128
#define configTOTAL_HEAP_SIZE ((size_t)(2 * 1024))
#define configMAX_TASK_NAME_LEN 16
#define configUSE_PREEMPTION 1
#define configUSE_IDLE_HOOK 0
#define configUSE_TICK_HOOK 0
#define configUSE_16_BIT_TICKS 0
#define configIDLE_SHOULD_YIELD 1

// Memory allocation
#define configSUPPORT_STATIC_ALLOCATION 0
#define configSUPPORT_DYNAMIC_ALLOCATION 1

// Features
#define configUSE_MUTEXES 1
#define configUSE_SEMAPHORES 1
#define configUSE_TIMERS 0
#define configUSE_COUNTING_SEMAPHORES 0
#define configUSE_QUEUE_SETS 0
#define configUSE_TASK_NOTIFICATIONS 1

// Disable unused features to save flash
#define configUSE_CO_ROUTINES 0
#define configUSE_TRACE_FACILITY 0
#define configGENERATE_RUN_TIME_STATS 0
#define configCHECK_FOR_STACK_OVERFLOW 0

// Cortex-M3 interrupt priorities
#define configPRIO_BITS 4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY \
  (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
  (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

// Map FreeRTOS handlers to our vector table names
// Include optional API functions
#define INCLUDE_vTaskDelay 1
#define INCLUDE_vTaskPrioritySet 0
#define INCLUDE_uxTaskPriorityGet 0
#define INCLUDE_vTaskDelete 0
#define INCLUDE_vTaskSuspend 0

// Map FreeRTOS handlers to our vector table names
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif
