#include "can.h"
#include "stm32f4xx_hal.h"

void CAN2_RX0_IRQHandler(void) {
    HAL_CAN_IRQHandler(&hcan2);
}
