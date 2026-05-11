/**
 * @file attitude_imu.c
 * @brief Uses the imported Balance BMI088 driver and EKF for pitch feedback.
 */

#include "attitude_imu.h"
#include "balance_feedback.h"
#include "BMI088driver.h"
#include "orientation_ekf.h"
#include <math.h>
#include <string.h>

#define DEG_TO_RAD 0.017453292519943295f
#define G_TO_MS2   9.80665f

static bool imu_ready;
static uint32_t last_update_ms;

bool attitude_imu_init(void) {
    imu_ready = false;
    last_update_ms = 0U;

    OrientationEKF_Init(&ekf);
    if (BMI088_init() != BMI088_NO_ERROR) {
        balance_feedback_set_attitude(0.0f, 0.0f, false);
        return false;
    }

    imu_ready = true;
    return true;
}

bool attitude_imu_update(uint32_t now_ms) {
    if (!imu_ready) {
        balance_feedback_set_attitude(0.0f, 0.0f, false);
        return false;
    }

    float gyro_dps[3] = {0.0f};
    float accel_g[3] = {0.0f};
    float temperature = 0.0f;
    float quat[4] = {0.0f};
    float rpy_deg[3] = {0.0f};

    BMI088_read(gyro_dps, accel_g, &temperature);

    Vector3d gyro_rad_s;
    Vector3d accel_ms2;
    Vector3d_set(&gyro_rad_s,
                 gyro_dps[0] * DEG_TO_RAD,
                 gyro_dps[1] * DEG_TO_RAD,
                 gyro_dps[2] * DEG_TO_RAD);
    Vector3d_set(&accel_ms2,
                 accel_g[0] * G_TO_MS2,
                 accel_g[1] * G_TO_MS2,
                 accel_g[2] * G_TO_MS2);

    uint64_t timestamp_ns = (uint64_t)now_ms * 1000000ULL;
    if (last_update_ms == 0U || timestamp_ns == 0ULL) {
        timestamp_ns = 1ULL;
    }
    last_update_ms = now_ms;

    OrientationEKF_UpdateGyro(&ekf, &gyro_rad_s, timestamp_ns);
    OrientationEKF_UpdateAccel(&ekf, &accel_ms2, timestamp_ns);
    OrientationEKF_GetQuaternion(&ekf, quat);
    OrientationEKF_GetRPY(quat, rpy_deg);

    balance_feedback_set_attitude(rpy_deg[1] * DEG_TO_RAD, gyro_rad_s.y, true);
    (void)temperature;
    return true;
}

bool attitude_imu_is_ready(void) {
    return imu_ready;
}
