/**
 * @file lqr_controller.c
 * @brief LQR controller implementation
 * @author Team
 * @date 2026-05-09
 */

#include "lqr_controller.h"
#include "lqr_gains.h"
#include <string.h>
#include <math.h>

/**
 * @brief Initialize LQR controller
 */
bool lqr_controller_init(lqr_controller_t *controller, float leg_length) {
    if (controller == NULL) {
        return false;
    }
    
    memset(controller, 0, sizeof(lqr_controller_t));
    
    /* Set default configuration */
    controller->config.leg_length = leg_length;
    controller->config.sampling_period = 0.005f;  /* 5ms sampling */
    controller->config.enable_saturation = true;
    controller->config.torque_limit = 10.0f;      /* 10 Nm limit */
    
    /* Set Q matrix weights (from MATLAB sys_calc.m) */
    controller->config.q_weights[0] = 1.0f;       /* theta */
    controller->config.q_weights[1] = 10.0f;      /* dtheta */
    controller->config.q_weights[2] = 100.0f;     /* x */
    controller->config.q_weights[3] = 20.0f;      /* dx */
    controller->config.q_weights[4] = 1000.0f;    /* phi */
    controller->config.q_weights[5] = 1.0f;       /* dphi */
    
    /* Set R matrix weights */
    controller->config.r_weights[0] = 1.0f;       /* T (body torque) */
    controller->config.r_weights[1] = 1.0f;       /* Tp (leg torque) */
    
    /* Calculate initial gains */
    lqr_gains_compute(leg_length, &controller->gain_matrix[0][0]);
    
    return true;
}

/**
 * @brief Update controller configuration
 */
bool lqr_controller_set_config(lqr_controller_t *controller, const lqr_config_t *config) {
    if (controller == NULL || config == NULL) {
        return false;
    }
    
    memcpy(&controller->config, config, sizeof(lqr_config_t));
    
    /* Recalculate gains with new leg length if changed */
    lqr_gains_compute(config->leg_length, &controller->gain_matrix[0][0]);
    
    return true;
}

/**
 * @brief Set system state
 */
void lqr_controller_set_state(lqr_controller_t *controller, const lqr_state_t *state) {
    if (controller != NULL && state != NULL) {
        memcpy(&controller->state, state, sizeof(lqr_state_t));
    }
}

/**
 * @brief Calculate LQR feedback gains K for given leg length
 */
bool lqr_calculate_gains(float L0, float K[2][6]) {
    if (K == NULL) {
        return false;
    }
    
    lqr_gains_compute(L0, &K[0][0]);
    return true;
}

/**
 * @brief Compute control input using LQR
 * 
 * Control law: u = -K * x
 * where x = [theta, dtheta, x, dx, phi, dphi]^T
 * and u = [T, Tp]^T (body torque and leg torque)
 */
bool lqr_compute_control(lqr_controller_t *controller) {
    if (controller == NULL) {
        return false;
    }
    
    /* State vector: [theta, dtheta, x, dx, phi, dphi] */
    float x[6] = {
        controller->state.theta,
        controller->state.dtheta,
        controller->state.x,
        controller->state.dx,
        controller->state.phi,
        controller->state.dphi
    };
    
    /* Compute control: u = -K * x */
    for (int i = 0; i < 2; i++) {
        float u = 0.0f;
        for (int j = 0; j < 6; j++) {
            u += controller->gain_matrix[i][j] * x[j];
        }
        controller->torque_cmd[i] = -u;  /* Negative feedback */
    }
    
    /* Apply saturation if enabled */
    if (controller->config.enable_saturation) {
        for (int i = 0; i < 2; i++) {
            if (controller->torque_cmd[i] > controller->config.torque_limit) {
                controller->torque_cmd[i] = controller->config.torque_limit;
            }
            if (controller->torque_cmd[i] < -controller->config.torque_limit) {
                controller->torque_cmd[i] = -controller->config.torque_limit;
            }
        }
    }
    
    return true;
}

/**
 * @brief Get computed control torques
 */
void lqr_get_control(lqr_controller_t *controller, float torque[2]) {
    if (controller != NULL && torque != NULL) {
        torque[0] = controller->torque_cmd[0];
        torque[1] = controller->torque_cmd[1];
    }
}

/**
 * @brief Get error norm (state magnitude)
 */
float lqr_get_error_norm(const lqr_controller_t *controller) {
    if (controller == NULL) {
        return 0.0f;
    }
    
    float norm = 0.0f;
    const lqr_state_t *state = &controller->state;
    
    /* Weighted error norm */
    const float *q = controller->config.q_weights;
    
    norm += q[0] * state->theta * state->theta;
    norm += q[1] * state->dtheta * state->dtheta;
    norm += q[2] * state->x * state->x;
    norm += q[3] * state->dx * state->dx;
    norm += q[4] * state->phi * state->phi;
    norm += q[5] * state->dphi * state->dphi;
    
    return sqrtf(norm);
}

/**
 * @brief Reset controller state
 */
void lqr_controller_reset(lqr_controller_t *controller) {
    if (controller != NULL) {
        memset(&controller->state, 0, sizeof(lqr_state_t));
        memset(&controller->torque_cmd, 0, sizeof(controller->torque_cmd));
    }
}
