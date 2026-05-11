/**
 * @file motor_controller.c
 * @brief Motor controller implementation for Damiaotech MIT mode motors.
 */

#include "motor_controller.h"
#include "can_driver.h"
#include "motor_config.h"
#include <math.h>
#include <string.h>

extern uint32_t HAL_GetTick(void);

static motor_t motors[TOTAL_MOTORS];
static uint8_t motor_count;

static float clamp_value(float value, float min_value, float max_value) {
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static float motor_torque_limit(const motor_t *motor) {
    return (motor->motor_type == MOTOR_TYPE_6215) ? DM6215_MAX_TORQUE : DM4310_MAX_TORQUE;
}

static float motor_speed_limit(const motor_t *motor) {
    return (motor->motor_type == MOTOR_TYPE_6215) ? DM6215_MAX_SPEED : DM4310_MAX_SPEED;
}

static uint16_t float_to_uint(float x, float x_min, float x_max, uint8_t bits) {
    float span = x_max - x_min;
    float offset = x_min;
    uint32_t max_int = (1UL << bits) - 1UL;
    x = clamp_value(x, x_min, x_max);
    return (uint16_t)((x - offset) * (float)max_int / span);
}

static float uint_to_float(uint16_t x, float x_min, float x_max, uint8_t bits) {
    float span = x_max - x_min;
    uint32_t max_int = (1UL << bits) - 1UL;
    return ((float)x) * span / (float)max_int + x_min;
}

static can_status_t send_mit_special_command(uint8_t motor_id, uint8_t command) {
    can_message_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.can_id = motor_id;
    msg.dlc = 8;
    for (uint8_t i = 0; i < 7U; ++i) {
        msg.data[i] = 0xFFU;
    }
    msg.data[7] = command;
    return can_send(&msg);
}

static can_status_t send_mit_control(uint8_t motor_id, float p, float v, float kp, float kd, float torque) {
    motor_t *motor = motor_get(motor_id);
    if (motor == NULL) {
        return CAN_STATUS_ERROR;
    }

    float v_max = motor_speed_limit(motor);
    float t_max = motor_torque_limit(motor);

    uint16_t p_int = float_to_uint(p, DM_MIT_P_MIN, DM_MIT_P_MAX, 16);
    uint16_t v_int = float_to_uint(v, -v_max, v_max, 12);
    uint16_t kp_int = float_to_uint(kp, DM_MIT_KP_MIN, DM_MIT_KP_MAX, 12);
    uint16_t kd_int = float_to_uint(kd, DM_MIT_KD_MIN, DM_MIT_KD_MAX, 12);
    uint16_t t_int = float_to_uint(torque, -t_max, t_max, 12);

    can_message_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.can_id = motor_id;
    msg.dlc = 8;
    msg.data[0] = (uint8_t)(p_int >> 8);
    msg.data[1] = (uint8_t)(p_int & 0xFFU);
    msg.data[2] = (uint8_t)(v_int >> 4);
    msg.data[3] = (uint8_t)(((v_int & 0x0FU) << 4) | (kp_int >> 8));
    msg.data[4] = (uint8_t)(kp_int & 0xFFU);
    msg.data[5] = (uint8_t)(kd_int >> 4);
    msg.data[6] = (uint8_t)(((kd_int & 0x0FU) << 4) | (t_int >> 8));
    msg.data[7] = (uint8_t)(t_int & 0xFFU);

    return can_send(&msg);
}

static void put_float_le(uint8_t *dst, float value) {
    memcpy(dst, &value, sizeof(value));
}

static uint8_t feedback_id_from_frame(const can_message_t *msg) {
    if (msg->can_id >= 1U && msg->can_id <= TOTAL_MOTORS) {
        return (uint8_t)msg->can_id;
    }

    if (msg->can_id >= CAN_MOTOR_FEEDBACK_ID + 1U &&
        msg->can_id <= CAN_MOTOR_FEEDBACK_ID + TOTAL_MOTORS) {
        return (uint8_t)(msg->can_id - CAN_MOTOR_FEEDBACK_ID);
    }

    if (msg->dlc >= 6U) {
        uint8_t id = msg->data[0] & 0x0FU;
        if (id >= 1U && id <= TOTAL_MOTORS) {
            return id;
        }
    }

    return 0U;
}

static void parse_feedback(const can_message_t *msg) {
    if (msg == NULL || msg->dlc < 6U) {
        return;
    }

    uint8_t motor_id = feedback_id_from_frame(msg);
    if (motor_id == 0U) {
        return;
    }

    motor_t *motor = &motors[motor_id - 1U];
    float v_max = motor_speed_limit(motor);
    float t_max = motor_torque_limit(motor);

    motor->status_code = msg->data[0] >> 4;
    motor->last_feedback_ms = HAL_GetTick();
    motor->feedback_seen = true;

    uint16_t p_int = ((uint16_t)msg->data[1] << 8) | msg->data[2];
    uint16_t v_int = ((uint16_t)msg->data[3] << 4) | (msg->data[4] >> 4);
    uint16_t t_int = (((uint16_t)msg->data[4] & 0x0FU) << 8) | msg->data[5];

    motor->position = uint_to_float(p_int, DM_MIT_P_MIN, DM_MIT_P_MAX, 16);
    motor->velocity = uint_to_float(v_int, -v_max, v_max, 12);
    motor->torque = uint_to_float(t_int, -t_max, t_max, 12);
    if (msg->dlc >= 8U) {
        motor->temperature = msg->data[7];
    }
}

bool motor_controller_init(void) {
    memset(motors, 0, sizeof(motors));
    motor_count = TOTAL_MOTORS;

    for (uint8_t i = 0; i < motor_count; ++i) {
        motors[i].motor_id = (uint8_t)(i + 1U);
        motors[i].motor_type = (i < MAX_JOINT_MOTORS) ? MOTOR_TYPE_4310 : MOTOR_TYPE_6215;
    }

    can_set_filter(0x000U, 0x000U);
    return true;
}

bool motor_enable(uint8_t motor_id) {
    motor_t *motor = motor_get(motor_id);
    if (motor == NULL) {
        return false;
    }

    if (send_mit_special_command(motor_id, CMD_MOTOR_ON) != CAN_STATUS_OK) {
        return false;
    }

    motor->enabled = true;
    return true;
}

bool motor_disable(uint8_t motor_id) {
    motor_t *motor = motor_get(motor_id);
    if (motor == NULL) {
        return false;
    }

    motor->target_torque = 0.0f;
    motor->target_velocity = 0.0f;
    motor->target_position = motor->position;
    motor->enabled = false;

    return send_mit_special_command(motor_id, CMD_MOTOR_OFF) == CAN_STATUS_OK;
}

bool motor_set_torque(uint8_t motor_id, float torque) {
    motor_t *motor = motor_get(motor_id);
    if (motor == NULL || !motor->enabled) {
        return false;
    }

    motor->target_torque = clamp_value(torque, -motor_torque_limit(motor), motor_torque_limit(motor));
    motor->target_position = 0.0f;
    motor->target_velocity = 0.0f;

    return send_mit_control(motor_id, 0.0f, 0.0f, 0.0f, 0.0f, motor->target_torque) == CAN_STATUS_OK;
}

bool motor_set_speed_mode(uint8_t motor_id, float speed) {
    motor_t *motor = motor_get(motor_id);
    if (motor == NULL || !motor->enabled) {
        return false;
    }

    motor->target_velocity = clamp_value(speed, -motor_speed_limit(motor), motor_speed_limit(motor));
    motor->target_position = 0.0f;
    motor->target_torque = 0.0f;

    can_message_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.can_id = 0x200U + motor_id;
    msg.dlc = 4U;
    put_float_le(&msg.data[0], motor->target_velocity);

    return can_send(&msg) == CAN_STATUS_OK;
}

bool motor_set_position_velocity_mode(uint8_t motor_id, float position, float speed) {
    motor_t *motor = motor_get(motor_id);
    if (motor == NULL || !motor->enabled) {
        return false;
    }

    motor->target_position = clamp_value(position, DM_MIT_P_MIN, DM_MIT_P_MAX);
    motor->target_velocity = clamp_value(speed, -motor_speed_limit(motor), motor_speed_limit(motor));
    motor->target_torque = 0.0f;

    can_message_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.can_id = 0x100U + motor_id;
    msg.dlc = 8U;
    put_float_le(&msg.data[0], motor->target_position);
    put_float_le(&msg.data[4], motor->target_velocity);

    return can_send(&msg) == CAN_STATUS_OK;
}

bool motor_set_speed(uint8_t motor_id, float speed) {
    motor_t *motor = motor_get(motor_id);
    if (motor == NULL || !motor->enabled) {
        return false;
    }

    motor->target_velocity = clamp_value(speed, -motor_speed_limit(motor), motor_speed_limit(motor));
    motor->target_position = 0.0f;
    motor->target_torque = 0.0f;

    return send_mit_control(motor_id, 0.0f, motor->target_velocity, 0.0f, 0.15f, 0.0f) == CAN_STATUS_OK;
}

bool motor_set_position(uint8_t motor_id, float position) {
    motor_t *motor = motor_get(motor_id);
    if (motor == NULL || !motor->enabled) {
        return false;
    }

    motor->target_position = clamp_value(position, DM_MIT_P_MIN, DM_MIT_P_MAX);
    motor->target_velocity = 0.0f;
    motor->target_torque = 0.0f;

    return send_mit_control(motor_id, motor->target_position, 0.0f, 20.0f, 0.6f, 0.0f) == CAN_STATUS_OK;
}

bool motor_get_status(uint8_t motor_id, motor_t *motor) {
    motor_t *src = motor_get(motor_id);
    if (src == NULL || motor == NULL) {
        return false;
    }

    memcpy(motor, src, sizeof(*motor));
    return true;
}

bool motor_feedback_recent(uint8_t motor_id, uint32_t now_ms, uint32_t timeout_ms) {
    motor_t *motor = motor_get(motor_id);
    if (motor == NULL || !motor->feedback_seen) {
        return false;
    }

    return (now_ms - motor->last_feedback_ms) <= timeout_ms;
}

bool motor_update(void) {
    can_message_t msg;
    while (can_message_available()) {
        if (can_receive(&msg) == CAN_STATUS_OK) {
            parse_feedback(&msg);
        }
    }
    return true;
}

motor_t *motor_get(uint8_t motor_id) {
    if (motor_id >= 1U && motor_id <= motor_count) {
        return &motors[motor_id - 1U];
    }
    return NULL;
}
