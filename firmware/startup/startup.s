.syntax unified
.cpu cortex-m4
.thumb

.global Reset_Handler
.type Reset_Handler, %function

.section .isr_vector, "a", %progbits
.word _estack
.word Reset_Handler
.word NMI_Handler
.word HardFault_Handler
.word MemManage_Handler
.word BusFault_Handler
.word UsageFault_Handler
.word 0, 0, 0, 0
.word SVC_Handler
.word DebugMon_Handler
.word 0
.word PendSV_Handler
.word SysTick_Handler
.rept 60
.word Default_Handler
.endr

.section .text

Reset_Handler:
    /* Copy .data from Flash to RAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
.data_copy:
    cmp r0, r1
    ittt lt
    ldrlt r3, [r2], #4
    strlt r3, [r0], #4
    blt .data_copy

    /* Zero .bss */
    ldr r0, =_sbss
    ldr r1, =_ebss
    mov r2, #0
.bss_zero:
    cmp r0, r1
    it lt
    strlt r2, [r0], #4
    blt .bss_zero

    /* Call C++ global constructors */
    ldr r0, =_init_array_start
    ldr r1, =_init_array_end
.init_loop:
    cmp r0, r1
    bge .init_done
    ldr r2, [r0], #4
    blx r2
    b .init_loop
.init_done:

    /* Enable FPU (CP10 and CP11 full access) */
    ldr r0, =0xE000ED88
    ldr r1, [r0]
    orr r1, r1, #(0xF << 20)
    str r1, [r0]
    dsb
    isb

    bl main
    b .

/* Weak default handlers */
.macro weak_handler name
.weak \name
.thumb_func
\name:
.endm

weak_handler NMI_Handler
weak_handler HardFault_Handler
weak_handler MemManage_Handler
weak_handler BusFault_Handler
weak_handler UsageFault_Handler
weak_handler SVC_Handler
weak_handler DebugMon_Handler
weak_handler PendSV_Handler
weak_handler SysTick_Handler

.thumb_func
Default_Handler:
    b .
