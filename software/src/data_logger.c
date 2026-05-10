/**
 * @file data_logger.c
 * @brief Data logging implementation
 * @author Team
 * @date 2026-05-09
 */

#include "data_logger.h"
#include "motor_controller.h"
#include "leg_controller.h"
#include <string.h>
#include <stdio.h>

#define LOG_BUFFER_SIZE 1000

static log_entry_t log_buffer[LOG_BUFFER_SIZE];
static uint32_t log_index = 0;
static uint32_t log_count = 0;

/**
 * @brief Initialize data logger
 */
bool data_logger_init(void) {
    memset(log_buffer, 0, sizeof(log_buffer));
    log_index = 0;
    log_count = 0;
    return true;
}

/**
 * @brief Log current system state
 */
bool data_logger_log_state(void) {
    if (log_count >= LOG_BUFFER_SIZE) {
        return false;  // Buffer full
    }
    
    log_entry_t *entry = &log_buffer[log_index];
    entry->timestamp = 0;  // TODO: Get system time
    
    // Log motor states
    for (int i = 0; i < 8; i++) {
        motor_t *motor = motor_get(i + 1);
        if (motor != NULL) {
            entry->motor_positions[i] = motor->position;
            entry->motor_velocities[i] = motor->velocity;
            entry->motor_torques[i] = motor->torque;
        }
    }
    
    // Log leg positions
    for (int i = 0; i < 4; i++) {
        leg_state_t leg_state;
        leg_get_state(i, &leg_state);
        entry->leg_positions[i][0] = leg_state.position.x;
        entry->leg_positions[i][1] = leg_state.position.y;
        entry->leg_positions[i][2] = leg_state.position.z;
    }
    
    log_index = (log_index + 1) % LOG_BUFFER_SIZE;
    if (log_count < LOG_BUFFER_SIZE) {
        log_count++;
    }
    
    return true;
}

/**
 * @brief Get number of logged entries
 */
uint32_t data_logger_get_count(void) {
    return log_count;
}

/**
 * @brief Get log entry by index
 */
bool data_logger_get_entry(uint32_t index, log_entry_t *entry) {
    if (index >= log_count || entry == NULL) {
        return false;
    }
    
    memcpy(entry, &log_buffer[index], sizeof(log_entry_t));
    return true;
}

/**
 * @brief Clear log buffer
 */
void data_logger_clear(void) {
    memset(log_buffer, 0, sizeof(log_buffer));
    log_index = 0;
    log_count = 0;
}

/**
 * @brief Export log to file
 */
bool data_logger_export(const char *filename) {
    // TODO: Implement file export
    // For embedded system, this would write to SD card or transmit over UART
    
    (void)filename;  // Avoid unused parameter warning
    return true;
}
