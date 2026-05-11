/**
 * @file bringup_config.h
 * @brief Safety switches for first power-on and motor bring-up.
 */

#ifndef BRINGUP_CONFIG_H
#define BRINGUP_CONFIG_H

/* CAN bitrate selection.
 * 0: 100 kbps, matches the old CubeMX CAN2 settings you pasted.
 * 1: 1 Mbps, typical Damiaotech motor default.
 */
#define BRINGUP_CAN_1MBPS              1

/* Firmware boot mode.
 * Keep both disabled for the first flash with motors powered off.
 */
#define BRINGUP_ENABLE_REMOTE_CONTROL  1
#define BRINGUP_ENABLE_BALANCE_CONTROL 0
#define BRINGUP_ENABLE_IMU_FEEDBACK    0

/* Raw CAN scope test.
 * Sends a simple frame periodically so PB6/CANH/CANL can be checked with a scope.
 * Keep motor power off for this test.
 */
#define BRINGUP_CAN_SCOPE_TEST_ENABLE  0
#define BRINGUP_CAN_SCOPE_TEST_ID      0x123U
#define BRINGUP_CAN_SCOPE_PERIOD_MS    10U

/* Single-motor torque test.
 * Set BRINGUP_MOTOR_TEST_ENABLE to 1 only after CAN feedback is confirmed.
 */
#define BRINGUP_MOTOR_TEST_ENABLE      0
#define BRINGUP_TEST_MOTOR_ID          3
#define BRINGUP_TEST_MODE_MIT_TORQUE   0
#define BRINGUP_TEST_MODE_PV_POSITION  1
#define BRINGUP_TEST_MODE_SPEED        2
#define BRINGUP_TEST_CONTROL_MODE      BRINGUP_TEST_MODE_MIT_TORQUE
#define BRINGUP_TEST_TORQUE_NM         0.2f
#define BRINGUP_TEST_TORQUE_LIMIT_NM   0.25f
#define BRINGUP_TEST_SPEED_RAD_S       0.2f
#define BRINGUP_TEST_POSITION_DELTA_RAD 0.05f
#define BRINGUP_TEST_PERIOD_MS         2000U
#define BRINGUP_TEST_ALTERNATE_DIRECTION 0
#define BRINGUP_MOTOR_ENABLE_ONLY_MS   2000U
#define BRINGUP_MOTOR_ENABLE_RETRY_MS  20U
#define BRINGUP_MOTOR_FEEDBACK_TIMEOUT_MS 200U
#define BRINGUP_REQUIRE_FEEDBACK_FOR_TORQUE 0
#define BRINGUP_DISABLE_NON_TEST_MOTORS 1
#define BRINGUP_NON_TEST_DISABLE_PERIOD_MS 100U
#define BRINGUP_BOOT_DISABLE_ALL_MOTORS 1
#define BRINGUP_IDLE_DISABLE_PERIOD_MS  100U
#define BRINGUP_REMOTE_WHEEL_TEST_ENABLE 0
#define BRINGUP_REMOTE_WHEEL_TEST_TORQUE_LIMIT_NM 0.15f
#define BRINGUP_BALANCE_TORQUE_LIMIT_NM 0.25f

#endif /* BRINGUP_CONFIG_H */
