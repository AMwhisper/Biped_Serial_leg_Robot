/**
 * @file system.h
 * @brief System initialization and management
 * @author Team
 * @date 2026-05-09
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* System state enumeration */
typedef enum {
    SYS_STATE_IDLE = 0,
    SYS_STATE_INITIALIZING,
    SYS_STATE_RUNNING,
    SYS_STATE_ERROR,
    SYS_STATE_SHUTDOWN
} system_state_t;

/* Function Declarations */

/**
 * @brief Initialize system
 * @return true if successful
 */
bool system_init(void);

/**
 * @brief Get system state
 * @return Current system state
 */
system_state_t system_get_state(void);

/**
 * @brief Set system state
 * @param state New system state
 */
void system_set_state(system_state_t state);

/**
 * @brief Main system update
 * @return true if successful
 */
bool system_update(void);

/**
 * @brief Shutdown system
 */
void system_shutdown(void);

/**
 * @brief Get system uptime (ms)
 * @return Uptime in milliseconds
 */
uint32_t system_get_uptime(void);

/**
 * @brief Get system error code
 * @return Error code (0 = no error)
 */
uint32_t system_get_error(void);

/**
 * @brief Emergency stop
 */
void system_emergency_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_H */
