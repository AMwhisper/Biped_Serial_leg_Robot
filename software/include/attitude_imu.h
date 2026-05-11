/**
 * @file attitude_imu.h
 * @brief BMI088 + EKF attitude provider for the balance feedback adapter.
 */

#ifndef ATTITUDE_IMU_H
#define ATTITUDE_IMU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

bool attitude_imu_init(void);
bool attitude_imu_update(uint32_t now_ms);
bool attitude_imu_is_ready(void);

#ifdef __cplusplus
}
#endif

#endif /* ATTITUDE_IMU_H */
