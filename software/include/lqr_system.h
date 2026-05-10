/**
 * @file lqr_system.h
 * @brief LQR controller system integration
 * @author Team
 * @date 2026-05-09
 */

#ifndef LQR_SYSTEM_H
#define LQR_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lqr_controller.h"
#include "flight_controller.h"
#include <stdbool.h>

/* Control mode enumeration */
typedef enum {
    CONTROL_MODE_IDLE = 0,
    CONTROL_MODE_LQR_REGULATION,    /* Regulate to equilibrium */
    CONTROL_MODE_LQR_TRACKING,      /* Track reference trajectory */
    CONTROL_MODE_FLIGHT_HOLD,       /* Flight phase control */
    CONTROL_MODE_JUMP_SEQUENCE      /* Full jump sequence */
} control_mode_t;

/* System state */
typedef struct {
    lqr_controller_t lqr;
    flight_joint_controller_t flight;
    control_mode_t mode;
    float sequence_time;            /* Time in current sequence (s) */
    float sequence_phase;           /* Phase progress [0, 1] */
} lqr_system_t;

/* Function Declarations */

/**
 * @brief Initialize LQR system
 * @param sys Pointer to system
 * @param leg_length Leg length (m)
 * @return true if successful
 */
bool lqr_system_init(lqr_system_t *sys, float leg_length);

/**
 * @brief Set control mode
 * @param sys Pointer to system
 * @param mode New control mode
 * @return true if successful
 */
bool lqr_system_set_mode(lqr_system_t *sys, control_mode_t mode);

/**
 * @brief Update system (main control loop)
 * @param sys Pointer to system
 * @param state Current system state
 * @return true if successful
 */
bool lqr_system_update(lqr_system_t *sys, const lqr_state_t *state);

/**
 * @brief Get current control output
 * @param sys Pointer to system
 * @param T_body Output body torque
 * @param T_leg Output leg torque
 */
void lqr_system_get_output(lqr_system_t *sys, float *T_body, float *T_leg);

/**
 * @brief Get system status
 * @param sys Pointer to system
 * @return Current control mode
 */
control_mode_t lqr_system_get_mode(const lqr_system_t *sys);

/**
 * @brief Execute jump sequence (LQR + Flight control)
 * @param sys Pointer to system
 * @param state Current state
 * @param phase Current flight phase
 * @return true if successful
 */
bool lqr_system_execute_jump(
    lqr_system_t *sys,
    const lqr_state_t *state,
    flight_phase_t phase
);

/**
 * @brief Reset system
 * @param sys Pointer to system
 */
void lqr_system_reset(lqr_system_t *sys);

#ifdef __cplusplus
}
#endif

#endif /* LQR_SYSTEM_H */
