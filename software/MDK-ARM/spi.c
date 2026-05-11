#include "spi.h"
#include "gpio.h"

SPI_HandleTypeDef hspi1;

void MX_SPI1_Init(void) {
    GPIO_InitTypeDef gpio;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_3 | GPIO_PIN_4;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &gpio);

    gpio.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &gpio);

    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10U;
    (void)HAL_SPI_Init(&hspi1);
}

HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi) {
    if (hspi == 0 || hspi->Instance == 0) {
        return HAL_ERROR;
    }

    hspi->Instance->CR1 = 0U;
    hspi->Instance->CR1 = hspi->Init.Mode |
                          hspi->Init.Direction |
                          hspi->Init.DataSize |
                          hspi->Init.CLKPolarity |
                          hspi->Init.CLKPhase |
                          hspi->Init.NSS |
                          hspi->Init.BaudRatePrescaler |
                          hspi->Init.FirstBit;
    hspi->Instance->CR1 |= (1UL << 6);
    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size, uint32_t timeout) {
    if (hspi == 0 || hspi->Instance == 0 || tx_data == 0 || rx_data == 0) {
        return HAL_ERROR;
    }

    for (uint16_t i = 0; i < size; ++i) {
        uint32_t guard = timeout * 1000U + 1000U;
        while (((hspi->Instance->SR & (1UL << 1)) == 0U) && guard-- > 0U) {
        }
        if (guard == 0U) {
            return HAL_TIMEOUT;
        }

        *((__IO uint8_t *)&hspi->Instance->DR) = tx_data[i];

        guard = timeout * 1000U + 1000U;
        while (((hspi->Instance->SR & (1UL << 0)) == 0U) && guard-- > 0U) {
        }
        if (guard == 0U) {
            return HAL_TIMEOUT;
        }

        rx_data[i] = *((__IO uint8_t *)&hspi->Instance->DR);
    }

    return HAL_OK;
}
