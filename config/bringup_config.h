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
#define BRINGUP_ENABLE_REMOTE_CONTROL  0
#define BRINGUP_ENABLE_BALANCE_CONTROL 0

/* Single-motor torque test.
 * Set BRINGUP_MOTOR_TEST_ENABLE to 1 only after CAN feedback is confirmed.
 */
#define BRINGUP_MOTOR_TEST_ENABLE      1
#define BRINGUP_TEST_MOTOR_ID          1
#define BRINGUP_TEST_TORQUE_NM         0.05f
#define BRINGUP_TEST_PERIOD_MS         1000U

#endif /* BRINGUP_CONFIG_H */
