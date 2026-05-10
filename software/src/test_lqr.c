/**
 * @file test_lqr.c
 * @brief LQR controller unit tests
 * @author Team
 * @date 2026-05-09
 */

#include "test_framework.h"
#include "lqr_controller.h"
#include "lqr_gains.h"
#include "lqr_system.h"
#include "flight_controller.h"
#include <math.h>

/**
 * @brief Test LQR gain calculation
 */
test_result_t test_lqr_gains_compute(void) {
    float K[12];
    
    /* Test for different leg lengths */
    lqr_gains_compute(0.07f, K);  /* Midpoint of range */
    
    /* K should be a 2x6 matrix with reasonable values */
    TEST_ASSERT_TRUE(K[0] != 0.0f);  /* K[0,0] should be non-zero */
    
    /* Check bounds (gains should be reasonable) */
    TEST_ASSERT_TRUE(fabsf(K[0]) < 1000.0f);  /* Not unreasonably large */
    
    return TEST_PASS;
}

/**
 * @brief Test LQR controller initialization
 */
test_result_t test_lqr_controller_init(void) {
    lqr_controller_t controller;
    
    TEST_ASSERT_TRUE(lqr_controller_init(&controller, 0.07f));
    
    /* Verify configuration */
    TEST_ASSERT_TRUE(controller.config.leg_length == 0.07f);
    TEST_ASSERT_TRUE(controller.config.enable_saturation == true);
    
    return TEST_PASS;
}

/**
 * @brief Test LQR state control
 */
test_result_t test_lqr_control_computation(void) {
    lqr_controller_t controller;
    lqr_controller_init(&controller, 0.07f);
    
    /* Set test state */
    lqr_state_t state = {
        .theta = 0.1f,   /* 0.1 rad body angle */
        .dtheta = 0.0f,
        .x = 0.0f,
        .dx = 0.0f,
        .phi = 0.0f,
        .dphi = 0.0f
    };
    
    lqr_controller_set_state(&controller, &state);
    TEST_ASSERT_TRUE(lqr_compute_control(&controller));
    
    /* Verify control output computed */
    float torque[2];
    lqr_get_control(&controller, torque);
    
    /* Control should be non-zero for non-zero state */
    TEST_ASSERT_TRUE(torque[0] != 0.0f || torque[1] != 0.0f);
    
    return TEST_PASS;
}

/**
 * @brief Test control saturation
 */
test_result_t test_lqr_saturation(void) {
    lqr_controller_t controller;
    lqr_controller_init(&controller, 0.07f);
    
    controller.config.torque_limit = 1.0f;
    controller.config.enable_saturation = true;
    
    /* Set large error state */
    lqr_state_t state = {
        .theta = 1.0f,
        .dtheta = 1.0f,
        .x = 1.0f,
        .dx = 1.0f,
        .phi = 1.0f,
        .dphi = 1.0f
    };
    
    lqr_controller_set_state(&controller, &state);
    lqr_compute_control(&controller);
    
    float torque[2];
    lqr_get_control(&controller, torque);
    
    /* Verify saturation is applied */
    TEST_ASSERT_TRUE(fabsf(torque[0]) <= controller.config.torque_limit);
    TEST_ASSERT_TRUE(fabsf(torque[1]) <= controller.config.torque_limit);
    
    return TEST_PASS;
}

/**
 * @brief Test error norm calculation
 */
test_result_t test_lqr_error_norm(void) {
    lqr_controller_t controller;
    lqr_controller_init(&controller, 0.07f);
    
    /* Zero state should give zero error */
    lqr_state_t zero_state = {0};
    lqr_controller_set_state(&controller, &zero_state);
    float error0 = lqr_get_error_norm(&controller);
    TEST_ASSERT_TRUE(error0 == 0.0f);
    
    /* Non-zero state should give non-zero error */
    lqr_state_t nonzero_state = {.theta = 0.1f};
    lqr_controller_set_state(&controller, &nonzero_state);
    float error1 = lqr_get_error_norm(&controller);
    TEST_ASSERT_TRUE(error1 > 0.0f);
    
    return TEST_PASS;
}

/**
 * @brief Test flight controller initialization
 */
test_result_t test_flight_controller_init(void) {
    flight_joint_controller_t controller;
    
    TEST_ASSERT_TRUE(flight_controller_init(&controller));
    TEST_ASSERT_TRUE(controller.phase == FLIGHT_PHASE_STANCE);
    TEST_ASSERT_FALSE(controller.in_flight);
    
    return TEST_PASS;
}

/**
 * @brief Test flight phase transition
 */
test_result_t test_flight_phase_transition(void) {
    flight_joint_controller_t controller;
    flight_controller_init(&controller);
    
    /* Update through phases */
    flight_controller_update(&controller, FLIGHT_PHASE_STANCE, 0.0f, 0.0f, 0.0f, 0.0f);
    TEST_ASSERT_FALSE(controller.in_flight);
    
    flight_controller_update(&controller, FLIGHT_PHASE_THRUST, 0.1f, 0.2f, 0.0f, 0.0f);
    TEST_ASSERT_FALSE(controller.in_flight);
    
    flight_controller_update(&controller, FLIGHT_PHASE_FLIGHT, 0.1f, 0.2f, 0.1f, 0.2f);
    TEST_ASSERT_TRUE(controller.in_flight);
    
    return TEST_PASS;
}

/**
 * @brief Test LQR system integration
 */
test_result_t test_lqr_system_init(void) {
    lqr_system_t sys;
    
    TEST_ASSERT_TRUE(lqr_system_init(&sys, 0.07f));
    TEST_ASSERT_TRUE(sys.mode == CONTROL_MODE_IDLE);
    
    return TEST_PASS;
}

/**
 * @brief Test LQR system mode switching
 */
test_result_t test_lqr_system_modes(void) {
    lqr_system_t sys;
    lqr_system_init(&sys, 0.07f);
    
    /* Test mode transitions */
    TEST_ASSERT_TRUE(lqr_system_set_mode(&sys, CONTROL_MODE_LQR_REGULATION));
    TEST_ASSERT_TRUE(lqr_system_get_mode(&sys) == CONTROL_MODE_LQR_REGULATION);
    
    TEST_ASSERT_TRUE(lqr_system_set_mode(&sys, CONTROL_MODE_FLIGHT_HOLD));
    TEST_ASSERT_TRUE(lqr_system_get_mode(&sys) == CONTROL_MODE_FLIGHT_HOLD);
    
    TEST_ASSERT_TRUE(lqr_system_set_mode(&sys, CONTROL_MODE_IDLE));
    TEST_ASSERT_TRUE(lqr_system_get_mode(&sys) == CONTROL_MODE_IDLE);
    
    return TEST_PASS;
}

/**
 * @brief Test jump sequence execution
 */
test_result_t test_lqr_jump_sequence(void) {
    lqr_system_t sys;
    lqr_system_init(&sys, 0.07f);
    
    lqr_state_t state = {.theta = 0.05f, .x = 0.0f, .phi = 0.1f};
    
    /* Execute through jump phases */
    TEST_ASSERT_TRUE(lqr_system_execute_jump(&sys, &state, FLIGHT_PHASE_STANCE));
    TEST_ASSERT_TRUE(lqr_system_execute_jump(&sys, &state, FLIGHT_PHASE_THRUST));
    TEST_ASSERT_TRUE(lqr_system_execute_jump(&sys, &state, FLIGHT_PHASE_FLIGHT));
    
    return TEST_PASS;
}

/**
 * @brief Test gain coefficient retrieval
 */
test_result_t test_lqr_gain_coefficients(void) {
    /* Test getting specific coefficients */
    float coeff;
    
    coeff = lqr_gains_get_coeff(0, 0, 0);  /* K[0,0] L0^3 coefficient */
    TEST_ASSERT_TRUE(fabsf(coeff - 3228.25952f) < 1e-3f);
    
    coeff = lqr_gains_get_coeff(0, 0, 3);  /* K[0,0] constant */
    TEST_ASSERT_TRUE(fabsf(coeff - 0.317834556f) < 1e-6f);
    
    return TEST_PASS;
}

/* LQR test suite */
const test_case_t lqr_tests[] = {
    {"LQR Gain Computation", test_lqr_gains_compute},
    {"LQR Controller Init", test_lqr_controller_init},
    {"LQR Control Computation", test_lqr_control_computation},
    {"LQR Control Saturation", test_lqr_saturation},
    {"LQR Error Norm", test_lqr_error_norm},
    {"Flight Controller Init", test_flight_controller_init},
    {"Flight Phase Transition", test_flight_phase_transition},
    {"LQR System Init", test_lqr_system_init},
    {"LQR System Modes", test_lqr_system_modes},
    {"LQR Jump Sequence", test_lqr_jump_sequence},
    {"LQR Gain Coefficients", test_lqr_gain_coefficients}
};

#define LQR_TESTS_COUNT (sizeof(lqr_tests) / sizeof(lqr_tests[0]))

/**
 * @brief Run LQR tests
 */
void run_lqr_tests(void) {
    test_run_suite(lqr_tests, LQR_TESTS_COUNT);
}
