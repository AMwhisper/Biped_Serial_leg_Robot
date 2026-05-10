/**
 * @file can_driver.c
 * @brief CAN bus driver implementation for RoboMaster C Type
 * @author Team
 * @date 2026-05-09
 */

#include "can_driver.h"
#include "hardware_config.h"
#include <string.h>

#if defined(__has_include)
#if __has_include("stm32f4xx_hal.h")
#include "stm32f4xx_hal.h"
#define CAN_DRIVER_USE_HAL 1
#endif
#endif

#ifndef CAN_DRIVER_USE_HAL
#define CAN_DRIVER_USE_HAL 0
#endif

#if CAN_DRIVER_USE_HAL
#include "can.h"
#ifndef CAN_DRIVER_HANDLE
#define CAN_DRIVER_HANDLE hcan2
#endif
extern CAN_HandleTypeDef CAN_DRIVER_HANDLE;
#endif

/* CAN Message Queue */
#define CAN_RX_QUEUE_SIZE   32
#define CAN_TX_QUEUE_SIZE   32

typedef struct {
    can_message_t messages[CAN_RX_QUEUE_SIZE];
    uint16_t head;
    uint16_t tail;
    uint16_t count;
} can_queue_t;

static can_queue_t rx_queue = {0};
static can_queue_t tx_queue = {0};
static can_status_t driver_status = CAN_STATUS_OK;

#if CAN_DRIVER_USE_HAL
static uint32_t next_filter_bank;
#endif

static void queue_reset(can_queue_t *queue) {
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
}

static void queue_push_drop_oldest(can_queue_t *queue, const can_message_t *msg) {
    if (queue->count >= CAN_RX_QUEUE_SIZE) {
        queue->head = (uint16_t)((queue->head + 1U) % CAN_RX_QUEUE_SIZE);
        queue->count--;
    }

    memcpy(&queue->messages[queue->tail], msg, sizeof(*msg));
    queue->tail = (uint16_t)((queue->tail + 1U) % CAN_RX_QUEUE_SIZE);
    queue->count++;
}

#if CAN_DRIVER_USE_HAL
static bool configure_hal_filter(uint32_t id, uint32_t mask) {
    CAN_FilterTypeDef filter;

    if (next_filter_bank >= 28U) {
        driver_status = CAN_STATUS_FULL;
        return false;
    }

    memset(&filter, 0, sizeof(filter));
    filter.FilterBank = next_filter_bank++;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterIdHigh = (uint16_t)((id & 0x7FFU) << 5);
    filter.FilterIdLow = 0x0000;
    filter.FilterMaskIdHigh = (uint16_t)((mask & 0x7FFU) << 5);
    filter.FilterMaskIdLow = 0x0000;
    filter.FilterFIFOAssignment = CAN_RX_FIFO0;
    filter.FilterActivation = ENABLE;
    filter.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(&CAN_DRIVER_HANDLE, &filter) != HAL_OK) {
        driver_status = CAN_STATUS_ERROR;
        return false;
    }

    return true;
}
#endif

/**
 * @brief Initialize CAN driver
 */
bool can_driver_init(void) {
    queue_reset(&rx_queue);
    queue_reset(&tx_queue);

    driver_status = CAN_STATUS_OK;

#if CAN_DRIVER_USE_HAL
    MX_CAN2_Init();
    next_filter_bank = 14;

    if (!configure_hal_filter(0x000U, 0x000U)) {
        return false;
    }

    if (HAL_CAN_Start(&CAN_DRIVER_HANDLE) != HAL_OK) {
        driver_status = CAN_STATUS_ERROR;
        return false;
    }

    if (HAL_CAN_ActivateNotification(&CAN_DRIVER_HANDLE, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        driver_status = CAN_STATUS_ERROR;
        return false;
    }
#endif

    return true;
}

/**
 * @brief Deinitialize CAN driver
 */
void can_driver_deinit(void) {
#if CAN_DRIVER_USE_HAL
    (void)HAL_CAN_DeactivateNotification(&CAN_DRIVER_HANDLE, CAN_IT_RX_FIFO0_MSG_PENDING);
    (void)HAL_CAN_Stop(&CAN_DRIVER_HANDLE);
#endif
    driver_status = CAN_STATUS_OK;
}

/**
 * @brief Send CAN message
 */
can_status_t can_send(const can_message_t *msg) {
    if (msg == NULL) {
        return CAN_STATUS_ERROR;
    }

#if CAN_DRIVER_USE_HAL
    CAN_TxHeaderTypeDef header;
    uint32_t mailbox;

    memset(&header, 0, sizeof(header));
    header.StdId = msg->can_id & 0x7FFU;
    header.IDE = CAN_ID_STD;
    header.RTR = CAN_RTR_DATA;
    header.DLC = msg->dlc;
    header.TransmitGlobalTime = DISABLE;

    if (HAL_CAN_AddTxMessage(&CAN_DRIVER_HANDLE, &header, (uint8_t *)msg->data, &mailbox) != HAL_OK) {
        driver_status = CAN_STATUS_ERROR;
        return CAN_STATUS_ERROR;
    }
#endif

    if (tx_queue.count >= CAN_TX_QUEUE_SIZE) {
        tx_queue.head = (tx_queue.head + 1) % CAN_TX_QUEUE_SIZE;
        tx_queue.count--;
    }
    
    // Add message to queue
    memcpy(&tx_queue.messages[tx_queue.tail], msg, sizeof(can_message_t));
    tx_queue.tail = (tx_queue.tail + 1) % CAN_TX_QUEUE_SIZE;
    tx_queue.count++;
    
    // TODO: Actually send via CAN peripheral
    // HAL_CAN_AddTxMessage(&hcan, &pHeader, pData, &pTxMailbox);
    
    return CAN_STATUS_OK;
}

/**
 * @brief Receive CAN message
 */
can_status_t can_receive(can_message_t *msg) {
    if (msg == NULL) {
        return CAN_STATUS_ERROR;
    }
    
    if (rx_queue.count == 0) {
        return CAN_STATUS_TIMEOUT;
    }
    
    // Get message from queue
    memcpy(msg, &rx_queue.messages[rx_queue.head], sizeof(can_message_t));
    rx_queue.head = (rx_queue.head + 1) % CAN_RX_QUEUE_SIZE;
    rx_queue.count--;
    
    return CAN_STATUS_OK;
}

/**
 * @brief Check if message received
 */
bool can_message_available(void) {
    return rx_queue.count > 0;
}

/**
 * @brief Set CAN filter
 */
bool can_set_filter(uint32_t id, uint32_t mask) {
#if CAN_DRIVER_USE_HAL
    /*
     * A broad motor-feedback filter is installed before HAL_CAN_Start().
     * Keep later filter calls harmless because STM32 HAL projects often call
     * this after CAN is already running.
     */
    (void)id;
    (void)mask;
    return true;
#else
    (void)id;
    (void)mask;
    return true;
#endif
}

/**
 * @brief Get CAN status
 */
can_status_t can_get_status(void) {
    return driver_status;
}

/**
 * @brief CAN receive interrupt handler (to be called from ISR)
 */
void can_rx_isr_handler(uint8_t *data, uint32_t can_id, uint8_t dlc) {
    if (data == NULL || dlc > 8U) {
        driver_status = CAN_STATUS_ERROR;
        return;
    }

    can_message_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.can_id = can_id;
    msg.dlc = dlc;
    memcpy(msg.data, data, dlc);
    queue_push_drop_oldest(&rx_queue, &msg);
}

#if CAN_DRIVER_USE_HAL
__attribute__((weak))
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef header;
    uint8_t data[8];

    if (hcan != &CAN_DRIVER_HANDLE) {
        return;
    }

    while (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0U) {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &header, data) != HAL_OK) {
            driver_status = CAN_STATUS_ERROR;
            return;
        }

        if (header.IDE == CAN_ID_STD && header.RTR == CAN_RTR_DATA) {
            can_rx_isr_handler(data, header.StdId, header.DLC);
        }
    }
}
#endif
