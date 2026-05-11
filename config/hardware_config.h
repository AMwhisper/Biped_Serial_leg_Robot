#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

/* ==================== RoboMaster C Type Hardware Configuration ==================== */

/* CAN Interface Configuration */
#define CAN_DEVICE              CAN2
#define CAN_BAUDRATE            100000       /* Set actual timing in bringup_config.h */
#define CAN_RX_PIN              GPIO_PIN_5   /* PB5, CAN2_RX */
#define CAN_TX_PIN              GPIO_PIN_6   /* PB6, CAN2_TX */
#define CAN_GPIO_PORT           GPIOB

/* BMI088 IMU on RoboMaster C Type board, imported from Balance CubeMX setup */
#define IMU_SPI_DEVICE          SPI1
#define IMU_SPI_SCK_PIN         GPIO_PIN_3   /* PB3, SPI1_SCK */
#define IMU_SPI_MISO_PIN        GPIO_PIN_4   /* PB4, SPI1_MISO */
#define IMU_SPI_MOSI_PIN        GPIO_PIN_7   /* PA7, SPI1_MOSI */
#define IMU_ACCEL_CS_PIN        GPIO_PIN_4   /* PA4 */
#define IMU_GYRO_CS_PIN         GPIO_PIN_0   /* PB0 */

/* Motor Update Frequency */
#define MOTOR_CONTROL_FREQ      500          /* Hz - Main control loop frequency */
#define MOTOR_FEEDBACK_TIMEOUT  100          /* ms - CAN feedback timeout */

/* LED Indicators */
#define LED_STATUS_PIN          GPIO_PIN_0
#define LED_ERROR_PIN           GPIO_PIN_1
#define LED_GPIO_PORT           GPIOB

/* Communication UART */
#define DEBUG_UART              UART1
#define DEBUG_UART_BAUDRATE     115200
#define DBUS_UART_BAUDRATE      100000       /* DJI DBus: 100 kbps, 8E1 */

/* ==================== Motor Hardware Mapping ==================== */

/* Damiaotech 4310 Joint Motors: two serial legs, two joints per leg */
#define LEFT_HIP_MOTOR_ID       0x01
#define LEFT_KNEE_MOTOR_ID      0x02
#define RIGHT_HIP_MOTOR_ID      0x03
#define RIGHT_KNEE_MOTOR_ID     0x04

/* Motor encoder angle to VMC model angle:
 *   phi_model = motor_position * SIGN + OFFSET
 *
 * Leg calibration:
 *   motor zero is the rearward maximum-leg-length pose.
 *   virtual leg lies on the x axis, with link l1 at +45 degrees.
 *   Left and right legs are mechanically symmetric.
 *   Right leg: ID3 positive torque makes l1 angle smaller, so SIGN is -1.
 *              ID4 positive torque extends the leg, matching positive phi4 near this pose.
 */
#define LEFT_HIP_MODEL_SIGN     -1.0f
#define LEFT_KNEE_MODEL_SIGN     1.0f
#define RIGHT_HIP_MODEL_SIGN    -1.0f
#define RIGHT_KNEE_MODEL_SIGN    1.0f
#define LEFT_HIP_MODEL_OFFSET    0.785398163f
#define LEFT_KNEE_MODEL_OFFSET  -0.785398163f
#define RIGHT_HIP_MODEL_OFFSET   0.785398163f
#define RIGHT_KNEE_MODEL_OFFSET -0.785398163f

/* Damiaotech 6215 Wheel Motors */
#define LEFT_WHEEL_MOTOR_ID     0x05
#define RIGHT_WHEEL_MOTOR_ID    0x06
#define LEFT_WHEEL_TORQUE_SIGN  1.0f
#define RIGHT_WHEEL_TORQUE_SIGN -1.0f

/* ==================== System Parameters ==================== */

/* Mechanical Parameters */
#define ROBOT_MASS              5.0f         /* kg - Total robot mass */
#define LEG_LENGTH              0.11f        /* m - Nominal half-link length used by MATLAB model */
#define WHEEL_RADIUS            0.05f        /* m - Wheel radius */
#define TRACK_WIDTH             0.25f        /* m - Distance between left and right wheels */

/* Control Parameters */
#define MAX_LINEAR_VELOCITY     1.0f         /* m/s */
#define MAX_ANGULAR_VELOCITY    2.0f         /* rad/s */
#define MAX_JOINT_ANGLE         3.14159f     /* rad - Maximum joint angle */
#define MIN_JOINT_ANGLE         -3.14159f    /* rad - Minimum joint angle */

/* Safety Limits */
#define EMERGENCY_STOP_TIMEOUT  1000         /* ms - Emergency stop trigger time */
#define MAX_MOTOR_TEMP          60           /* °C - Maximum motor temperature */
#define LOW_BATTERY_VOLTAGE     20.0f        /* V - Low battery warning threshold */

/* ==================== System State Bits ==================== */
#define SYS_STATE_INITIALIZED   0x01
#define SYS_STATE_MOTOR_OK      0x02
#define SYS_STATE_CAN_OK        0x04
#define SYS_STATE_EMERGENCY     0x08
#define SYS_STATE_LOW_BATTERY   0x10

#endif /* HARDWARE_CONFIG_H */
