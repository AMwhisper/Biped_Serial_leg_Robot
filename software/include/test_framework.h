/**
 * @file test_framework.h
 * @brief Unit testing framework for embedded systems
 * @author Team
 * @date 2026-05-09
 */

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/* Test result enumeration */
typedef enum {
    TEST_PASS = 0,
    TEST_FAIL = 1,
    TEST_SKIP = 2
} test_result_t;

/* Test function prototype */
typedef test_result_t (*test_func_t)(void);

/* Test suite structure */
typedef struct {
    const char *name;
    test_func_t func;
} test_case_t;

/* Global test counters */
extern uint32_t test_total;
extern uint32_t test_passed;
extern uint32_t test_failed;
extern uint32_t test_skipped;

/* Assertion macros */
#define TEST_ASSERT_TRUE(condition) \
    if (!(condition)) { \
        return TEST_FAIL; \
    }

#define TEST_ASSERT_FALSE(condition) \
    if (condition) { \
        return TEST_FAIL; \
    }

#define TEST_ASSERT_EQUAL(actual, expected) \
    if ((actual) != (expected)) { \
        return TEST_FAIL; \
    }

#define TEST_ASSERT_NOT_NULL(ptr) \
    if ((ptr) == NULL) { \
        return TEST_FAIL; \
    }

#define TEST_ASSERT_NULL(ptr) \
    if ((ptr) != NULL) { \
        return TEST_FAIL; \
    }

/* Function Declarations */

/**
 * @brief Run single test case
 * @param test Test case to run
 * @return Test result
 */
test_result_t test_run_case(const test_case_t *test);

/**
 * @brief Run test suite
 * @param tests Array of test cases
 * @param count Number of test cases
 */
void test_run_suite(const test_case_t *tests, uint32_t count);

/**
 * @brief Print test results
 */
void test_print_results(void);

/**
 * @brief Reset test counters
 */
void test_reset_counters(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_FRAMEWORK_H */
