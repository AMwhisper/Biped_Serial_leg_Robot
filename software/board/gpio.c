#include "gpio.h"
#include "stm32f4xx_hal.h"

void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init) {
    if (GPIOx == 0 || GPIO_Init == 0) {
        return;
    }

    for (uint32_t pin = 0; pin < 16U; ++pin) {
        uint32_t mask = 1UL << pin;
        if ((GPIO_Init->Pin & mask) == 0U) {
            continue;
        }

        GPIOx->MODER &= ~(3UL << (pin * 2U));
        GPIOx->MODER |= ((GPIO_Init->Mode & 3U) << (pin * 2U));

        GPIOx->PUPDR &= ~(3UL << (pin * 2U));
        GPIOx->PUPDR |= ((GPIO_Init->Pull & 3U) << (pin * 2U));

        GPIOx->OSPEEDR &= ~(3UL << (pin * 2U));
        GPIOx->OSPEEDR |= ((GPIO_Init->Speed & 3U) << (pin * 2U));

        uint32_t afr_index = pin / 8U;
        uint32_t afr_shift = (pin % 8U) * 4U;
        GPIOx->AFR[afr_index] &= ~(0xFUL << afr_shift);
        GPIOx->AFR[afr_index] |= ((GPIO_Init->Alternate & 0xFU) << afr_shift);
    }
}

void HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin) {
    if (GPIOx == 0) {
        return;
    }

    for (uint32_t pin = 0; pin < 16U; ++pin) {
        uint32_t mask = 1UL << pin;
        if ((GPIO_Pin & mask) == 0U) {
            continue;
        }

        GPIOx->MODER &= ~(3UL << (pin * 2U));
        GPIOx->PUPDR &= ~(3UL << (pin * 2U));
        GPIOx->OSPEEDR &= ~(3UL << (pin * 2U));
        GPIOx->OTYPER &= ~mask;
        GPIOx->AFR[pin / 8U] &= ~(0xFUL << ((pin % 8U) * 4U));
    }
}

void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
}
