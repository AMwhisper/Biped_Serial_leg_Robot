/**
 * @file motor_controller.h
 * @brief Motor controller interface for Damiaotech motors
 * @author Team
 * @date 2026-05-09
 */

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Motor control data structure */
typedef struct {
    uint8_t motor_id;
    uint8_t motor_type;      // 0x01: DM6215, 0x02: DM4310
    float position;           // Current position in radians
    float velocity;           // Current velocity in rad/s
    float torque;            // Current torque in Nm
    float target_torque;     // Target torque
    float target_velocity;   // Target velocity
    float target_position;   // Target position
    uint16_t temperature;    // Motor temperature
    bool enabled;            // Motor enabled flag
} motor_t;

/* Function declarations */

/**
 * @brief Initialize motor controller
 * @return true if successful, false otherwise
 */
bool motor_controller_init(void);

/**
 * @brief Enable a motor
 * @param motor_id Motor ID
 * @return true if successful
 */
bool motor_enable(uint8_t motor_id);

/**
 * @brief Disable a motor
 * @param motor_id Motor ID
 * @return true if successful
 */
bool motor_disable(uint8_t motor_id);

/**
 * @brief Set motor torque command
 * @param motor_id Motor ID
 * @param torque Desired torque in Nm
 * @return true if successful
 */
bool motor_set_torque(uint8_t motor_id, float torque);

/**
 * @brief Set motor speed command
 * @param motor_id Motor ID
 * @param speed Desired speed in rad/s
 * @return true if successful
 */
bool motor_set_speed(uint8_t motor_id, float speed);

/**
 * @brief Set motor position command
 * @param motor_id Motor ID
 * @param position Desired position in radians
 * @return true if successful
 */
bool motor_set_position(uint8_t motor_id, float position);

/**
 * @brief Get motor status
 * @param motor_id Motor ID
 * @param motor Pointer to motor_t structure to fill
 * @return true if successful
 */
bool motor_get_status(uint8_t motor_id, motor_t *motor);

/**
 * @brief Update all motors (main loop call)
 * @return true if successful
 */
bool motor_update(void);

/**
 * @brief Get motor pointer by ID
 * @param motor_id Motor ID
 * @return Pointer to motor structure or NULL
 */
motor_t* motor_get(uint8_t motor_id);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_CONTROLLER_H */
