/**
 * @file wheel_leg_control.h
 * @brief High-level balance controller for the serial wheel-legged robot.
 */

#ifndef WHEEL_LEG_CONTROL_H
#define WHEEL_LEG_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct {
    float theta;
    float dtheta;
    float x;
    float dx;
} wheel_leg_feedback_t;

typedef struct {
    float wheel_torque_left;
    float wheel_torque_right;
    float leg_force_left;
    float leg_force_right;
    float leg_torque_left;
    float leg_torque_right;
} wheel_leg_output_t;

typedef struct {
    float target_vx;
    float target_yaw_rate;
    float target_leg_length;
} wheel_leg_command_t;

bool wheel_leg_control_init(void);
bool wheel_leg_control_enable(bool enable);
void wheel_leg_control_set_command(const wheel_leg_command_t *command);
bool wheel_leg_control_update(const wheel_leg_feedback_t *feedback, wheel_leg_output_t *output);

#ifdef __cplusplus
}
#endif

#endif /* WHEEL_LEG_CONTROL_H */
