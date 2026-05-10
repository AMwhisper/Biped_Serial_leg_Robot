/**
 * @file main.c
 * @brief Main application for RoboMaster C Type Quadruped Robot
 * @author Team
 * @date 2026-05-09
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "system.h"
#include "bringup_config.h"
#include "can_driver.h"
#include "dbus_receiver.h"
#include "motor_controller.h"
#include "leg_controller.h"
#include "motor_config.h"
#include "uart_interface.h"
#include "wheel_leg_control.h"

#if defined(__has_include)
#if __has_include("stm32f4xx_hal.h")
#include "stm32f4xx_hal.h"
#define MAIN_USE_HAL 1
#endif
#endif

#ifndef MAIN_USE_HAL
#define MAIN_USE_HAL 0
#endif

/* Control loop timer */
#define CONTROL_FREQ_HZ         500
#define CONTROL_PERIOD_MS       (1000 / CONTROL_FREQ_HZ)

/* Global variables */
static uint32_t loop_counter = 0;
static bool demo_mode_enabled = false;
static bool remote_enabled = false;

static void bringup_motor_test_step(void) {
#if BRINGUP_MOTOR_TEST_ENABLE
    static bool motor_started = false;
    static uint32_t last_toggle_ms = 0;
    static float torque = BRINGUP_TEST_TORQUE_NM;
    uint32_t now = HAL_GetTick();

    if (!motor_started) {
        motor_enable(BRINGUP_TEST_MOTOR_ID);
        motor_started = true;
    }

    if (now - last_toggle_ms >= BRINGUP_TEST_PERIOD_MS) {
        torque = -torque;
        last_toggle_ms = now;
    }

    motor_set_torque(BRINGUP_TEST_MOTOR_ID, torque);
#endif
}

/* Forward declarations */
extern void SystemInit(void);
extern uint32_t HAL_GetTick(void);
void HardFault_Handler(void);

/**
 * @brief Demo control - simple leg movement pattern
 */
static void demo_control_step(void) {
    static uint32_t demo_counter = 0;
    demo_counter++;
    
    // Simple sine wave motion for demonstration
    float phase = (demo_counter / 100.0f);
    
    // Enable motors for demo
    for (int i = 1; i <= TOTAL_MOTORS; i++) {
        motor_enable(i);
    }
    
    // Apply torque based on sine wave
    for (int i = 1; i <= 4; i++) {
        float torque = 2.0f * sinf(phase + i * 3.14159f / 2.0f);
        motor_set_torque(i, torque);
    }
    
    if (demo_counter > 500) {
        demo_counter = 0;
    }
}

/**
 * @brief Real-time control loop
 */
static void control_loop(void) {
    uint8_t byte;
    while (uart_receive(&byte, 1U) == 1U) {
        dbus_rx_byte(byte);
    }
    dbus_update_timeout(HAL_GetTick());

    if (!BRINGUP_MOTOR_TEST_ENABLE && BRINGUP_ENABLE_REMOTE_CONTROL) {
        dbus_motion_cmd_t remote = dbus_get_motion_cmd();
        if (remote.emergency_stop) {
            wheel_leg_control_enable(false);
            wheel_leg_control_set_command(NULL);
            remote_enabled = false;
        } else if (remote.enable) {
            if (!remote_enabled && BRINGUP_ENABLE_BALANCE_CONTROL) {
                wheel_leg_control_enable(true);
                remote_enabled = true;
            }
            wheel_leg_command_t command = {
                .target_vx = remote.vx,
                .target_yaw_rate = remote.yaw_rate,
                .target_leg_length = remote.leg_length,
            };
            wheel_leg_control_set_command(&command);
        } else {
            wheel_leg_control_set_command(NULL);
        }
    }

    // Update feedback first, then compute commands from the freshest motor state.
    system_update();

    // Execute demo or user control
    if (BRINGUP_MOTOR_TEST_ENABLE) {
        bringup_motor_test_step();
    } else if (demo_mode_enabled) {
        demo_control_step();
    } else if (BRINGUP_ENABLE_BALANCE_CONTROL) {
        wheel_leg_feedback_t feedback = {0};
        wheel_leg_control_update(&feedback, NULL);
    }
    
    // Get and display motor status periodically
    if (loop_counter % 100 == 0) {
        motor_t motor;
        for (int i = 1; i <= 4; i++) {
            if (motor_get_status(i, &motor) == true) {
                // Status available for logging/display
                // printf("Motor %d: Pos=%.2f, Vel=%.2f, Torque=%.2f\n",
                //        i, motor.position, motor.velocity, motor.torque);
            }
        }
    }
    
    loop_counter++;
}

/**
 * @brief Application entry point
 */
int main(void) {
    // Initialize system clocks and peripherals
    SystemInit();
    
    // Initialize system components
    if (!system_init()) {
        // Initialization failed
        HardFault_Handler();
    }
    
    demo_mode_enabled = false;
    remote_enabled = false;
    
    uint32_t last_time = 0;
    
    // Main control loop
    while (1) {
        uint32_t current_time = HAL_GetTick();  // milliseconds
        
        // Execute control loop at fixed frequency
        if (current_time - last_time >= CONTROL_PERIOD_MS) {
            control_loop();
            last_time = current_time;
        }
        
        // Low priority tasks (e.g., communication processing)
        // TODO: Handle command input
        // TODO: Process data logging
        // TODO: Handle error recovery
    }
    
    return 0;
}

/**
 * @brief System Tick Handler (called from SysTick interrupt)
 */
__attribute__((weak))
void SysTick_Handler(void) {
#if MAIN_USE_HAL
    HAL_IncTick();
#endif
}

/**
 * @brief Hard Fault Handler
 */
__attribute__((weak))
void HardFault_Handler(void) {
    // Emergency stop all motors
    system_emergency_stop();
    
    // Enter infinite loop for debugging
    while (1) {
        // Halt - attach debugger to continue
    }
}

/**
 * @brief NMI Handler
 */
__attribute__((weak))
void NMI_Handler(void) {
}

/**
 * @brief Usage Fault Handler
 */
__attribute__((weak))
void UsageFault_Handler(void) {
}

/**
 * @brief Bus Fault Handler
 */
__attribute__((weak))
void BusFault_Handler(void) {
    system_emergency_stop();
    while (1);
}

/**
 * @brief Memory Manage Handler
 */
__attribute__((weak))
void MemManage_Handler(void) {
    system_emergency_stop();
    while (1);
}

/**
 * @brief Debug Monitor Handler
 */
__attribute__((weak))
void DebugMon_Handler(void) {
}

/**
 * @brief PendSV Handler
 */
__attribute__((weak))
void PendSV_Handler(void) {
}
