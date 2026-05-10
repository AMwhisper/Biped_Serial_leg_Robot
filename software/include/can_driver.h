/**
 * @file can_driver.h
 * @brief CAN bus driver interface for RoboMaster C Type
 * @author Team
 * @date 2026-05-09
 */

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* CAN Message Structure */
typedef struct {
    uint32_t can_id;
    uint8_t dlc;                    // Data Length Code
    uint8_t data[8];
    uint32_t timestamp;
} can_message_t;

/* CAN Driver Status */
typedef enum {
    CAN_STATUS_OK = 0,
    CAN_STATUS_ERROR,
    CAN_STATUS_TIMEOUT,
    CAN_STATUS_FULL
} can_status_t;

/* Function Declarations */

/**
 * @brief Initialize CAN driver
 * @return true if successful
 */
bool can_driver_init(void);

/**
 * @brief Deinitialize CAN driver
 */
void can_driver_deinit(void);

/**
 * @brief Send CAN message
 * @param msg Pointer to CAN message
 * @return CAN status
 */
can_status_t can_send(const can_message_t *msg);

/**
 * @brief Receive CAN message
 * @param msg Pointer to CAN message buffer
 * @return CAN status
 */
can_status_t can_receive(can_message_t *msg);

/**
 * @brief Check if message received
 * @return true if message available
 */
bool can_message_available(void);

/**
 * @brief Set CAN filter
 * @param id CAN ID to filter
 * @param mask Mask for ID
 * @return true if successful
 */
bool can_set_filter(uint32_t id, uint32_t mask);

/**
 * @brief Get CAN status
 * @return CAN status
 */
can_status_t can_get_status(void);

/**
 * @brief Push a received CAN frame into the driver RX queue.
 *
 * This is used by the STM32 HAL RX callback/ISR glue.
 */
void can_rx_isr_handler(uint8_t *data, uint32_t can_id, uint8_t dlc);

#ifdef __cplusplus
}
#endif

#endif /* CAN_DRIVER_H */
