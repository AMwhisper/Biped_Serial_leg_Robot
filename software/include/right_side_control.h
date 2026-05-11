/**
 * @file right_side_control.h
 * @brief Thin right-side reference control adapter.
 */

#ifndef RIGHT_SIDE_CONTROL_H
#define RIGHT_SIDE_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lqr_controller.h"
#include <stdbool.h>

typedef struct {
    float theta;       /* Body pitch angle, rad */
    float dtheta;      /* Body pitch angular velocity, rad/s */
    float x;           /* Wheel/body horizontal position, m */
    float dx;          /* Wheel/body horizontal velocity, m/s */
    float axial_force; /* VMC axial force F, N */
} right_side_control_input_t;

typedef struct {
    float phi1;
    float phi4;
    float dphi1;
    float dphi4;
    float pos[2];          /* pos[0] = equivalent pole length, pos[1] = pole angle */
    float spd[2];          /* spd[0] = length rate, spd[1] = pole angular rate */
    float lqr_output[2];   /* [T, Tp] */
    float joint_torque[2]; /* [phi1 torque, phi4 torque] */
} right_side_control_debug_t;

typedef struct {
    lqr_controller_t lqr;
    bool initialized;
} right_side_control_t;

bool right_side_control_init(right_side_control_t *control, float initial_leg_length);
bool right_side_control_enable_motors(void);
bool right_side_control_disable_motors(void);
bool right_side_control_update(right_side_control_t *control,
                               const right_side_control_input_t *input,
                               right_side_control_debug_t *debug);

#ifdef __cplusplus
}
#endif

#endif /* RIGHT_SIDE_CONTROL_H */
