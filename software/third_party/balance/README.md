# Balance Third-Party Import

Source imported from `https://github.com/Rian-Night/Balance`.

Imported pieces:

- `Usr/Driver_Gyroscope.*`
- `Usr/Driver_Filter.*`
- `Usr/config.h`
- `Usr/macro.h`
- `FML/bmi088/*`
- `FML/ekf/*`

The BMI088 driver and EKF core are added to the Keil build through the
`BalanceThirdParty` group. The original `Usr/Driver_Gyroscope.*` wrapper is kept
as reference only because it depends on the original project's timing macros and
global task structure.

Current integration point:

- `software/include/balance_feedback.h`
- `software/src/balance_feedback.c`

The adapter already feeds wheel odometry into the LQR loop. The active
BMI088/EKF bridge is `software/src/attitude_imu.c`, which provides pitch and
pitch-rate through `balance_feedback_set_attitude()`.
