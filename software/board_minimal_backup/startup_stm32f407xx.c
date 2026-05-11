/**
 * @file startup_stm32f407xx.c
 * @brief Minimal C startup file for Keil/ARMClang.
 *
 * Replace this file with the official ST startup_stm32f407xx.s when CubeMX/HAL
 * support is added.
 */

#include <stdint.h>

#define SRAM_START 0x20000000UL
#define SRAM_SIZE  (128UL * 1024UL)
#define SRAM_END   (SRAM_START + SRAM_SIZE)

__asm(".global __initial_sp\n"
      ".equ __initial_sp, 0x20020000\n");

extern void __main(void);
extern void SystemInit(void);

void Reset_Handler(void);
void Default_Handler(void);

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void CAN2_RX0_IRQHandler(void);

__attribute__((used, section("RESET")))
const void *vector_table[] = {
    [0] = (const void *)SRAM_END,
    [1] = Reset_Handler,
    [2] = NMI_Handler,
    [3] = HardFault_Handler,
    [4] = MemManage_Handler,
    [5] = BusFault_Handler,
    [6] = UsageFault_Handler,
    [11] = SVC_Handler,
    [12] = DebugMon_Handler,
    [14] = PendSV_Handler,
    [15] = SysTick_Handler,
    [16 + 65] = CAN2_RX0_IRQHandler,
};

void Reset_Handler(void) {
    SystemInit();
    __main();

    while (1) {
    }
}

void Default_Handler(void) {
    while (1) {
    }
}
