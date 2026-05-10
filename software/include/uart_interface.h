/**
 * @file uart_interface.h
 * @brief UART communication interface for debugging and control
 * @author Team
 * @date 2026-05-09
 */

#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Function Declarations */

/**
 * @brief Initialize UART interface
 * @return true if successful
 */
bool uart_init(void);

/**
 * @brief Send data via UART
 * @param data Pointer to data buffer
 * @param len Number of bytes to send
 * @return Number of bytes sent
 */
uint32_t uart_send(const uint8_t *data, uint32_t len);

/**
 * @brief Receive data from UART
 * @param data Pointer to data buffer
 * @param len Maximum bytes to receive
 * @return Number of bytes received
 */
uint32_t uart_receive(uint8_t *data, uint32_t len);

/**
 * @brief Check if data available in UART RX buffer
 * @return Number of bytes available
 */
uint32_t uart_available(void);

/**
 * @brief Push one received UART byte into the software RX buffer.
 *
 * Call this from USART ISR or DMA idle-line handling.
 */
void uart_rx_isr_handler(uint8_t byte);

/**
 * @brief Printf-style output to UART
 * @param format Format string
 * @param ... Variable arguments
 */
void uart_printf(const char *format, ...);

/**
 * @brief Send debug message
 * @param msg Message string
 */
void uart_debug_msg(const char *msg);

#ifdef __cplusplus
}
#endif

#endif /* UART_INTERFACE_H */
