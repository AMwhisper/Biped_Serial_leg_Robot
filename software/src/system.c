/**
 * @file system.c
 * @brief Minimal system initialization for BSP and motor drivers.
 */

#include "system.h"
#include "can_driver.h"
#include "motor_controller.h"
#include "motor_config.h"

#if defined(__has_include)
#if __has_include("stm32f4xx_hal.h")
#include "stm32f4xx_hal.h"
#define SYSTEM_USE_HAL 1
#endif
#endif

#ifndef SYSTEM_USE_HAL
#define SYSTEM_USE_HAL 0
#endif

static system_state_t system_state = SYS_STATE_IDLE;
static uint32_t system_uptime = 0U;
static uint32_t system_error = 0U;
static uint32_t last_update_time = 0U;

#define SYSTEM_UPDATE_PERIOD_MS 2U

bool system_init(void) {
    system_state = SYS_STATE_INITIALIZING;
    system_error = 0U;
    system_uptime = 0U;
    last_update_time = 0U;

    if (!can_driver_init()) {
        system_error |= 0x01U;
        system_state = SYS_STATE_ERROR;
        return false;
    }

    if (!motor_controller_init()) {
        system_error |= 0x02U;
        system_state = SYS_STATE_ERROR;
        return false;
    }

    system_state = SYS_STATE_RUNNING;
    return true;
}

system_state_t system_get_state(void) {
    return system_state;
}

void system_set_state(system_state_t state) {
    system_state = state;
}

bool system_update(void) {
#if SYSTEM_USE_HAL
    uint32_t current_time = HAL_GetTick();
#else
    uint32_t current_time = last_update_time + SYSTEM_UPDATE_PERIOD_MS;
#endif

    if ((current_time - last_update_time) >= SYSTEM_UPDATE_PERIOD_MS) {
        system_uptime = current_time;
        last_update_time = current_time;
    }

    if (system_state != SYS_STATE_RUNNING) {
        return false;
    }

    if (!motor_update()) {
        system_error |= 0x10U;
        system_state = SYS_STATE_ERROR;
        return false;
    }

    return true;
}

void system_shutdown(void) {
    system_state = SYS_STATE_SHUTDOWN;

    for (uint8_t id = 1U; id <= TOTAL_MOTORS; ++id) {
        (void)motor_disable(id);
    }

    can_driver_deinit();
    system_uptime = 0U;
}

uint32_t system_get_uptime(void) {
    return system_uptime;
}

uint32_t system_get_error(void) {
    return system_error;
}

void system_emergency_stop(void) {
    system_state = SYS_STATE_ERROR;

    for (uint8_t id = 1U; id <= TOTAL_MOTORS; ++id) {
        (void)motor_disable(id);
    }

    system_error |= 0x80U;
}
