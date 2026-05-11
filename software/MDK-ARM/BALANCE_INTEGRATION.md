# Balance Repository Integration

Source repository:

- `https://github.com/Rian-Night/Balance`

Imported source snapshot:

- `../third_party/balance/Usr`
- `../third_party/balance/FML/bmi088`
- `../third_party/balance/FML/ekf`

The imported files provide the RoboMaster C board BMI088 and orientation EKF
reference implementation. The BMI088 driver and EKF core are now added to the
Keil build through the `BalanceThirdParty` group. The original Balance
`Usr/Driver_Gyroscope.*` wrapper is kept as reference only.

Board SPI mapping imported from the Balance CubeMX setup:

- PB3: SPI1_SCK
- PB4: SPI1_MISO
- PA7: SPI1_MOSI
- PA4: BMI088 accel CS
- PB0: BMI088 gyro CS

Active integration in this project:

- `../include/balance_feedback.h`
- `../src/balance_feedback.c`
- `../include/attitude_imu.h`
- `../src/attitude_imu.c`

`balance_feedback` is now part of the Keil project. It estimates:

- `x` and `dx` from the left/right DM6215 wheel feedback.
- `theta` and `dtheta` from an external attitude provider.

`attitude_imu.c` calls:

```c
balance_feedback_set_attitude(theta_rad, dtheta_rad_s, true);
```

The mapping is:

```c
theta_rad = rpy_deg[1] * DEG_TO_RAD;
dtheta_rad_s = gyro_rad_s.y;
```

Until valid attitude is provided, `main.c` disables balance output instead of
running LQR with zero feedback.
