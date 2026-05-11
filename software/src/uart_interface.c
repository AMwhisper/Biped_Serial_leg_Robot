/**
 * @file uart_interface.c
 * @brief UART communication implementation
 * @author Team
 * @date 2026-05-09
 */

#include "uart_interface.h"
#include "dma.h"
#include "dbus_receiver.h"
#include "usart.h"
#include "stm32f4xx_hal.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define UART_RX_BUFFER_SIZE 256
#define UART_TX_BUFFER_SIZE 256
#define DBUS_DMA_RX_SIZE    36U

static uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static uint8_t tx_buffer[UART_TX_BUFFER_SIZE];
static uint8_t dbus_dma_buffer[DBUS_DMA_RX_SIZE];
static uint32_t rx_head = 0;
static uint32_t rx_tail = 0;
static uint32_t rx_count = 0;

/**
 * @brief Initialize UART interface
 */
bool uart_init(void) {
    MX_DMA_Init();
    MX_USART3_UART_Init();

    memset(rx_buffer, 0, sizeof(rx_buffer));
    memset(tx_buffer, 0, sizeof(tx_buffer));
    memset(dbus_dma_buffer, 0, sizeof(dbus_dma_buffer));
    rx_head = 0;
    rx_tail = 0;
    rx_count = 0;

    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
    if (HAL_UARTEx_ReceiveToIdle_DMA(&huart3, dbus_dma_buffer, sizeof(dbus_dma_buffer)) != HAL_OK) {
        return false;
    }
    
    return true;
}

/**
 * @brief Send data via UART
 */
uint32_t uart_send(const uint8_t *data, uint32_t len) {
    if (data == NULL || len == 0) {
        return 0;
    }
    
    if (HAL_UART_Transmit(&huart3, (uint8_t *)data, (uint16_t)len, 10U) != HAL_OK) {
        return 0;
    }

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

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {
    if (huart->Instance != USART3) {
        return;
    }

    for (uint16_t offset = 0U;
         offset + DBUS_FRAME_LEN <= size && offset + DBUS_FRAME_LEN <= sizeof(dbus_dma_buffer);
         offset = (uint16_t)(offset + DBUS_FRAME_LEN)) {
        (void)dbus_parse_frame(&dbus_dma_buffer[offset]);
    }

    (void)HAL_UARTEx_ReceiveToIdle_DMA(&huart3, dbus_dma_buffer, sizeof(dbus_dma_buffer));
}
