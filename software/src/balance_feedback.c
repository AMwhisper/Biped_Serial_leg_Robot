/**
 * @file balance_feedback.c
 * @brief Combines attitude and wheel odometry for the LQR balance loop.
 */

#include "balance_feedback.h"
#include "hardware_config.h"
#include "motor_controller.h"
#include <string.h>

static wheel_leg_feedback_t feedback;
static balance_attitude_t attitude;
static uint32_t last_update_ms;
static bool initialized;

static float average_wheel_velocity(void) {
    motor_t left;
    motor_t right;

    if (!motor_get_status(LEFT_WHEEL_MOTOR_ID, &left) ||
        !motor_get_status(RIGHT_WHEEL_MOTOR_ID, &right)) {
        return 0.0f;
    }

    return 0.5f * ((left.velocity * LEFT_WHEEL_TORQUE_SIGN) +
                   (right.velocity * RIGHT_WHEEL_TORQUE_SIGN)) * WHEEL_RADIUS;
}

void balance_feedback_init(void) {
    memset(&feedback, 0, sizeof(feedback));
    memset(&attitude, 0, sizeof(attitude));
    last_update_ms = 0U;
    initialized = true;
}

void balance_feedback_set_attitude(float theta_rad, float dtheta_rad_s, bool valid) {
    attitude.theta = theta_rad;
    attitude.dtheta = dtheta_rad_s;
    attitude.attitude_valid = valid;
}

bool balance_feedback_update(uint32_t now_ms) {
    if (!initialized) {
        return false;
    }

    float dt = 0.0f;
    if (last_update_ms != 0U && now_ms >= last_update_ms) {
        dt = (float)(now_ms - last_update_ms) * 0.001f;
    }
    last_update_ms = now_ms;

    feedback.dx = average_wheel_velocity();
    if (dt > 0.0f && dt < 0.1f) {
        feedback.x += feedback.dx * dt;
    }

    if (attitude.attitude_valid) {
        feedback.theta = attitude.theta;
        feedback.dtheta = attitude.dtheta;
    } else {
        feedback.theta = 0.0f;
        feedback.dtheta = 0.0f;
    }

    return attitude.attitude_valid;
}

bool balance_feedback_get(wheel_leg_feedback_t *out) {
    if (out == 0 || !initialized) {
        return false;
    }

    memcpy(out, &feedback, sizeof(*out));
    return attitude.attitude_valid;
}
