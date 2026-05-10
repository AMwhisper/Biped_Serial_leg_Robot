/**
 * @file system.c
 * @brief System initialization and management implementation
 * @author Team
 * @date 2026-05-09
 */

#include "system.h"
#include "can_driver.h"
#include "dbus_receiver.h"
#include "motor_controller.h"
#include "leg_controller.h"
#include "motor_config.h"
#include "uart_interface.h"
#include "wheel_leg_control.h"
#include <string.h>
#include <stdio.h>

/* System variables */
static system_state_t system_state = SYS_STATE_IDLE;
static uint32_t system_uptime = 0;
static uint32_t system_error = 0;
static uint32_t last_update_time = 0;

#define SYSTEM_UPDATE_PERIOD_MS 2  // 500 Hz

/**
 * @brief Initialize system
 */
bool system_init(void) {
    system_state = SYS_STATE_INITIALIZING;
    system_error = 0;
    system_uptime = 0;
    
    if (!uart_init()) {
        system_error |= 0x40;
        system_state = SYS_STATE_ERROR;
        return false;
    }

    dbus_init();

    // Initialize CAN driver
    if (!can_driver_init()) {
        system_error |= 0x01;
        system_state = SYS_STATE_ERROR;
        return false;
    }
    
    // Initialize motor controller
    if (!motor_controller_init()) {
        system_error |= 0x02;
        system_state = SYS_STATE_ERROR;
        return false;
    }
    
    // Initialize leg controller
    if (!leg_controller_init()) {
        system_error |= 0x04;
        system_state = SYS_STATE_ERROR;
        return false;
    }
    
    if (!wheel_leg_control_init()) {
        system_error |= 0x08;
        system_state = SYS_STATE_ERROR;
        return false;
    }

    // TODO: Initialize other subsystems
    // - Power management
    // - Communication interface
    // - Sensor interface
    
    system_state = SYS_STATE_RUNNING;
    return true;
}

/**
 * @brief Get system state
 */
system_state_t system_get_state(void) {
    return system_state;
}

/**
 * @brief Set system state
 */
void system_set_state(system_state_t state) {
    system_state = state;
}

/**
 * @brief Main system update
 */
bool system_update(void) {
    // Get current time (TODO: implement timer)
    uint32_t current_time = 0;  // system_get_time_ms();
    
    // Update uptime
    if (current_time - last_update_time >= SYSTEM_UPDATE_PERIOD_MS) {
        system_uptime = current_time;
        last_update_time = current_time;
    }
    
    if (system_state != SYS_STATE_RUNNING) {
        return false;
    }
    
    // Update motor controller
    if (!motor_update()) {
        system_error |= 0x10;
        system_state = SYS_STATE_ERROR;
        return false;
    }
    
    // Update leg controller
    if (!leg_update()) {
        system_error |= 0x20;
        system_state = SYS_STATE_ERROR;
        return false;
    }
    
    // TODO: Update other subsystems
    
    return true;
}

/**
 * @brief Shutdown system
 */
void system_shutdown(void) {
    system_state = SYS_STATE_SHUTDOWN;
    
    // Disable all motors
    for (uint8_t i = 1; i <= TOTAL_MOTORS; i++) {
        motor_disable(i);
    }
    
    // Deinitialize drivers
    can_driver_deinit();
    
    system_uptime = 0;
}

/**
 * @brief Get system uptime
 */
uint32_t system_get_uptime(void) {
    return system_uptime;
}

/**
 * @brief Get system error code
 */
uint32_t system_get_error(void) {
    return system_error;
}

/**
 * @brief Emergency stop
 */
void system_emergency_stop(void) {
    system_state = SYS_STATE_ERROR;
    
    // Immediately disable all motors
    for (uint8_t i = 1; i <= TOTAL_MOTORS; i++) {
        motor_disable(i);
    }
    
    system_error |= 0x80;
}
