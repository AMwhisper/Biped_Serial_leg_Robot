# First Flash Checklist

All bring-up switches are in `../../config/bringup_config.h`.

## 1. First Flash, No Motor Power

Use these settings:

```c
#define BRINGUP_CAN_1MBPS              0
#define BRINGUP_ENABLE_REMOTE_CONTROL  0
#define BRINGUP_ENABLE_BALANCE_CONTROL 0
#define BRINGUP_MOTOR_TEST_ENABLE      0
```

Power only the RoboMaster C board and flash the program. This validates that the firmware boots without enabling motors.

## 2. Confirm CAN Bitrate

If your motors are configured for DJI/Damiaotech default 1 Mbps, set:

```c
#define BRINGUP_CAN_1MBPS 1
```

If they match your old CubeMX CAN2 settings, keep:

```c
#define BRINGUP_CAN_1MBPS 0
```

Your old CAN2 settings are PB5/PB6, prescaler 42, BS1 4TQ, BS2 5TQ, which is 100 kbps when APB1 is 42 MHz.

## 3. Single Motor Test

Put the robot on a stand and test one motor with tiny torque:

```c
#define BRINGUP_MOTOR_TEST_ENABLE 1
#define BRINGUP_TEST_MOTOR_ID     1
#define BRINGUP_TEST_TORQUE_NM    0.05f
```

Rebuild and flash. The selected motor will receive alternating small torque every second.

Motor IDs:

- `1-4`: DM4310 joint motors
- `5-6`: DM6215 wheel motors

## 4. Remote Control

After CAN feedback and single-motor tests are correct, enable remote parsing:

```c
#define BRINGUP_ENABLE_REMOTE_CONTROL 1
```

Do not enable balance until IMU/pose feedback is real.

## 5. Balance Control

Only enable after the robot is mechanically constrained and sensor feedback is wired:

```c
#define BRINGUP_ENABLE_BALANCE_CONTROL 1
```
