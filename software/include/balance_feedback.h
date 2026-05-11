/**
 * @file balance_feedback.h
 * @brief Feedback adapter for the wheel-leg LQR balance loop.
 */

#ifndef BALANCE_FEEDBACK_H
#define BALANCE_FEEDBACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wheel_leg_control.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float theta;
    float dtheta;
    bool attitude_valid;
} balance_attitude_t;

void balance_feedback_init(void);
void balance_feedback_set_attitude(float theta_rad, float dtheta_rad_s, bool valid);
bool balance_feedback_update(uint32_t now_ms);
bool balance_feedback_get(wheel_leg_feedback_t *feedback);

#ifdef __cplusplus
}
#endif

#endif /* BALANCE_FEEDBACK_H */
