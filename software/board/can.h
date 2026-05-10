#ifndef CAN_H
#define CAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

extern CAN_HandleTypeDef hcan2;

void MX_CAN2_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* CAN_H */
