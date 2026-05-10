/**
 * @file lqr_system.c
 * @brief LQR system implementation
 * @author Team
 * @date 2026-05-09
 */

#include "lqr_system.h"
#include <string.h>

/**
 * @brief Initialize LQR system
 */
bool lqr_system_init(lqr_system_t *sys, float leg_length) {
    if (sys == NULL) {
        return false;
    }
    
    memset(sys, 0, sizeof(lqr_system_t));
    
    /* Initialize LQR controller */
    if (!lqr_controller_init(&sys->lqr, leg_length)) {
        return false;
    }
    
    /* Initialize flight controller */
    if (!flight_controller_init(&sys->flight)) {
        return false;
    }
    
    sys->mode = CONTROL_MODE_IDLE;
    sys->sequence_time = 0.0f;
    
    return true;
}

/**
 * @brief Set control mode
 */
bool lqr_system_set_mode(lqr_system_t *sys, control_mode_t mode) {
    if (sys == NULL) {
        return false;
    }
    
    sys->mode = mode;
    sys->sequence_time = 0.0f;
    
    return true;
}

/**
 * @brief Update system
 */
bool lqr_system_update(lqr_system_t *sys, const lqr_state_t *state) {
    if (sys == NULL || state == NULL) {
        return false;
    }
    
    switch (sys->mode) {
        case CONTROL_MODE_LQR_REGULATION:
        case CONTROL_MODE_LQR_TRACKING:
            /* Use LQR for body control */
            lqr_controller_set_state(&sys->lqr, state);
            lqr_compute_control(&sys->lqr);
            break;
            
        case CONTROL_MODE_FLIGHT_HOLD:
            /* Flight joint holding (update through execute_jump) */
            break;
            
        case CONTROL_MODE_JUMP_SEQUENCE:
            /* Full sequence handled in execute_jump */
            break;
            
        case CONTROL_MODE_IDLE:
        default:
            break;
    }
    
    return true;
}

/**
 * @brief Get control output
 */
void lqr_system_get_output(lqr_system_t *sys, float *T_body, float *T_leg) {
    if (sys == NULL) {
        return;
    }
    
    switch (sys->mode) {
        case CONTROL_MODE_LQR_REGULATION:
        case CONTROL_MODE_LQR_TRACKING:
            {
                float torque[2] = {0.0f, 0.0f};
                lqr_get_control(&sys->lqr, torque);
                if (T_body != NULL) *T_body = torque[0];
                if (T_leg != NULL) *T_leg = torque[1];
            }
            break;
            
        case CONTROL_MODE_FLIGHT_HOLD:
            flight_controller_get_torques(&sys->flight, T_body, T_leg);
            break;
            
        case CONTROL_MODE_IDLE:
        default:
            if (T_body != NULL) *T_body = 0.0f;
            if (T_leg != NULL) *T_leg = 0.0f;
            break;
    }
}

/**
 * @brief Get current mode
 */
control_mode_t lqr_system_get_mode(const lqr_system_t *sys) {
    if (sys == NULL) {
        return CONTROL_MODE_IDLE;
    }
    return sys->mode;
}

/**
 * @brief Execute jump sequence
 */
bool lqr_system_execute_jump(
    lqr_system_t *sys,
    const lqr_state_t *state,
    flight_phase_t phase
) {
    if (sys == NULL || state == NULL) {
        return false;
    }
    
    /* Set mode to jump sequence */
    sys->mode = CONTROL_MODE_JUMP_SEQUENCE;
    
    /* During stance phases (0,1,2), use LQR for thrust generation */
    if (phase <= FLIGHT_PHASE_THRUST) {
        lqr_controller_set_state(&sys->lqr, state);
        lqr_compute_control(&sys->lqr);
    }
    
    /* During flight (3), use flight joint holding */
    if (phase == FLIGHT_PHASE_FLIGHT) {
        sys->mode = CONTROL_MODE_FLIGHT_HOLD;
    }
    
    /* Update flight controller */
    flight_controller_update(
        &sys->flight,
        phase,
        state->phi,
        state->phi,  /* Use phi for both joints (simplified) */
        state->dphi,
        state->dphi
    );
    
    return true;
}

/**
 * @brief Reset system
 */
void lqr_system_reset(lqr_system_t *sys) {
    if (sys == NULL) {
        return;
    }
    
    lqr_controller_reset(&sys->lqr);
    sys->mode = CONTROL_MODE_IDLE;
    sys->sequence_time = 0.0f;
}
