/**
 * @file wheel_leg_control.c
 * @brief High-level balance controller for the serial wheel-legged robot.
 */

#include "wheel_leg_control.h"
#include "hardware_config.h"
#include "leg_controller.h"
#include "lqr_controller.h"
#include "motor_config.h"
#include "motor_controller.h"
#include <math.h>
#include <string.h>

#define NOMINAL_LEG_LENGTH_M 0.11f
#define LEG_LENGTH_KP        180.0f
#define LEG_LENGTH_KD        6.0f
#define STANCE_FORCE_N       25.0f
#define YAW_TORQUE_GAIN      1.2f

static lqr_controller_t lqr;
static bool initialized;
static bool enabled;
static wheel_leg_command_t command = {
    .target_vx = 0.0f,
    .target_yaw_rate = 0.0f,
    .target_leg_length = NOMINAL_LEG_LENGTH_M,
};

static float clampf(float value, float lo, float hi) {
    if (value < lo) {
        return lo;
    }
    if (value > hi) {
        return hi;
    }
    return value;
}

bool wheel_leg_control_init(void) {
    if (!lqr_controller_init(&lqr, NOMINAL_LEG_LENGTH_M)) {
        return false;
    }

    lqr.config.torque_limit = DM6215_MAX_TORQUE;
    initialized = true;
    enabled = false;
    return true;
}

bool wheel_leg_control_enable(bool enable) {
    if (!initialized) {
        return false;
    }

    for (uint8_t id = 1; id <= TOTAL_MOTORS; ++id) {
        if (enable) {
            if (!motor_enable(id)) {
                return false;
            }
        } else {
            motor_disable(id);
        }
    }

    enabled = enable;
    return true;
}

void wheel_leg_control_set_command(const wheel_leg_command_t *cmd) {
    if (cmd == NULL) {
        command.target_vx = 0.0f;
        command.target_yaw_rate = 0.0f;
        command.target_leg_length = NOMINAL_LEG_LENGTH_M;
        return;
    }

    command.target_vx = clampf(cmd->target_vx, -MAX_LINEAR_VELOCITY, MAX_LINEAR_VELOCITY);
    command.target_yaw_rate = clampf(cmd->target_yaw_rate, -MAX_ANGULAR_VELOCITY, MAX_ANGULAR_VELOCITY);
    command.target_leg_length = clampf(cmd->target_leg_length, 0.07f, 0.14f);
}

bool wheel_leg_control_update(const wheel_leg_feedback_t *feedback, wheel_leg_output_t *output) {
    if (!initialized || feedback == NULL) {
        return false;
    }

    leg_state_t left;
    leg_state_t right;
    if (!leg_get_state(LEG_LEFT, &left) || !leg_get_state(LEG_RIGHT, &right)) {
        return false;
    }

    float leg_length = 0.5f * (left.leg_length + right.leg_length);
    float leg_length_rate = 0.5f * (left.leg_length_rate + right.leg_length_rate);
    float leg_angle = 0.5f * (left.leg_angle + right.leg_angle);
    float leg_angle_rate = 0.5f * (left.leg_angle_rate + right.leg_angle_rate);

    lqr_state_t state = {
        .theta = feedback->theta,
        .dtheta = feedback->dtheta,
        .x = feedback->x,
        .dx = feedback->dx - command.target_vx,
        .phi = leg_angle,
        .dphi = leg_angle_rate,
    };

    lqr.config.leg_length = clampf(leg_length, 0.04f, 0.14f);
    lqr_controller_set_config(&lqr, &lqr.config);
    lqr_controller_set_state(&lqr, &state);
    lqr_compute_control(&lqr);

    float u[2] = {0.0f, 0.0f};
    lqr_get_control(&lqr, u);

    float hold_force = STANCE_FORCE_N +
                       LEG_LENGTH_KP * (command.target_leg_length - leg_length) -
                       LEG_LENGTH_KD * leg_length_rate;
    hold_force = clampf(hold_force, -80.0f, 80.0f);

    wheel_leg_output_t local = {
        .wheel_torque_left = u[0] - command.target_yaw_rate * YAW_TORQUE_GAIN,
        .wheel_torque_right = u[0] + command.target_yaw_rate * YAW_TORQUE_GAIN,
        .leg_force_left = hold_force,
        .leg_force_right = hold_force,
        .leg_torque_left = u[1],
        .leg_torque_right = u[1],
    };

    if (enabled) {
        leg_set_wheel_torque(LEG_LEFT, local.wheel_torque_left);
        leg_set_wheel_torque(LEG_RIGHT, local.wheel_torque_right);
        leg_set_virtual_force(LEG_LEFT, local.leg_force_left, local.leg_torque_left);
        leg_set_virtual_force(LEG_RIGHT, local.leg_force_right, local.leg_torque_right);
    }

    if (output != NULL) {
        memcpy(output, &local, sizeof(local));
    }

    return true;
}
