/**
 * @file main.c
 * @brief Minimal RoboMaster C bring-up entry.
 */

#include <stdint.h>
#include "system.h"
#include "motor_controller.h"
#include "motor_config.h"

#if defined(__has_include)
#if __has_include("stm32f4xx_hal.h")
#include "stm32f4xx_hal.h"
#define MAIN_USE_HAL 1
#endif
#endif

#ifndef MAIN_USE_HAL
#define MAIN_USE_HAL 0
#endif

#define CONTROL_PERIOD_MS 2U

extern void SystemInit(void);

#if MAIN_USE_HAL
void SystemClock_Config(void);
void Error_Handler(void);
#endif

int main(void) {
#if MAIN_USE_HAL
    HAL_Init();
    SystemClock_Config();
#else
    SystemInit();
#endif

    if (!system_init()) {
        system_emergency_stop();
        while (1) {
        }
    }

    for (uint8_t id = 1U; id <= TOTAL_MOTORS; ++id) {
        (void)motor_disable(id);
#if MAIN_USE_HAL
        HAL_Delay(2U);
#endif
    }

    uint32_t last_time = 0U;
    while (1) {
#if MAIN_USE_HAL
        uint32_t now = HAL_GetTick();
        if ((now - last_time) >= CONTROL_PERIOD_MS) {
            (void)system_update();
            last_time = now;
        }
#else
        (void)system_update();
#endif
    }
}

#if MAIN_USE_HAL
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 6;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        HAL_IncTick();
    }
}
#endif
