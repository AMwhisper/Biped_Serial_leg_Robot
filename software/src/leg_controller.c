/**
 * @file leg_controller.c
 * @brief Two-leg VMC controller implementation.
 */

#include "leg_controller.h"
#include "hardware_config.h"
#include "motor_controller.h"
#include "vmc_math.h"
#include <math.h>
#include <string.h>

static leg_state_t legs[LEG_COUNT];

static const uint8_t joint_motors[LEG_COUNT][2] = {
    {LEFT_HIP_MOTOR_ID, LEFT_KNEE_MOTOR_ID},
    {RIGHT_HIP_MOTOR_ID, RIGHT_KNEE_MOTOR_ID},
};

static const float joint_model_signs[LEG_COUNT][2] = {
    {LEFT_HIP_MODEL_SIGN, LEFT_KNEE_MODEL_SIGN},
    {RIGHT_HIP_MODEL_SIGN, RIGHT_KNEE_MODEL_SIGN},
};

static const float joint_model_offsets[LEG_COUNT][2] = {
    {LEFT_HIP_MODEL_OFFSET, LEFT_KNEE_MODEL_OFFSET},
    {RIGHT_HIP_MODEL_OFFSET, RIGHT_KNEE_MODEL_OFFSET},
};

static const uint8_t wheel_motors[LEG_COUNT] = {
    LEFT_WHEEL_MOTOR_ID,
    RIGHT_WHEEL_MOTOR_ID,
};

static const float wheel_torque_signs[LEG_COUNT] = {
    LEFT_WHEEL_TORQUE_SIGN,
    RIGHT_WHEEL_TORQUE_SIGN,
};

static bool valid_leg(leg_id_t leg_id) {
    return leg_id >= LEG_LEFT && leg_id < LEG_COUNT;
}

static float motor_to_model_angle(leg_id_t leg_id, uint8_t joint_index, float motor_angle) {
    return motor_angle * joint_model_signs[leg_id][joint_index] +
           joint_model_offsets[leg_id][joint_index];
}

static float motor_to_model_velocity(leg_id_t leg_id, uint8_t joint_index, float motor_velocity) {
    return motor_velocity * joint_model_signs[leg_id][joint_index];
}

static float model_to_motor_angle(leg_id_t leg_id, uint8_t joint_index, float model_angle) {
    return (model_angle - joint_model_offsets[leg_id][joint_index]) *
           joint_model_signs[leg_id][joint_index];
}

static float model_to_motor_torque(leg_id_t leg_id, uint8_t joint_index, float model_torque) {
    return model_torque * joint_model_signs[leg_id][joint_index];
}

static void refresh_leg_state(leg_id_t leg_id) {
    leg_state_t *leg = &legs[leg_id];

    for (uint8_t i = 0; i < 2; ++i) {
        motor_t *motor = motor_get(joint_motors[leg_id][i]);
        if (motor != NULL) {
            leg->joint_angles[i] = motor_to_model_angle(leg_id, i, motor->position);
            leg->joint_velocities[i] = motor_to_model_velocity(leg_id, i, motor->velocity);
            leg->joint_torques[i] = model_to_motor_torque(leg_id, i, motor->torque);
        }
    }

    motor_t *wheel = motor_get(wheel_motors[leg_id]);
    if (wheel != NULL) {
        leg->wheel_position = wheel->position * wheel_torque_signs[leg_id];
        leg->wheel_velocity = wheel->velocity * wheel_torque_signs[leg_id];
        leg->wheel_torque = wheel->torque * wheel_torque_signs[leg_id];
    }

    float pos[2];
    float spd[2];
    vmc_leg_pos(leg->joint_angles[0], leg->joint_angles[1], pos);
    vmc_leg_spd(leg->joint_velocities[0], leg->joint_velocities[1],
                leg->joint_angles[0], leg->joint_angles[1], spd);

    leg->leg_length = pos[0];
    leg->leg_angle = pos[1];
    leg->leg_length_rate = spd[0];
    leg->leg_angle_rate = spd[1];

    leg->position.x = pos[0];
    leg->position.y = 0.0f;
    leg->position.z = pos[1];
    leg->velocity.vx = spd[0];
    leg->velocity.vy = 0.0f;
    leg->velocity.vz = spd[1];
}

bool leg_controller_init(void) {
    memset(legs, 0, sizeof(legs));
    for (uint8_t i = 0; i < LEG_COUNT; ++i) {
        refresh_leg_state((leg_id_t)i);
    }
    return true;
}

bool leg_set_position(leg_id_t leg_id, const leg_position_t *pos) {
    if (!valid_leg(leg_id) || pos == NULL) {
        return false;
    }

    float angles[2];
    if (!leg_inverse_kinematics(pos, angles)) {
        return false;
    }

    return motor_set_position(joint_motors[leg_id][0],
                              model_to_motor_angle(leg_id, 0U, angles[0])) &&
           motor_set_position(joint_motors[leg_id][1],
                              model_to_motor_angle(leg_id, 1U, angles[1]));
}

bool leg_set_velocity(leg_id_t leg_id, const leg_velocity_t *vel) {
    if (!valid_leg(leg_id) || vel == NULL) {
        return false;
    }

    legs[leg_id].velocity = *vel;
    return true;
}

bool leg_get_state(leg_id_t leg_id, leg_state_t *state) {
    if (!valid_leg(leg_id) || state == NULL) {
        return false;
    }

    refresh_leg_state(leg_id);
    memcpy(state, &legs[leg_id], sizeof(*state));
    return true;
}

bool leg_set_torques(leg_id_t leg_id, const float torques[2]) {
    if (!valid_leg(leg_id) || torques == NULL) {
        return false;
    }

    return motor_set_torque(joint_motors[leg_id][0],
                            model_to_motor_torque(leg_id, 0U, torques[0])) &&
           motor_set_torque(joint_motors[leg_id][1],
                            model_to_motor_torque(leg_id, 1U, torques[1]));
}

bool leg_set_virtual_force(leg_id_t leg_id, float axial_force, float leg_torque) {
    if (!valid_leg(leg_id)) {
        return false;
    }

    refresh_leg_state(leg_id);
    float joint_torques[2];
    vmc_leg_conv(axial_force, leg_torque,
                 legs[leg_id].joint_angles[0], legs[leg_id].joint_angles[1],
                 joint_torques);
    return leg_set_torques(leg_id, joint_torques);
}

bool leg_set_wheel_torque(leg_id_t leg_id, float wheel_torque) {
    if (!valid_leg(leg_id)) {
        return false;
    }

    return motor_set_torque(wheel_motors[leg_id], wheel_torque * wheel_torque_signs[leg_id]);
}

bool leg_forward_kinematics(const float angles[2], leg_position_t *position) {
    if (angles == NULL || position == NULL) {
        return false;
    }

    float pos[2];
    vmc_leg_pos(angles[0], angles[1], pos);
    position->x = pos[0];
    position->y = 0.0f;
    position->z = pos[1];
    return true;
}

bool leg_inverse_kinematics(const leg_position_t *position, float angles[2]) {
    if (position == NULL || angles == NULL) {
        return false;
    }

    float length = position->x;
    float angle = position->z;
    float reach = fminf(fmaxf(length, 0.04f), 0.24f);

    /* Symmetric seed for the five-bar leg. Fine positioning is closed by torque/VMC. */
    float hip = angle + acosf(fminf(fmaxf(reach / 0.242f, -1.0f), 1.0f));
    float knee = angle - acosf(fminf(fmaxf(reach / 0.242f, -1.0f), 1.0f));

    angles[0] = fminf(fmaxf(hip, MIN_JOINT_ANGLE), MAX_JOINT_ANGLE);
    angles[1] = fminf(fmaxf(knee, MIN_JOINT_ANGLE), MAX_JOINT_ANGLE);
    return true;
}

bool leg_update(void) {
    for (uint8_t i = 0; i < LEG_COUNT; ++i) {
        refresh_leg_state((leg_id_t)i);
    }
    return true;
}
