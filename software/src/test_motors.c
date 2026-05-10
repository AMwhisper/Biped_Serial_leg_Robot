/**
 * @file test_motors.c
 * @brief Motor controller unit tests
 * @author Team
 * @date 2026-05-09
 */

#include "test_framework.h"
#include "motor_controller.h"

/**
 * @brief Test motor initialization
 */
test_result_t test_motor_init(void) {
    TEST_ASSERT_TRUE(motor_controller_init());
    
    // Verify motor can be retrieved
    motor_t *motor = motor_get(1);
    TEST_ASSERT_NOT_NULL(motor);
    
    return TEST_PASS;
}

/**
 * @brief Test motor enable/disable
 */
test_result_t test_motor_enable_disable(void) {
    motor_t *motor = motor_get(1);
    TEST_ASSERT_NOT_NULL(motor);
    
    // Test enable
    TEST_ASSERT_TRUE(motor_enable(1));
    TEST_ASSERT_TRUE(motor->enabled);
    
    // Test disable
    TEST_ASSERT_TRUE(motor_disable(1));
    TEST_ASSERT_FALSE(motor->enabled);
    
    return TEST_PASS;
}

/**
 * @brief Test motor torque control
 */
test_result_t test_motor_torque(void) {
    motor_t *motor = motor_get(1);
    TEST_ASSERT_NOT_NULL(motor);
    
    motor_enable(1);
    
    // Set torque
    TEST_ASSERT_TRUE(motor_set_torque(1, 5.0f));
    
    // Verify target torque is set
    TEST_ASSERT_EQUAL(motor->target_torque, 5.0f);
    
    return TEST_PASS;
}

/**
 * @brief Test motor speed control
 */
test_result_t test_motor_speed(void) {
    motor_t *motor = motor_get(2);
    TEST_ASSERT_NOT_NULL(motor);
    
    motor_enable(2);
    
    // Set speed
    TEST_ASSERT_TRUE(motor_set_speed(2, 10.0f));
    
    // Verify target velocity is set
    TEST_ASSERT_EQUAL(motor->target_velocity, 10.0f);
    
    return TEST_PASS;
}

/**
 * @brief Test motor position control
 */
test_result_t test_motor_position(void) {
    motor_t *motor = motor_get(3);
    TEST_ASSERT_NOT_NULL(motor);
    
    motor_enable(3);
    
    // Set position
    TEST_ASSERT_TRUE(motor_set_position(3, 1.57f));
    
    // Verify target position is set
    TEST_ASSERT_EQUAL(motor->target_position, 1.57f);
    
    return TEST_PASS;
}

/**
 * @brief Test motor status retrieval
 */
test_result_t test_motor_status(void) {
    motor_t motor_status;
    
    TEST_ASSERT_TRUE(motor_get_status(1, &motor_status));
    TEST_ASSERT_EQUAL(motor_status.motor_id, 1);
    
    return TEST_PASS;
}

/* Test suite */
const test_case_t motor_tests[] = {
    {"Motor Initialization", test_motor_init},
    {"Motor Enable/Disable", test_motor_enable_disable},
    {"Motor Torque Control", test_motor_torque},
    {"Motor Speed Control", test_motor_speed},
    {"Motor Position Control", test_motor_position},
    {"Motor Status", test_motor_status}
};

#define MOTOR_TESTS_COUNT (sizeof(motor_tests) / sizeof(motor_tests[0]))

/**
 * @brief Run motor tests
 */
void run_motor_tests(void) {
    test_run_suite(motor_tests, MOTOR_TESTS_COUNT);
}
