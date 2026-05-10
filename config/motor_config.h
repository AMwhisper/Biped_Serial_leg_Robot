/**
 * @file motor_config.h
 * @brief Motor configuration for RoboMaster C Type Development Board
 * @author Team
 * @date 2026-05-09
 */

#ifndef MOTOR_CONFIG_H
#define MOTOR_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ==================== Motor Types ==================== */
#define MOTOR_TYPE_6215    0x01  // Damiaotech 6215
#define MOTOR_TYPE_4310    0x02  // Damiaotech 4310

/* ==================== Motor IDs ==================== */
// Damiaotech 4310 Joint Motors
#define MOTOR_4310_1       0x01
#define MOTOR_4310_2       0x02
#define MOTOR_4310_3       0x03
#define MOTOR_4310_4       0x04

// Damiaotech 6215 Drive Wheel Motors
#define MOTOR_6215_1       0x05
#define MOTOR_6215_2       0x06

/* ==================== Damiaotech 6215 Configuration ==================== */
#define DM6215_MAX_TORQUE      12.0f    // Max torque in Nm
#define DM6215_MAX_SPEED       65.0f    // Max speed in rad/s
#define DM6215_GEAR_RATIO      1.0f     // Gear ratio
#define DM6215_ENCODER_BITS    16       // Encoder resolution

/* ==================== Damiaotech 4310 Configuration ==================== */
#define DM4310_MAX_TORQUE      5.0f     // Max torque in Nm
#define DM4310_MAX_SPEED       130.0f   // Max speed in rad/s
#define DM4310_GEAR_RATIO      1.0f     // Gear ratio
#define DM4310_ENCODER_BITS    16       // Encoder resolution

/* ==================== CAN Configuration ==================== */
#define CAN_MOTOR_FEEDBACK_ID  0x200    // Legacy/base feedback ID, MIT feedback may also use configured feedback IDs
#define CAN_MOTOR_COMMAND_ID   0x000    // MIT command frame ID equals motor ID

/* ==================== Damiaotech MIT Ranges ==================== */
#define DM_MIT_P_MIN           -12.5f
#define DM_MIT_P_MAX           12.5f
#define DM_MIT_KP_MIN          0.0f
#define DM_MIT_KP_MAX          500.0f
#define DM_MIT_KD_MIN          0.0f
#define DM_MIT_KD_MAX          5.0f

/* ==================== Motor Control Parameters ==================== */
#define MAX_JOINT_MOTORS       4
#define MAX_DRIVE_MOTORS       2
#define TOTAL_MOTORS           6

/* ==================== Control Modes ==================== */
#define CTRL_MODE_TORQUE       0x01     // Pure torque control
#define CTRL_MODE_SPEED        0x02     // Speed control
#define CTRL_MODE_POSITION     0x03     // Position control
#define CTRL_MODE_MIXED        0x04     // Mixed position and speed

/* ==================== Communication Protocol ==================== */
// Damiaotech MIT special commands, bytes 0-6 are 0xFF and byte 7 is the command.
#define CMD_MOTOR_ON           0xFC
#define CMD_MOTOR_OFF          0xFD
#define CMD_SET_ZERO           0xFE

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_CONFIG_H */
