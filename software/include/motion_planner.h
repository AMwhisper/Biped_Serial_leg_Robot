/**
 * @file motion_planner.h
 * @brief Motion planning interface for quadruped locomotion
 * @author Team
 * @date 2026-05-09
 */

#ifndef MOTION_PLANNER_H
#define MOTION_PLANNER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Motion commands */
typedef enum {
    MOTION_IDLE = 0,
    MOTION_WALK_FORWARD,
    MOTION_WALK_BACKWARD,
    MOTION_TURN_LEFT,
    MOTION_TURN_RIGHT,
    MOTION_JUMP,
    MOTION_DANCE
} motion_cmd_t;

/* Motion parameters */
typedef struct {
    float linear_velocity;      // m/s
    float angular_velocity;     // rad/s
    float step_height;          // m
    float step_length;          // m
    float gait_period;          // seconds
} motion_params_t;

/* Function Declarations */

/**
 * @brief Initialize motion planner
 * @return true if successful
 */
bool motion_planner_init(void);

/**
 * @brief Execute motion command
 * @param cmd Motion command
 * @param params Motion parameters
 * @return true if successful
 */
bool motion_planner_execute(motion_cmd_t cmd, const motion_params_t *params);

/**
 * @brief Update motion planner (main loop call)
 * @return true if successful
 */
bool motion_planner_update(void);

/**
 * @brief Stop current motion
 * @return true if successful
 */
bool motion_planner_stop(void);

/**
 * @brief Get current motion command
 * @return Current motion command
 */
motion_cmd_t motion_planner_get_current_cmd(void);

/**
 * @brief Set gait type
 * @param gait_type Gait type identifier (0-3 for different gaits)
 * @return true if successful
 */
bool motion_planner_set_gait(uint8_t gait_type);

#ifdef __cplusplus
}
#endif

#endif /* MOTION_PLANNER_H */
