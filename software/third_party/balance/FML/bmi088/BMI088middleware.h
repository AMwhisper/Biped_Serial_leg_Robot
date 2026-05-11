#ifndef BMI088MIDDLEWARE_H
#define BMI088MIDDLEWARE_H

#include "stm32f4xx_hal.h"
#include "spi.h"

extern SPI_HandleTypeDef hspi1;

#define CS1_ACCEL_Pin GPIO_PIN_4
#define CS1_ACCEL_GPIO_Port GPIOA
#define CS1_GYRO_Pin GPIO_PIN_0
#define CS1_GYRO_GPIO_Port GPIOB
#define SPI_Instance hspi1

#define GPIO_SetBits(GPIOx, GPIO_PIN_x) HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x, GPIO_PIN_SET)
#define GPIO_ResetBits(GPIOx, GPIO_PIN_x) HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x, GPIO_PIN_RESET)

extern void BMI088_GPIO_init(void);
extern void BMI088_com_init(void);
extern void BMI088_delay_ms(uint16_t ms);
extern void BMI088_delay_us(uint16_t us);
extern void BMI088_ACCEL_NS_L(void);
extern void BMI088_ACCEL_NS_H(void);

extern void BMI088_GYRO_NS_L(void);
extern void BMI088_GYRO_NS_H(void);

extern uint8_t BMI088_read_write_byte(uint8_t reg);

#endif
