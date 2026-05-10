/**
 * @file uart_interface.c
 * @brief UART communication implementation
 * @author Team
 * @date 2026-05-09
 */

#include "uart_interface.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define UART_RX_BUFFER_SIZE 256
#define UART_TX_BUFFER_SIZE 256

static uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static uint8_t tx_buffer[UART_TX_BUFFER_SIZE];
static uint32_t rx_head = 0;
static uint32_t rx_tail = 0;
static uint32_t rx_count = 0;

/**
 * @brief Initialize UART interface
 */
bool uart_init(void) {
    // TODO: Initialize STM32 UART peripheral
    // - Configure USART1 (PA9/PA10)
    // - Set baudrate to 115200
    // - Enable RX interrupt
    
    memset(rx_buffer, 0, sizeof(rx_buffer));
    memset(tx_buffer, 0, sizeof(tx_buffer));
    rx_head = 0;
    rx_tail = 0;
    rx_count = 0;
    
    return true;
}

/**
 * @brief Send data via UART
 */
uint32_t uart_send(const uint8_t *data, uint32_t len) {
    if (data == NULL || len == 0) {
        return 0;
    }
    
    // TODO: Implement actual UART transmission
    // HAL_UART_Transmit(&huart1, (uint8_t *)data, len, HAL_MAX_DELAY);
    
    return len;
}

/**
 * @brief Receive data from UART
 */
uint32_t uart_receive(uint8_t *data, uint32_t len) {
    if (data == NULL || len == 0) {
        return 0;
    }
    
    uint32_t bytes_read = 0;
    
    while (rx_count > 0 && bytes_read < len) {
        data[bytes_read++] = rx_buffer[rx_head];
        rx_head = (rx_head + 1) % UART_RX_BUFFER_SIZE;
        rx_count--;
    }
    
    return bytes_read;
}

/**
 * @brief Check if data available
 */
uint32_t uart_available(void) {
    return rx_count;
}

/**
 * @brief Printf-style output
 */
void uart_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    static char buffer[256];
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    
    va_end(args);
    
    if (len > 0) {
        uart_send((uint8_t *)buffer, len);
    }
}

/**
 * @brief Send debug message
 */
void uart_debug_msg(const char *msg) {
    if (msg != NULL) {
        uart_send((uint8_t *)msg, strlen(msg));
        uart_send((uint8_t *)"\r\n", 2);
    }
}

/**
 * @brief UART RX interrupt handler (to be called from ISR)
 */
void uart_rx_isr_handler(uint8_t byte) {
    if (rx_count < UART_RX_BUFFER_SIZE) {
        rx_buffer[rx_tail] = byte;
        rx_tail = (rx_tail + 1) % UART_RX_BUFFER_SIZE;
        rx_count++;
    }
}
