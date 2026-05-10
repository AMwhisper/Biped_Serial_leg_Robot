/**
 * @file motion_planner.c
 * @brief Motion planning implementation
 * @author Team
 * @date 2026-05-09
 */

#include "motion_planner.h"
#include "leg_controller.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

/* Internal state */
static motion_cmd_t current_cmd = MOTION_IDLE;
static motion_params_t current_params = {0};
static uint32_t motion_step_counter = 0;

/* Gait phase enumeration */
typedef enum {
    GAIT_STANCE = 0,    // Leg in contact with ground
    GAIT_SWING = 1      // Leg moving through air
} gait_phase_t;

/* Leg gait states (which legs move together) */
static gait_phase_t leg_phases[4];

/**
 * @brief Initialize motion planner
 */
bool motion_planner_init(void) {
    current_cmd = MOTION_IDLE;
    memset(&current_params, 0, sizeof(motion_params_t));
    motion_step_counter = 0;
    
    // Initialize all legs in stance phase
    for (int i = 0; i < 4; i++) {
        leg_phases[i] = GAIT_STANCE;
    }
    
    return true;
}

/**
 * @brief Execute motion command
 */
bool motion_planner_execute(motion_cmd_t cmd, const motion_params_t *params) {
    if (params == NULL) {
        return false;
    }
    
    current_cmd = cmd;
    memcpy(&current_params, params, sizeof(motion_params_t));
    motion_step_counter = 0;
    
    return true;
}

/**
 * @brief Generate trotting gait pattern
 * Legs: 0=FL, 1=FR, 2=BL, 3=BR
 * Trot: FL+BR move together, FR+BL move together
 */
static void update_trotting_gait(float phase) {
    // Phase from 0 to 1 (full gait cycle)
    if (phase < 0.5f) {
        // First half: FL and BR in swing, FR and BL in stance
        leg_phases[0] = GAIT_SWING;  // FL
        leg_phases[1] = GAIT_STANCE; // FR
        leg_phases[2] = GAIT_STANCE; // BL
        leg_phases[3] = GAIT_SWING;  // BR
    } else {
        // Second half: FR and BL in swing, FL and BR in stance
        leg_phases[0] = GAIT_STANCE; // FL
        leg_phases[1] = GAIT_SWING;  // FR
        leg_phases[2] = GAIT_SWING;  // BL
        leg_phases[3] = GAIT_STANCE; // BR
    }
}

/**
 * @brief Update motion planner
 */
bool motion_planner_update(void) {
    if (current_cmd == MOTION_IDLE) {
        return true;
    }
    
    // Calculate phase in current gait cycle (0 to 1)
    uint32_t cycle_time_ms = (uint32_t)(current_params.gait_period * 1000.0f);
    if (cycle_time_ms == 0) cycle_time_ms = 1;
    
    uint32_t phase_time = motion_step_counter % cycle_time_ms;
    float phase = (float)phase_time / (float)cycle_time_ms;
    
    // Update leg gait states
    update_trotting_gait(phase);
    
    // Calculate target positions for each leg
    for (int leg_id = 0; leg_id < 4; leg_id++) {
        leg_position_t target_pos = {0};
        
        // Base position (varies by leg quadrant)
        float x_offset = (leg_id % 2 == 0) ? -0.1f : 0.1f;  // FL/BL vs FR/BR
        float z_offset = -0.2f;  // Ground level
        
        if (leg_phases[leg_id] == GAIT_SWING) {
            // Swing phase: lift and extend
            float swing_phase = phase * 2.0f;
            if (swing_phase > 1.0f) swing_phase -= 1.0f;
            
            target_pos.x = x_offset + current_params.step_length * sinf(swing_phase * M_PI);
            target_pos.z = z_offset - current_params.step_height * sinf(swing_phase * M_PI);
        } else {
            // Stance phase: push backward
            target_pos.x = x_offset - current_params.step_length * 0.5f;
            target_pos.z = z_offset;
        }
        
        // Execute motion command based on type
        switch (current_cmd) {
            case MOTION_WALK_FORWARD:
                target_pos.x += current_params.linear_velocity * phase * 0.1f;
                break;
            case MOTION_WALK_BACKWARD:
                target_pos.x -= current_params.linear_velocity * phase * 0.1f;
                break;
            case MOTION_TURN_LEFT:
                target_pos.x += current_params.angular_velocity * sinf(phase * M_PI);
                break;
            case MOTION_TURN_RIGHT:
                target_pos.x -= current_params.angular_velocity * sinf(phase * M_PI);
                break;
            default:
                break;
        }
        
        // Apply target position to leg
        leg_set_position(leg_id, &target_pos);
    }
    
    motion_step_counter++;
    
    return true;
}

/**
 * @brief Stop current motion
 */
bool motion_planner_stop(void) {
    current_cmd = MOTION_IDLE;
    motion_step_counter = 0;
    
    // Return all legs to neutral position
    for (int i = 0; i < 4; i++) {
        leg_position_t neutral_pos = {0.0f, 0.0f, -0.2f};
        leg_set_position(i, &neutral_pos);
    }
    
    return true;
}

/**
 * @brief Get current motion command
 */
motion_cmd_t motion_planner_get_current_cmd(void) {
    return current_cmd;
}

/**
 * @brief Set gait type
 */
bool motion_planner_set_gait(uint8_t gait_type) {
    // TODO: Implement different gait types
    // 0 = Trot (currently implemented)
    // 1 = Gallop
    // 2 = Pace
    // 3 = Bound
    
    (void)gait_type;  // Avoid unused parameter warning
    return true;
}
