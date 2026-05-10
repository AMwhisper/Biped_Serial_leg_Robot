/**
 * @file lqr_gains.h
 * @brief LQR gain calculation derived from MATLAB sys_calc.m
 * @author MATLAB Symbolic Math Toolbox (Auto-generated)
 * @date 2026-03-24
 * 
 * This file contains the LQR feedback gain matrix K as a function of leg length L0.
 * K is computed as a 3rd-order polynomial fit over the range L0 = [0.04, 0.14] m
 * 
 * System parameters:
 * - Wheel radius R = 0.02 m
 * - Body mass M = 0.325 kg
 * - Leg mass (each) mp = 0.0248 kg
 * - Wheel inertia Iw = 5.02655e-5 kg·m²
 * - Leg inertia Ip ≈ 7.33158e-6 kg·m² (per leg)
 * - Motor inertia Im = 0.000338542 kg·m²
 * - Gravity g = 9.8 m/s²
 */

#ifndef LQR_GAINS_H
#define LQR_GAINS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Calculate LQR feedback gain matrix K(L0)
 * 
 * The gain matrix is computed as a 3rd-order polynomial in L0:
 * K[i,j] = c3*L0^3 + c2*L0^2 + c1*L0 + c0
 * 
 * @param L0 Leg length in meters [0.04, 0.14]
 * @param K Output gain matrix (2x6 array in row-major order)
 */
void lqr_gains_compute(float L0, float K[12]);

/**
 * @brief Get specific gain coefficient (for testing/debugging)
 * @param row Row index (0-1)
 * @param col Column index (0-5)
 * @param coeff Coefficient index (0=L0^3, 1=L0^2, 2=L0, 3=const)
 * @return Coefficient value
 */
float lqr_gains_get_coeff(uint8_t row, uint8_t col, uint8_t coeff);

#ifdef __cplusplus
}
#endif

#endif /* LQR_GAINS_H */
