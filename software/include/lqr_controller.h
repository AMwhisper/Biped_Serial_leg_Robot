/**
 * @file lqr_controller.h
 * @brief Linear Quadratic Regulator (LQR) controller for quadruped robot
 * @author Team
 * @date 2026-05-09
 * 
 * Based on sys_calc.m and the single-side equivalent-pendulum model:
 * - State: [theta, dtheta, x, dx, phi, dphi] (6D)
 * - phi and dphi come from the VMC equivalent pendulum polar angle and speed
 * - Control: [T, Tp] (2D)
 * - T is the wheel/body drive output used by the wheel motor
 * - Tp is the virtual pendulum/leg angular torque passed to VMC
 * - Sampling period: 5ms
 * - Weight matrices: Q = diag([1, 10, 100, 20, 1000, 1]), R = diag([1, 1])
 */

#ifndef LQR_CONTROLLER_H
#define LQR_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* System state structure */
typedef struct {
    float theta;      // Body pitch angle (rad)
    float dtheta;     // Body angular velocity (rad/s)
    float x;          // Horizontal position (m)
    float dx;         // Horizontal velocity (m/s)
    float phi;        // Equivalent pendulum polar angle from VMC pos[1] (rad)
    float dphi;       // Equivalent pendulum polar angular speed from VMC spd[1] (rad/s)
} lqr_state_t;

/* LQR controller configuration */
typedef struct {
    float leg_length;         // L0: Full leg length (m)
    float sampling_period;    // Ts: Sampling period (s)
    float q_weights[6];       // Q matrix diagonal weights
    float r_weights[2];       // R matrix diagonal weights
    bool enable_saturation;   // Enable control saturation
    float torque_limit;       // Max torque saturation (Nm)
} lqr_config_t;

/* LQR controller state */
typedef struct {
    lqr_state_t state;
    float torque_cmd[2];      // Control output [T, Tp]
    float gain_matrix[2][6];  // K: Feedback gain matrix (2x6)
    lqr_config_t config;
} lqr_controller_t;

/* Function Declarations */

/**
 * @brief Initialize LQR controller
 * @param controller Pointer to controller structure
 * @param leg_length Leg length L0 (m)
 * @return true if successful
 */
bool lqr_controller_init(lqr_controller_t *controller, float leg_length);

/**
 * @brief Update controller configuration
 * @param controller Pointer to controller
 * @param config New configuration
 * @return true if successful
 */
bool lqr_controller_set_config(lqr_controller_t *controller, const lqr_config_t *config);

/**
 * @brief Set system state
 * @param controller Pointer to controller
 * @param state New state
 */
void lqr_controller_set_state(lqr_controller_t *controller, const lqr_state_t *state);

/**
 * @brief Calculate LQR feedback gains K for given leg length L0
 * @param L0 Leg length (m)
 * @param K Output gain matrix (2x6)
 * @return true if successful
 */
bool lqr_calculate_gains(float L0, float K[2][6]);

/**
 * @brief Compute control input using LQR
 * @param controller Pointer to controller
 * @return true if successful
 */
bool lqr_compute_control(lqr_controller_t *controller);

/**
 * @brief Get computed control torques
 * @param controller Pointer to controller
 * @param torque Output torques [T, Tp]
 */
void lqr_get_control(lqr_controller_t *controller, float torque[2]);

/**
 * @brief Get reference trajectory (for regulation to origin)
 * @param controller Pointer to controller
 * @return Tracking error norm
 */
float lqr_get_error_norm(const lqr_controller_t *controller);

/**
 * @brief Reset controller state
 * @param controller Pointer to controller
 */
void lqr_controller_reset(lqr_controller_t *controller);

#ifdef __cplusplus
}
#endif

#endif /* LQR_CONTROLLER_H */
