/**
 * @file data_logger.h
 * @brief Data logging and telemetry interface
 * @author Team
 * @date 2026-05-09
 */

#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Log entry structure */
typedef struct {
    uint32_t timestamp;
    float motor_positions[8];
    float motor_velocities[8];
    float motor_torques[8];
    float leg_positions[4][3];   // 4 legs, 3 coordinates each
    uint32_t system_state;
    uint32_t error_flags;
} log_entry_t;

/* Function Declarations */

/**
 * @brief Initialize data logger
 * @return true if successful
 */
bool data_logger_init(void);

/**
 * @brief Log current system state
 * @return true if successful
 */
bool data_logger_log_state(void);

/**
 * @brief Get number of logged entries
 * @return Number of entries
 */
uint32_t data_logger_get_count(void);

/**
 * @brief Get log entry by index
 * @param index Entry index
 * @param entry Pointer to log entry buffer
 * @return true if successful
 */
bool data_logger_get_entry(uint32_t index, log_entry_t *entry);

/**
 * @brief Clear log buffer
 */
void data_logger_clear(void);

/**
 * @brief Export log to file (stub for embedded system)
 * @param filename Output filename
 * @return true if successful
 */
bool data_logger_export(const char *filename);

#ifdef __cplusplus
}
#endif

#endif /* DATA_LOGGER_H */
