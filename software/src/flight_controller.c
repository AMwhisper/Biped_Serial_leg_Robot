/**
 * @file flight_controller.c
 * @brief Flight phase controller implementation
 * @author Team
 * @date 2026-05-09
 */

#include "flight_controller.h"
#include <string.h>
#include <math.h>

/**
 * @brief Initialize flight controller
 */
bool flight_controller_init(flight_joint_controller_t *controller) {
    if (controller == NULL) {
        return false;
    }
    
    memset(controller, 0, sizeof(flight_joint_controller_t));
    
    /* Set default parameters (from MATLAB flight_hold_params.m) */
    controller->params.kp = 0.6f;           /* Position gain */
    controller->params.kd = 0.03f;          /* Velocity gain */
    controller->params.torque_limit = 0.08f; /* 0.08 Nm limit */
    
    controller->phase = FLIGHT_PHASE_STANCE;
    controller->in_flight = false;
    
    return true;
}

/**
 * @brief Clamp value between min and max
 */
static float clamp_value(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

/**
 * @brief Update flight controller
 */
bool flight_controller_update(
    flight_joint_controller_t *controller,
    flight_phase_t phase,
    float phi1,
    float phi4,
    float dphi1,
    float dphi4
) {
    if (controller == NULL) {
        return false;
    }
    
    controller->phase = phase;
    controller->phi1 = phi1;
    controller->phi4 = phi4;
    controller->dphi1 = dphi1;
    controller->dphi4 = dphi4;
    
    /* Lock angles at the beginning of thrust phase */
    if (phase == FLIGHT_PHASE_THRUST) {
        controller->phi1_lock = phi1;
        controller->phi4_lock = phi4;
    }
    
    /* Update in-flight status */
    controller->in_flight = (phase == FLIGHT_PHASE_FLIGHT);
    
    /* Compute torques based on phase */
    if (controller->in_flight) {
        /* PD control during flight to maintain assembly */
        float error_phi1 = controller->phi1_lock - phi1;
        float error_phi4 = controller->phi4_lock - phi4;
        
        controller->T1 = controller->params.kp * error_phi1 
                       - controller->params.kd * dphi1;
        controller->T2 = controller->params.kp * error_phi4 
                       - controller->params.kd * dphi4;
    } else {
        /* No torque during stance */
        controller->T1 = 0.0f;
        controller->T2 = 0.0f;
    }
    
    /* Apply saturation */
    controller->T1 = clamp_value(controller->T1, 
                                 -controller->params.torque_limit,
                                 controller->params.torque_limit);
    controller->T2 = clamp_value(controller->T2,
                                 -controller->params.torque_limit,
                                 controller->params.torque_limit);
    
    return true;
}

/**
 * @brief Get computed joint torques
 */
void flight_controller_get_torques(
    flight_joint_controller_t *controller,
    float *T1,
    float *T2
) {
    if (controller != NULL) {
        if (T1 != NULL) *T1 = controller->T1;
        if (T2 != NULL) *T2 = controller->T2;
    }
}

/**
 * @brief Check if in flight
 */
bool flight_controller_is_flying(const flight_joint_controller_t *controller) {
    if (controller == NULL) {
        return false;
    }
    return controller->in_flight;
}

/**
 * @brief Set controller parameters
 */
void flight_controller_set_params(
    flight_joint_controller_t *controller,
    const flight_params_t *params
) {
    if (controller != NULL && params != NULL) {
        memcpy(&controller->params, params, sizeof(flight_params_t));
    }
}
