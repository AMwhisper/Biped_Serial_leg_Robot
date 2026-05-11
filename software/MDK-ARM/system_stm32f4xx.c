/**
 * @file system_stm32f4xx.c
 * @brief Minimal system initialization placeholder.
 *
 * Replace this file with the CubeMX/ST CMSIS system file before hardware bring-up.
 */

#include <stdint.h>
#include "stm32f4xx_hal.h"

uint32_t SystemCoreClock = 168000000UL;
static uint32_t hal_tick;

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_Type;

#define SYSTICK_BASE 0xE000E010UL
#define SYSTICK      ((SysTick_Type *)SYSTICK_BASE)

static void systick_init_1ms(void) {
    SYSTICK->LOAD = (SystemCoreClock / 1000UL) - 1UL;
    SYSTICK->VAL = 0;
    SYSTICK->CTRL = 0x07UL;
}

void HAL_Init(void) {
    systick_init_1ms();
}

void SystemInit(void) {
    systick_init_1ms();
}

void SystemCoreClockUpdate(void) {
    SystemCoreClock = 168000000UL;
}

__attribute__((weak))
uint32_t HAL_GetTick(void) {
    return hal_tick;
}

void HAL_IncTick(void) {
    hal_tick++;
}

void NVIC_EnableIRQ(int32_t irq) {
    volatile uint32_t *iser = (volatile uint32_t *)(0xE000E100UL + ((uint32_t)irq / 32U) * 4U);
    *iser = 1UL << ((uint32_t)irq % 32U);
}

void NVIC_DisableIRQ(int32_t irq) {
    volatile uint32_t *icer = (volatile uint32_t *)(0xE000E180UL + ((uint32_t)irq / 32U) * 4U);
    *icer = 1UL << ((uint32_t)irq % 32U);
}

void NVIC_SetPriority(int32_t irq, uint32_t priority) {
    volatile uint8_t *ipr = (volatile uint8_t *)(0xE000E400UL + (uint32_t)irq);
    *ipr = (uint8_t)((priority & 0x0FU) << 4);
}
