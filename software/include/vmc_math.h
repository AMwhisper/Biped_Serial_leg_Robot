/**
 * @file vmc_math.h
 * @brief MATLAB-generated five-bar leg kinematics and virtual-force mapping.
 */

#ifndef VMC_MATH_H
#define VMC_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

void vmc_leg_pos(float phi1, float phi4, float pos[2]);
void vmc_leg_spd(float dphi1, float dphi4, float phi1, float phi4, float spd[2]);
void vmc_leg_conv(float F, float Tp, float phi1, float phi4, float torque[2]);

#ifdef __cplusplus
}
#endif

#endif /* VMC_MATH_H */
