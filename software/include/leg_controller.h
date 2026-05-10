/**
 * @file leg_controller.h
 * @brief Two-leg VMC interface for the wheel-legged robot.
 */

#ifndef LEG_CONTROLLER_H
#define LEG_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float x;
    float y;
    float z;
} leg_position_t;

typedef struct {
    float vx;
    float vy;
    float vz;
} leg_velocity_t;

typedef enum {
    LEG_LEFT = 0,
    LEG_RIGHT = 1,
    LEG_COUNT = 2,

    LEG_FRONT_LEFT = LEG_LEFT,
    LEG_FRONT_RIGHT = LEG_RIGHT
} leg_id_t;

typedef struct {
    leg_position_t position;       /* x = leg length, z = leg angle */
    leg_velocity_t velocity;       /* vx = d(length), vz = d(angle) */
    float joint_angles[2];         /* phi1, phi4 from MATLAB model */
    float joint_velocities[2];
    float joint_torques[2];
    float wheel_position;
    float wheel_velocity;
    float wheel_torque;
    float leg_length;
    float leg_angle;
    float leg_length_rate;
    float leg_angle_rate;
} leg_state_t;

bool leg_controller_init(void);
bool leg_set_position(leg_id_t leg_id, const leg_position_t *pos);
bool leg_set_velocity(leg_id_t leg_id, const leg_velocity_t *vel);
bool leg_get_state(leg_id_t leg_id, leg_state_t *state);
bool leg_set_torques(leg_id_t leg_id, const float torques[2]);
bool leg_set_virtual_force(leg_id_t leg_id, float axial_force, float leg_torque);
bool leg_set_wheel_torque(leg_id_t leg_id, float wheel_torque);
bool leg_forward_kinematics(const float angles[2], leg_position_t *position);
bool leg_inverse_kinematics(const leg_position_t *position, float angles[2]);
bool leg_update(void);

#ifdef __cplusplus
}
#endif

#endif /* LEG_CONTROLLER_H */
