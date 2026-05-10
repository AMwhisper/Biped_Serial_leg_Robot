/**
 * @file stm32f4xx_hal.h
 * @brief Minimal STM32F4 HAL-compatible layer used by this Keil target.
 */

#ifndef STM32F4XX_HAL_H
#define STM32F4XX_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define __IO volatile

typedef enum {
    HAL_OK = 0x00U,
    HAL_ERROR = 0x01U,
    HAL_BUSY = 0x02U,
    HAL_TIMEOUT = 0x03U,
} HAL_StatusTypeDef;

typedef struct {
    __IO uint32_t MODER;
    __IO uint32_t OTYPER;
    __IO uint32_t OSPEEDR;
    __IO uint32_t PUPDR;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t LCKR;
    __IO uint32_t AFR[2];
} GPIO_TypeDef;

typedef struct {
    __IO uint32_t CR;
    __IO uint32_t PLLCFGR;
    __IO uint32_t CFGR;
    __IO uint32_t CIR;
    __IO uint32_t AHB1RSTR;
    __IO uint32_t AHB2RSTR;
    __IO uint32_t AHB3RSTR;
    uint32_t RESERVED0;
    __IO uint32_t APB1RSTR;
    __IO uint32_t APB2RSTR;
    uint32_t RESERVED1[2];
    __IO uint32_t AHB1ENR;
    __IO uint32_t AHB2ENR;
    __IO uint32_t AHB3ENR;
    uint32_t RESERVED2;
    __IO uint32_t APB1ENR;
    __IO uint32_t APB2ENR;
} RCC_TypeDef;

typedef struct {
    __IO uint32_t TIR;
    __IO uint32_t TDTR;
    __IO uint32_t TDLR;
    __IO uint32_t TDHR;
} CAN_TxMailBox_TypeDef;

typedef struct {
    __IO uint32_t RIR;
    __IO uint32_t RDTR;
    __IO uint32_t RDLR;
    __IO uint32_t RDHR;
} CAN_FIFOMailBox_TypeDef;

typedef struct {
    __IO uint32_t FR1;
    __IO uint32_t FR2;
} CAN_FilterRegister_TypeDef;

typedef struct {
    __IO uint32_t MCR;
    __IO uint32_t MSR;
    __IO uint32_t TSR;
    __IO uint32_t RF0R;
    __IO uint32_t RF1R;
    __IO uint32_t IER;
    __IO uint32_t ESR;
    __IO uint32_t BTR;
    uint32_t RESERVED0[88];
    CAN_TxMailBox_TypeDef sTxMailBox[3];
    CAN_FIFOMailBox_TypeDef sFIFOMailBox[2];
    uint32_t RESERVED1[12];
    __IO uint32_t FMR;
    __IO uint32_t FM1R;
    uint32_t RESERVED2;
    __IO uint32_t FS1R;
    uint32_t RESERVED3;
    __IO uint32_t FFA1R;
    uint32_t RESERVED4;
    __IO uint32_t FA1R;
    uint32_t RESERVED5[8];
    CAN_FilterRegister_TypeDef sFilterRegister[28];
} CAN_TypeDef;

#define PERIPH_BASE        0x40000000UL
#define APB1PERIPH_BASE    PERIPH_BASE
#define AHB1PERIPH_BASE    (PERIPH_BASE + 0x00020000UL)
#define GPIOB_BASE         (AHB1PERIPH_BASE + 0x0400UL)
#define RCC_BASE           (AHB1PERIPH_BASE + 0x3800UL)
#define CAN1_BASE          (APB1PERIPH_BASE + 0x6400UL)
#define CAN2_BASE          (APB1PERIPH_BASE + 0x6800UL)

#define GPIOB              ((GPIO_TypeDef *)GPIOB_BASE)
#define RCC                ((RCC_TypeDef *)RCC_BASE)
#define CAN1               ((CAN_TypeDef *)CAN1_BASE)
#define CAN2               ((CAN_TypeDef *)CAN2_BASE)

#define GPIO_PIN_0         ((uint16_t)0x0001U)
#define GPIO_PIN_1         ((uint16_t)0x0002U)
#define GPIO_PIN_5         ((uint16_t)0x0020U)
#define GPIO_PIN_6         ((uint16_t)0x0040U)
#define GPIO_PIN_11        ((uint16_t)0x0800U)
#define GPIO_PIN_12        ((uint16_t)0x1000U)
#define GPIO_PIN_13        ((uint16_t)0x2000U)

#define GPIO_MODE_AF_PP    0x00000002U
#define GPIO_NOPULL        0x00000000U
#define GPIO_PULLUP        0x00000001U
#define GPIO_SPEED_FREQ_VERY_HIGH 0x00000003U
#define GPIO_AF9_CAN2      9U

typedef struct {
    uint32_t Pin;
    uint32_t Mode;
    uint32_t Pull;
    uint32_t Speed;
    uint32_t Alternate;
} GPIO_InitTypeDef;

typedef struct {
    uint32_t Prescaler;
    uint32_t Mode;
    uint32_t SyncJumpWidth;
    uint32_t TimeSeg1;
    uint32_t TimeSeg2;
    uint32_t TimeTriggeredMode;
    uint32_t AutoBusOff;
    uint32_t AutoWakeUp;
    uint32_t AutoRetransmission;
    uint32_t ReceiveFifoLocked;
    uint32_t TransmitFifoPriority;
} CAN_InitTypeDef;

typedef struct {
    CAN_TypeDef *Instance;
    CAN_InitTypeDef Init;
} CAN_HandleTypeDef;

typedef struct {
    uint32_t StdId;
    uint32_t ExtId;
    uint32_t IDE;
    uint32_t RTR;
    uint32_t DLC;
    uint32_t TransmitGlobalTime;
} CAN_TxHeaderTypeDef;

typedef struct {
    uint32_t StdId;
    uint32_t ExtId;
    uint32_t IDE;
    uint32_t RTR;
    uint32_t DLC;
    uint32_t Timestamp;
    uint32_t FilterMatchIndex;
} CAN_RxHeaderTypeDef;

typedef struct {
    uint32_t FilterIdHigh;
    uint32_t FilterIdLow;
    uint32_t FilterMaskIdHigh;
    uint32_t FilterMaskIdLow;
    uint32_t FilterFIFOAssignment;
    uint32_t FilterBank;
    uint32_t FilterMode;
    uint32_t FilterScale;
    uint32_t FilterActivation;
    uint32_t SlaveStartFilterBank;
} CAN_FilterTypeDef;

#define ENABLE             1U
#define DISABLE            0U

#define CAN_MODE_NORMAL    0U
#define CAN_SJW_1TQ        0U
#define CAN_BS1_4TQ        (3U << 16)
#define CAN_BS1_11TQ       (10U << 16)
#define CAN_BS1_12TQ       (11U << 16)
#define CAN_BS2_2TQ        (1U << 20)
#define CAN_BS2_4TQ        (3U << 20)
#define CAN_BS2_5TQ        (4U << 20)
#define CAN_ID_STD         0U
#define CAN_ID_EXT         4U
#define CAN_RTR_DATA       0U
#define CAN_RX_FIFO0       0U
#define CAN_FILTERMODE_IDMASK 0U
#define CAN_FILTERSCALE_32BIT 1U
#define CAN_IT_RX_FIFO0_MSG_PENDING 0x00000002U

void HAL_Init(void);
uint32_t HAL_GetTick(void);
void HAL_IncTick(void);
void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
HAL_StatusTypeDef HAL_CAN_Init(CAN_HandleTypeDef *hcan);
HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filter);
HAL_StatusTypeDef HAL_CAN_Start(CAN_HandleTypeDef *hcan);
HAL_StatusTypeDef HAL_CAN_Stop(CAN_HandleTypeDef *hcan);
HAL_StatusTypeDef HAL_CAN_ActivateNotification(CAN_HandleTypeDef *hcan, uint32_t active_its);
HAL_StatusTypeDef HAL_CAN_DeactivateNotification(CAN_HandleTypeDef *hcan, uint32_t inactive_its);
HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *header, uint8_t data[8], uint32_t *mailbox);
uint32_t HAL_CAN_GetRxFifoFillLevel(CAN_HandleTypeDef *hcan, uint32_t fifo);
HAL_StatusTypeDef HAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t fifo, CAN_RxHeaderTypeDef *header, uint8_t data[8]);
void HAL_CAN_IRQHandler(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);

void NVIC_EnableIRQ(int32_t irq);
void NVIC_SetPriority(int32_t irq, uint32_t priority);
void NVIC_DisableIRQ(int32_t irq);

#define HAL_NVIC_SetPriority(irq, preempt_priority, sub_priority) NVIC_SetPriority((irq), (preempt_priority))
#define HAL_NVIC_EnableIRQ(irq) NVIC_EnableIRQ((irq))
#define HAL_NVIC_DisableIRQ(irq) NVIC_DisableIRQ((irq))

#define CAN2_RX0_IRQn      65

#define __HAL_RCC_GPIOB_CLK_ENABLE() do { RCC->AHB1ENR |= (1UL << 1); (void)RCC->AHB1ENR; } while (0)
#define __HAL_RCC_CAN1_CLK_ENABLE()  do { RCC->APB1ENR |= (1UL << 25); (void)RCC->APB1ENR; } while (0)
#define __HAL_RCC_CAN2_CLK_ENABLE()  do { RCC->APB1ENR |= (1UL << 26); (void)RCC->APB1ENR; } while (0)
#define __HAL_RCC_CAN1_CLK_DISABLE() do { RCC->APB1ENR &= ~(1UL << 25); (void)RCC->APB1ENR; } while (0)
#define __HAL_RCC_CAN2_CLK_DISABLE() do { RCC->APB1ENR &= ~(1UL << 26); (void)RCC->APB1ENR; } while (0)

#ifdef __cplusplus
}
#endif

#endif /* STM32F4XX_HAL_H */
