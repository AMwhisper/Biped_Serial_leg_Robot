#include "BMI088middleware.h"

void BMI088_GPIO_init(void) {
    GPIO_InitTypeDef gpio;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpio.Pin = CS1_ACCEL_Pin;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = 0U;
    HAL_GPIO_Init(CS1_ACCEL_GPIO_Port, &gpio);

    gpio.Pin = CS1_GYRO_Pin;
    HAL_GPIO_Init(CS1_GYRO_GPIO_Port, &gpio);

    BMI088_ACCEL_NS_H();
    BMI088_GYRO_NS_H();
}

void BMI088_com_init(void) {
    MX_SPI1_Init();
}

void BMI088_delay_ms(uint16_t ms) {
    uint32_t start = HAL_GetTick();
    while ((HAL_GetTick() - start) < ms) {
    }
}

void BMI088_delay_us(uint16_t us) {
    volatile uint32_t loops = (uint32_t)us * 32U;
    while (loops-- > 0U) {
    }
}

void BMI088_ACCEL_NS_L(void) {
    GPIO_ResetBits(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin);
}
void BMI088_ACCEL_NS_H(void) {
    GPIO_SetBits(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin);
}

void BMI088_GYRO_NS_L(void) {
    GPIO_ResetBits(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin);
}
void BMI088_GYRO_NS_H(void) {
    GPIO_SetBits(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin);
}

uint8_t BMI088_read_write_byte(uint8_t txdata) {
    uint8_t rx_data;
    HAL_SPI_TransmitReceive(&SPI_Instance, &txdata, &rx_data, 1, 100);
    return rx_data;
}
