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
#include "hardware_config.h"
#include "uart_interface.h"
#include "wheel_leg_control.h"
#include "balance_feedback.h"
#include "attitude_imu.h"

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
static bool remote_wheel_test_active = false;

static float clamp_bringup_torque(float torque) {
    if (torque > BRINGUP_TEST_TORQUE_LIMIT_NM) {
        return BRINGUP_TEST_TORQUE_LIMIT_NM;
    }
    if (torque < -BRINGUP_TEST_TORQUE_LIMIT_NM) {
        return -BRINGUP_TEST_TORQUE_LIMIT_NM;
    }
    return torque;
}

static void bringup_can_scope_test_step(void) {
#if BRINGUP_CAN_SCOPE_TEST_ENABLE
    static uint32_t last_send_ms = 0;
    static uint8_t counter = 0;
    uint32_t now = HAL_GetTick();

    if (now != last_send_ms && now - last_send_ms < BRINGUP_CAN_SCOPE_PERIOD_MS) {
        return;
    }
    last_send_ms = now;

    can_message_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.can_id = BRINGUP_CAN_SCOPE_TEST_ID;
    msg.dlc = 8U;
    msg.data[0] = counter++;
    msg.data[1] = 0x11U;
    msg.data[2] = 0x22U;
    msg.data[3] = 0x33U;
    msg.data[4] = 0x44U;
    msg.data[5] = 0x55U;
    msg.data[6] = 0x66U;
    msg.data[7] = 0x77U;
    (void)can_send(&msg);
#endif
}

static void bringup_motor_test_step(void) {
#if BRINGUP_MOTOR_TEST_ENABLE
    static uint32_t first_run_ms = 0;
    static uint32_t last_enable_try_ms = 0;
    static uint32_t last_command_ms = 0;
    static uint32_t last_non_test_disable_ms = 0;
    static uint8_t next_non_test_disable_id = 1U;
    static uint32_t last_toggle_ms = 0;
    static float torque = BRINGUP_TEST_TORQUE_NM;
    static float speed = BRINGUP_TEST_SPEED_RAD_S;
    static float position_delta = BRINGUP_TEST_POSITION_DELTA_RAD;
    uint32_t now = HAL_GetTick();

    if (first_run_ms == 0U) {
        first_run_ms = now;
    }

#if BRINGUP_DISABLE_NON_TEST_MOTORS
    if (now - last_non_test_disable_ms >= BRINGUP_NON_TEST_DISABLE_PERIOD_MS) {
        if (next_non_test_disable_id == BRINGUP_TEST_MOTOR_ID) {
            next_non_test_disable_id++;
        }
        if (next_non_test_disable_id > TOTAL_MOTORS) {
            next_non_test_disable_id = 1U;
            if (next_non_test_disable_id == BRINGUP_TEST_MOTOR_ID) {
                next_non_test_disable_id++;
            }
        }
        (void)motor_disable(next_non_test_disable_id);
        next_non_test_disable_id++;
        last_non_test_disable_ms = now;
    }
#endif

    bool feedback_recent = motor_feedback_recent(BRINGUP_TEST_MOTOR_ID, now,
                                                 BRINGUP_MOTOR_FEEDBACK_TIMEOUT_MS);

    if (!feedback_recent && now - last_enable_try_ms >= BRINGUP_MOTOR_ENABLE_RETRY_MS) {
        (void)motor_enable(BRINGUP_TEST_MOTOR_ID);
        last_enable_try_ms = now;
    }

    if (feedback_recent && now - last_enable_try_ms >= 100U) {
        (void)motor_enable(BRINGUP_TEST_MOTOR_ID);
        last_enable_try_ms = now;
    }

    if (now - first_run_ms < BRINGUP_MOTOR_ENABLE_ONLY_MS) {
        return;
    }

    if (BRINGUP_REQUIRE_FEEDBACK_FOR_TORQUE && !feedback_recent) {
        return;
    }

    if (BRINGUP_TEST_ALTERNATE_DIRECTION &&
        now - last_toggle_ms >= BRINGUP_TEST_PERIOD_MS) {
        torque = -torque;
        speed = -speed;
        position_delta = -position_delta;
        last_toggle_ms = now;
    }

    if (now - last_command_ms >= 10U) {
#if BRINGUP_TEST_CONTROL_MODE == BRINGUP_TEST_MODE_SPEED
        (void)motor_set_speed_mode(BRINGUP_TEST_MOTOR_ID, speed);
#elif BRINGUP_TEST_CONTROL_MODE == BRINGUP_TEST_MODE_PV_POSITION
        motor_t motor;
        if (motor_get_status(BRINGUP_TEST_MOTOR_ID, &motor)) {
            (void)motor_set_position_velocity_mode(BRINGUP_TEST_MOTOR_ID,
                                                   motor.position + position_delta,
                                                   BRINGUP_TEST_SPEED_RAD_S);
        }
#else
        (void)motor_set_torque(BRINGUP_TEST_MOTOR_ID, clamp_bringup_torque(torque));
#endif
        last_command_ms = now;
    }
#endif
}

static void disable_all_motors_periodic(void) {
    static uint32_t last_disable_ms = 0;
    static uint8_t next_disable_id = 1U;
    uint32_t now = HAL_GetTick();

    if (now - last_disable_ms < BRINGUP_IDLE_DISABLE_PERIOD_MS) {
        return;
    }

    (void)motor_disable(next_disable_id);
    next_disable_id++;
    if (next_disable_id > TOTAL_MOTORS) {
        next_disable_id = 1U;
    }
    last_disable_ms = now;
}

static float clamp_remote_wheel_torque(float torque) {
    if (torque > BRINGUP_REMOTE_WHEEL_TEST_TORQUE_LIMIT_NM) {
        return BRINGUP_REMOTE_WHEEL_TEST_TORQUE_LIMIT_NM;
    }
    if (torque < -BRINGUP_REMOTE_WHEEL_TEST_TORQUE_LIMIT_NM) {
        return -BRINGUP_REMOTE_WHEEL_TEST_TORQUE_LIMIT_NM;
    }
    return torque;
}

static void remote_wheel_test_update(const dbus_motion_cmd_t *remote) {
#if BRINGUP_REMOTE_WHEEL_TEST_ENABLE
    static bool wheels_enabled = false;

    if (remote == NULL || remote->emergency_stop || !remote->enable) {
        (void)motor_disable(LEFT_WHEEL_MOTOR_ID);
        (void)motor_disable(RIGHT_WHEEL_MOTOR_ID);
        wheels_enabled = false;
        remote_wheel_test_active = false;
        return;
    }

    remote_wheel_test_active = true;

    if (!wheels_enabled) {
        (void)motor_enable(LEFT_WHEEL_MOTOR_ID);
        (void)motor_enable(RIGHT_WHEEL_MOTOR_ID);
        wheels_enabled = true;
    }

    float forward = clamp_remote_wheel_torque(remote->vx * BRINGUP_REMOTE_WHEEL_TEST_TORQUE_LIMIT_NM);
    float yaw = clamp_remote_wheel_torque(-remote->yaw_rate * 0.5f * BRINGUP_REMOTE_WHEEL_TEST_TORQUE_LIMIT_NM);

    (void)leg_set_wheel_torque(LEG_LEFT, forward - yaw);
    (void)leg_set_wheel_torque(LEG_RIGHT, forward + yaw);
#else
    (void)remote;
#endif
}

/* Forward declarations */
extern void SystemInit(void);
extern uint32_t HAL_GetTick(void);
void HardFault_Handler(void);
#if MAIN_USE_HAL
void SystemClock_Config(void);
void Error_Handler(void);
#endif

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
#if !BRINGUP_ENABLE_BALANCE_CONTROL
            remote_wheel_test_update(&remote);
#endif
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
#if !BRINGUP_ENABLE_BALANCE_CONTROL
            remote_wheel_test_update(&remote);
#endif
        } else {
            wheel_leg_control_set_command(NULL);
#if !BRINGUP_ENABLE_BALANCE_CONTROL
            remote_wheel_test_update(&remote);
#endif
        }
    }

    // Update feedback first, then compute commands from the freshest motor state.
    system_update();

    // Execute demo or user control
#if BRINGUP_CAN_SCOPE_TEST_ENABLE
    bringup_can_scope_test_step();
#endif

    if (!BRINGUP_CAN_SCOPE_TEST_ENABLE && BRINGUP_MOTOR_TEST_ENABLE) {
        bringup_motor_test_step();
    } else if (demo_mode_enabled) {
        demo_control_step();
    } else if (BRINGUP_ENABLE_BALANCE_CONTROL) {
        wheel_leg_feedback_t feedback;
#if BRINGUP_ENABLE_IMU_FEEDBACK
        (void)attitude_imu_update(HAL_GetTick());
#endif
        balance_feedback_update(HAL_GetTick());
        if (balance_feedback_get(&feedback)) {
            wheel_leg_control_update(&feedback, NULL);
        } else {
            wheel_leg_control_enable(false);
            remote_enabled = false;
        }
    } else {
        if (!remote_wheel_test_active) {
            disable_all_motors_periodic();
        }
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
#if MAIN_USE_HAL
    HAL_Init();
    SystemClock_Config();
#else
    SystemInit();
#endif
    
    // Initialize system components
    if (!system_init()) {
        // Initialization failed
        HardFault_Handler();
    }
    
    demo_mode_enabled = false;
    remote_enabled = false;

#if BRINGUP_BOOT_DISABLE_ALL_MOTORS
    for (uint8_t id = 1U; id <= TOTAL_MOTORS; ++id) {
        (void)motor_disable(id);
        HAL_Delay(2U);
    }
#endif
    
    uint32_t last_time = 0;
    
    // Main control loop
    while (1) {
        uint32_t current_time = HAL_GetTick();  // milliseconds

#if BRINGUP_CAN_SCOPE_TEST_ENABLE
        bringup_can_scope_test_step();
#endif
        
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

#if MAIN_USE_HAL
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 6;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        HAL_IncTick();
    }
}
#endif

