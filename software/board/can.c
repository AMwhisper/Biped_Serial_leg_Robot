#include "can.h"
#include "gpio.h"
#include "bringup_config.h"

CAN_HandleTypeDef hcan2;

static void CAN2_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void MX_CAN2_Init(void) {
    CAN2_GPIO_Init();
    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_CAN2_CLK_ENABLE();

    hcan2.Instance = CAN2;
    hcan2.Init.Prescaler = BRINGUP_CAN_1MBPS ? 3 : 42;
    hcan2.Init.Mode = CAN_MODE_NORMAL;
    hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan2.Init.TimeSeg1 = BRINGUP_CAN_1MBPS ? CAN_BS1_11TQ : CAN_BS1_4TQ;
    hcan2.Init.TimeSeg2 = BRINGUP_CAN_1MBPS ? CAN_BS2_2TQ : CAN_BS2_5TQ;
    hcan2.Init.TimeTriggeredMode = DISABLE;
    hcan2.Init.AutoBusOff = DISABLE;
    hcan2.Init.AutoWakeUp = DISABLE;
    hcan2.Init.AutoRetransmission = ENABLE;
    hcan2.Init.ReceiveFifoLocked = DISABLE;
    hcan2.Init.TransmitFifoPriority = DISABLE;

    (void)HAL_CAN_Init(&hcan2);
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
}

void HAL_CAN_MspInit(CAN_HandleTypeDef *canHandle) {
    if (canHandle != 0 && canHandle->Instance == CAN2) {
        CAN2_GPIO_Init();
        __HAL_RCC_CAN2_CLK_ENABLE();
        __HAL_RCC_CAN1_CLK_ENABLE();
        HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
    }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef *canHandle) {
    if (canHandle != 0 && canHandle->Instance == CAN2) {
        __HAL_RCC_CAN2_CLK_DISABLE();
        __HAL_RCC_CAN1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5 | GPIO_PIN_6);
        HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
    }
}

HAL_StatusTypeDef HAL_CAN_Init(CAN_HandleTypeDef *hcan) {
    if (hcan == 0 || hcan->Instance == 0) {
        return HAL_ERROR;
    }

    HAL_CAN_MspInit(hcan);

    CAN_TypeDef *can = hcan->Instance;
    can->MCR |= 0x00000001U;
    for (uint32_t timeout = 0; timeout < 100000U; ++timeout) {
        if ((can->MSR & 0x00000001U) != 0U) {
            break;
        }
    }

    can->MCR = 0x00000051U;
    can->BTR = (hcan->Init.Prescaler - 1U) |
               hcan->Init.SyncJumpWidth |
               hcan->Init.TimeSeg1 |
               hcan->Init.TimeSeg2 |
               hcan->Init.Mode;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filter) {
    (void)hcan;
    if (filter == 0 || filter->FilterBank >= 28U) {
        return HAL_ERROR;
    }

    CAN1->FMR |= 0x00000001U;
    CAN1->FMR = (CAN1->FMR & ~(0x3FU << 8)) | ((filter->SlaveStartFilterBank & 0x3FU) << 8) | 0x00000001U;
    CAN1->FA1R &= ~(1UL << filter->FilterBank);

    if (filter->FilterScale == CAN_FILTERSCALE_32BIT) {
        CAN1->FS1R |= (1UL << filter->FilterBank);
    } else {
        CAN1->FS1R &= ~(1UL << filter->FilterBank);
    }

    if (filter->FilterMode == CAN_FILTERMODE_IDMASK) {
        CAN1->FM1R &= ~(1UL << filter->FilterBank);
    } else {
        CAN1->FM1R |= (1UL << filter->FilterBank);
    }

    if (filter->FilterFIFOAssignment == CAN_RX_FIFO0) {
        CAN1->FFA1R &= ~(1UL << filter->FilterBank);
    } else {
        CAN1->FFA1R |= (1UL << filter->FilterBank);
    }

    CAN1->sFilterRegister[filter->FilterBank].FR1 =
        ((filter->FilterIdHigh & 0xFFFFU) << 16) | (filter->FilterIdLow & 0xFFFFU);
    CAN1->sFilterRegister[filter->FilterBank].FR2 =
        ((filter->FilterMaskIdHigh & 0xFFFFU) << 16) | (filter->FilterMaskIdLow & 0xFFFFU);

    if (filter->FilterActivation == ENABLE) {
        CAN1->FA1R |= (1UL << filter->FilterBank);
    }
    CAN1->FMR &= ~0x00000001U;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_CAN_Start(CAN_HandleTypeDef *hcan) {
    if (hcan == 0 || hcan->Instance == 0) {
        return HAL_ERROR;
    }

    hcan->Instance->MCR &= ~0x00000001U;
    for (uint32_t timeout = 0; timeout < 100000U; ++timeout) {
        if ((hcan->Instance->MSR & 0x00000001U) == 0U) {
            return HAL_OK;
        }
    }
    return HAL_TIMEOUT;
}

HAL_StatusTypeDef HAL_CAN_Stop(CAN_HandleTypeDef *hcan) {
    if (hcan == 0 || hcan->Instance == 0) {
        return HAL_ERROR;
    }
    hcan->Instance->MCR |= 0x00000001U;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_CAN_ActivateNotification(CAN_HandleTypeDef *hcan, uint32_t active_its) {
    if (hcan == 0 || hcan->Instance == 0) {
        return HAL_ERROR;
    }
    hcan->Instance->IER |= active_its;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_CAN_DeactivateNotification(CAN_HandleTypeDef *hcan, uint32_t inactive_its) {
    if (hcan == 0 || hcan->Instance == 0) {
        return HAL_ERROR;
    }
    hcan->Instance->IER &= ~inactive_its;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *header, uint8_t data[8], uint32_t *mailbox) {
    if (hcan == 0 || header == 0 || data == 0) {
        return HAL_ERROR;
    }

    CAN_TypeDef *can = hcan->Instance;
    uint32_t mb;
    if ((can->TSR & (1UL << 26)) != 0U) {
        mb = 0;
    } else if ((can->TSR & (1UL << 27)) != 0U) {
        mb = 1;
    } else if ((can->TSR & (1UL << 28)) != 0U) {
        mb = 2;
    } else {
        return HAL_BUSY;
    }

    CAN_TxMailBox_TypeDef *tx = &can->sTxMailBox[mb];
    tx->TDTR = header->DLC & 0x0FU;
    tx->TDLR = ((uint32_t)data[3] << 24) | ((uint32_t)data[2] << 16) |
               ((uint32_t)data[1] << 8) | data[0];
    tx->TDHR = ((uint32_t)data[7] << 24) | ((uint32_t)data[6] << 16) |
               ((uint32_t)data[5] << 8) | data[4];
    tx->TIR = ((header->StdId & 0x7FFU) << 21) | 0x00000001U;

    if (mailbox != 0) {
        *mailbox = mb;
    }
    return HAL_OK;
}

uint32_t HAL_CAN_GetRxFifoFillLevel(CAN_HandleTypeDef *hcan, uint32_t fifo) {
    if (hcan == 0 || hcan->Instance == 0 || fifo != CAN_RX_FIFO0) {
        return 0;
    }
    return hcan->Instance->RF0R & 0x03U;
}

HAL_StatusTypeDef HAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t fifo, CAN_RxHeaderTypeDef *header, uint8_t data[8]) {
    if (hcan == 0 || hcan->Instance == 0 || header == 0 || data == 0 || fifo != CAN_RX_FIFO0) {
        return HAL_ERROR;
    }

    CAN_FIFOMailBox_TypeDef *rx = &hcan->Instance->sFIFOMailBox[0];
    header->IDE = rx->RIR & 0x04U;
    header->RTR = rx->RIR & 0x02U;
    header->StdId = (rx->RIR >> 21) & 0x7FFU;
    header->DLC = rx->RDTR & 0x0FU;
    data[0] = (uint8_t)(rx->RDLR & 0xFFU);
    data[1] = (uint8_t)((rx->RDLR >> 8) & 0xFFU);
    data[2] = (uint8_t)((rx->RDLR >> 16) & 0xFFU);
    data[3] = (uint8_t)((rx->RDLR >> 24) & 0xFFU);
    data[4] = (uint8_t)(rx->RDHR & 0xFFU);
    data[5] = (uint8_t)((rx->RDHR >> 8) & 0xFFU);
    data[6] = (uint8_t)((rx->RDHR >> 16) & 0xFFU);
    data[7] = (uint8_t)((rx->RDHR >> 24) & 0xFFU);
    hcan->Instance->RF0R |= 0x20U;
    return HAL_OK;
}

void HAL_CAN_IRQHandler(CAN_HandleTypeDef *hcan) {
    if (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0U) {
        HAL_CAN_RxFifo0MsgPendingCallback(hcan);
    }
}
