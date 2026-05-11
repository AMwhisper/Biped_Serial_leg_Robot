/**
 * @file right_side_control.c
 * @brief Right-side reference control adapter.
 */

#include "right_side_control.h"
#include "hardware_config.h"
#include "motor_controller.h"
#include "vmc_math.h"
#include <math.h>
#include <string.h>

#define LEG_LENGTH_UPDATE_EPS 0.001f

bool right_side_control_init(right_side_control_t *control, float initial_leg_length) {
    if (control == NULL) {
        return false;
    }

    memset(control, 0, sizeof(*control));
    if (!lqr_controller_init(&control->lqr, initial_leg_length)) {
        return false;
    }

    control->initialized = true;
    return true;
}

bool right_side_control_enable_motors(void) {
    return motor_enable(RIGHT_PHI1_MOTOR_ID) &&
           motor_enable(RIGHT_PHI4_MOTOR_ID) &&
           motor_enable(RIGHT_WHEEL_MOTOR_ID);
}

bool right_side_control_disable_motors(void) {
    bool ok = true;
    ok = motor_disable(RIGHT_PHI1_MOTOR_ID) && ok;
    ok = motor_disable(RIGHT_PHI4_MOTOR_ID) && ok;
    ok = motor_disable(RIGHT_WHEEL_MOTOR_ID) && ok;
    return ok;
}

bool right_side_control_update(right_side_control_t *control,
                               const right_side_control_input_t *input,
                               right_side_control_debug_t *debug) {
    if (control == NULL || input == NULL || !control->initialized) {
        return false;
    }

    motor_t phi1_motor;
    motor_t phi4_motor;
    if (!motor_get_status(RIGHT_PHI1_MOTOR_ID, &phi1_motor) ||
        !motor_get_status(RIGHT_PHI4_MOTOR_ID, &phi4_motor)) {
        return false;
    }

    if (!phi1_motor.feedback_seen || !phi4_motor.feedback_seen) {
        return false;
    }

    const float phi1 = phi1_motor.position;
    const float phi4 = phi4_motor.position;
    const float dphi1 = phi1_motor.velocity;
    const float dphi4 = phi4_motor.velocity;

    float pos[2];
    float spd[2];
    vmc_leg_pos(phi1, phi4, pos);
    vmc_leg_spd(dphi1, dphi4, phi1, phi4, spd);

    if (fabsf(pos[0] - control->lqr.config.leg_length) > LEG_LENGTH_UPDATE_EPS) {
        lqr_config_t config = control->lqr.config;
        config.leg_length = pos[0];
        (void)lqr_controller_set_config(&control->lqr, &config);
    }

    lqr_state_t state = {
        .theta = input->theta,
        .dtheta = input->dtheta,
        .x = input->x,
        .dx = input->dx,
        .phi = pos[1],
        .dphi = spd[1],
    };

    lqr_controller_set_state(&control->lqr, &state);
    if (!lqr_compute_control(&control->lqr)) {
        return false;
    }

    float lqr_output[2];
    lqr_get_control(&control->lqr, lqr_output);

    float joint_torque[2];
    vmc_leg_conv(input->axial_force, lqr_output[1], phi1, phi4, joint_torque);

    /* Right-side reference mapping:
     *   LQR T      -> ID6 wheel torque
     *   torque[0] -> phi1 -> ID4
     *   torque[1] -> phi4 -> ID3
     */
    bool ok = true;
    ok = motor_set_torque(RIGHT_WHEEL_MOTOR_ID, lqr_output[0]) && ok;
    ok = motor_set_torque(RIGHT_PHI1_MOTOR_ID, joint_torque[0]) && ok;
    ok = motor_set_torque(RIGHT_PHI4_MOTOR_ID, joint_torque[1]) && ok;

    if (debug != NULL) {
        debug->phi1 = phi1;
        debug->phi4 = phi4;
        debug->dphi1 = dphi1;
        debug->dphi4 = dphi4;
        debug->pos[0] = pos[0];
        debug->pos[1] = pos[1];
        debug->spd[0] = spd[0];
        debug->spd[1] = spd[1];
        debug->lqr_output[0] = lqr_output[0];
        debug->lqr_output[1] = lqr_output[1];
        debug->joint_torque[0] = joint_torque[0];
        debug->joint_torque[1] = joint_torque[1];
    }

    return ok;
}
