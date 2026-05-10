/**
 * @file flight_controller.h
 * @brief Flight phase controller for jumping motion
 * @author Team
 * @date 2026-05-09
 * 
 * Based on MATLAB flight_joint_hold.m for maintaining leg assembly during flight
 */

#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Flight phase enumeration */
typedef enum {
    FLIGHT_PHASE_STANCE = 0,    /* Leg on ground */
    FLIGHT_PHASE_CROUCH = 1,    /* Preparation phase */
    FLIGHT_PHASE_THRUST = 2,    /* Thrust generation */
    FLIGHT_PHASE_FLIGHT = 3,    /* Airborne */
    FLIGHT_PHASE_LANDING = 4    /* Landing preparation */
} flight_phase_t;

/* Joint hold parameters */
typedef struct {
    float kp;                   /* Position gain */
    float kd;                   /* Velocity gain */
    float torque_limit;         /* Maximum torque (Nm) */
} flight_params_t;

/* Joint controller state */
typedef struct {
    flight_phase_t phase;
    float phi1_lock;            /* Locked hip angle */
    float phi4_lock;            /* Locked knee angle */
    float phi1;                 /* Current hip angle */
    float phi4;                 /* Current knee angle */
    float dphi1;                /* Hip angular velocity */
    float dphi4;                /* Knee angular velocity */
    float T1;                   /* Hip torque command */
    float T2;                   /* Knee torque command */
    bool in_flight;
    flight_params_t params;
} flight_joint_controller_t;

/* Function Declarations */

/**
 * @brief Initialize flight controller
 * @return true if successful
 */
bool flight_controller_init(flight_joint_controller_t *controller);

/**
 * @brief Update flight phase and compute joint torques
 * @param controller Pointer to controller
 * @param phase Current flight phase
 * @param phi1 Hip angle (rad)
 * @param phi4 Knee angle (rad)
 * @param dphi1 Hip angular velocity (rad/s)
 * @param dphi4 Knee angular velocity (rad/s)
 * @return true if successful
 */
bool flight_controller_update(
    flight_joint_controller_t *controller,
    flight_phase_t phase,
    float phi1,
    float phi4,
    float dphi1,
    float dphi4
);

/**
 * @brief Get computed joint torques
 * @param controller Pointer to controller
 * @param T1 Output hip torque
 * @param T2 Output knee torque
 */
void flight_controller_get_torques(
    flight_joint_controller_t *controller,
    float *T1,
    float *T2
);

/**
 * @brief Check if robot is in flight
 * @param controller Pointer to controller
 * @return true if in flight
 */
bool flight_controller_is_flying(const flight_joint_controller_t *controller);

/**
 * @brief Set controller parameters
 * @param controller Pointer to controller
 * @param params New parameters
 */
void flight_controller_set_params(
    flight_joint_controller_t *controller,
    const flight_params_t *params
);

#ifdef __cplusplus
}
#endif

#endif /* FLIGHT_CONTROLLER_H */
