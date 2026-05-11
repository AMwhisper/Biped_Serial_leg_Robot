#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

/* ==================== RoboMaster C Type Hardware Configuration ==================== */

/* CAN2: PB5 = RX, PB6 = TX. Timing is configured in board/can.c. */
#define CAN_DEVICE              CAN2
#define CAN_BAUDRATE            1000000
#define CAN_RX_PIN              GPIO_PIN_5
#define CAN_TX_PIN              GPIO_PIN_6
#define CAN_GPIO_PORT           GPIOB

/* Motor update and feedback timing */
#define MOTOR_CONTROL_FREQ      500U
#define MOTOR_FEEDBACK_TIMEOUT  100U

/* Damiaotech 4310 joint motors.
 *
 * The right side is the reference model for control:
 *   CAN ID 3 -> phi4
 *   CAN ID 4 -> phi1
 *
 * MATLAB VMC functions always use phi1, phi4 order. When commanding the right
 * leg, map torque[0] to ID4 and torque[1] to ID3.
 */
#define LEFT_MIRROR_PHI4_MOTOR_ID 0x01
#define LEFT_MIRROR_PHI1_MOTOR_ID 0x02
#define RIGHT_PHI4_MOTOR_ID       0x03
#define RIGHT_PHI1_MOTOR_ID       0x04

/* Compatibility aliases for low-level motor code and older notes. */
#define LEFT_HIP_MOTOR_ID         LEFT_MIRROR_PHI4_MOTOR_ID
#define LEFT_KNEE_MOTOR_ID        LEFT_MIRROR_PHI1_MOTOR_ID
#define RIGHT_HIP_MOTOR_ID        RIGHT_PHI4_MOTOR_ID
#define RIGHT_KNEE_MOTOR_ID       RIGHT_PHI1_MOTOR_ID

/* Damiaotech 6215 wheel motors */
#define LEFT_WHEEL_MOTOR_ID     0x05
#define RIGHT_WHEEL_MOTOR_ID    0x06

#endif /* HARDWARE_CONFIG_H */
