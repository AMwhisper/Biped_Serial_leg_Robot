/**
 * @file test_framework.c
 * @brief Test framework implementation
 * @author Team
 * @date 2026-05-09
 */

#include "test_framework.h"
#include "uart_interface.h"
#include <stdio.h>

/* Global test counters */
uint32_t test_total = 0;
uint32_t test_passed = 0;
uint32_t test_failed = 0;
uint32_t test_skipped = 0;

/**
 * @brief Run single test case
 */
test_result_t test_run_case(const test_case_t *test) {
    if (test == NULL || test->func == NULL) {
        return TEST_FAIL;
    }
    
    test_result_t result = test->func();
    test_total++;
    
    switch (result) {
        case TEST_PASS:
            test_passed++;
            uart_printf("[PASS] %s\n", test->name);
            break;
        case TEST_FAIL:
            test_failed++;
            uart_printf("[FAIL] %s\n", test->name);
            break;
        case TEST_SKIP:
            test_skipped++;
            uart_printf("[SKIP] %s\n", test->name);
            break;
    }
    
    return result;
}

/**
 * @brief Run test suite
 */
void test_run_suite(const test_case_t *tests, uint32_t count) {
    if (tests == NULL || count == 0) {
        return;
    }
    
    test_reset_counters();
    uart_debug_msg("\n========== Test Suite Starting ==========\n");
    
    for (uint32_t i = 0; i < count; i++) {
        test_run_case(&tests[i]);
    }
    
    test_print_results();
}

/**
 * @brief Print test results
 */
void test_print_results(void) {
    uart_printf("\n========== Test Results ==========\n");
    uart_printf("Total:  %u\n", test_total);
    uart_printf("Passed: %u\n", test_passed);
    uart_printf("Failed: %u\n", test_failed);
    uart_printf("Skipped: %u\n", test_skipped);
    
    if (test_failed == 0) {
        uart_debug_msg("All tests passed!\n");
    } else {
        uart_debug_msg("Some tests failed!\n");
    }
}

/**
 * @brief Reset test counters
 */
void test_reset_counters(void) {
    test_total = 0;
    test_passed = 0;
    test_failed = 0;
    test_skipped = 0;
}
