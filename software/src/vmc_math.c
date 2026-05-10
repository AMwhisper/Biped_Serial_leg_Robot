/**
 * @file vmc_math.c
 * @brief MATLAB-generated five-bar leg kinematics and force conversion.
 */

#include "vmc_math.h"
#include <math.h>

static float safe_sqrtf(float x) {
    return sqrtf((x > 0.0f) ? x : 0.0f);
}

void vmc_leg_pos(float phi1, float phi4, float pos[2]) {
    float t2 = cosf(phi1);
    float t3 = cosf(phi4);
    float t4 = sinf(phi1);
    float t5 = sinf(phi4);
    float t6 = t2 * 0.11f;
    float t8 = t4 * 0.11f;
    float t12 = t2 * 0.02904f;
    float t13 = t3 * 0.02904f;
    float t14 = t4 * 0.02904f;
    float t15 = t5 * 0.02904f;
    float dx = t6 - t3 * 0.11f;
    float dz = t8 - t5 * 0.11f;
    float ax = t12 - t13;
    float az = t14 - t15;
    float d2 = dx * dx + dz * dz;
    float beta = atanf(1.0f / ((t13 - t12) + d2) *
                       ((t15 - t14) + safe_sqrtf((ax * ax + az * az) - d2 * d2))) *
                 2.0f;
    float y = t8 + sinf(beta) * 0.132f;
    float x = t6 + cosf(beta) * 0.132f;

    pos[0] = safe_sqrtf(y * y + x * x);
    pos[1] = atan2f(y, x);
}

void vmc_leg_spd(float dphi1, float dphi4, float phi1, float phi4, float spd[2]) {
    float t2 = cosf(phi1);
    float t3 = cosf(phi4);
    float t4 = sinf(phi1);
    float t5 = sinf(phi4);
    float t10_tmp = t2 * 0.11f;
    float t12_tmp = t4 * 0.11f;
    float t21 = t2 * 0.02904f;
    float t22 = t3 * 0.02904f;
    float t23 = t4 * 0.02904f;
    float t24 = t5 * 0.02904f;
    float t29 = t10_tmp - t3 * 0.11f;
    float t30 = t12_tmp - t5 * 0.11f;
    float t33 = t21 - t22;
    float t34 = t23 - t24;
    float t35 = t4 * t29 * 0.22f;
    float t36 = t5 * t29 * 0.22f;
    float t37 = t2 * t30 * 0.22f;
    float t38 = t3 * t30 * 0.22f;
    float t51 = t29 * t29 + t30 * t30;
    float t59 = 1.0f / ((t22 - t21) + t51);
    float t60 = t59 * t59;
    float t64 = safe_sqrtf((t33 * t33 + t34 * t34) - t51 * t51);
    t29 = 1.0f / t64;
    float t66 = (t24 - t23) + t64;
    float t73 = atanf(t59 * t66) * 2.0f;
    t30 = cosf(t73);
    t64 = sinf(t73);
    t73 = 1.0f / (t60 * (t66 * t66) + 1.0f);
    t23 = ((t23 + t37) - t35) * t60 * t66 +
          t59 * (t21 - t29 * ((t2 * t34 * 0.05808f - t4 * t33 * 0.05808f) +
                              t51 * (t35 - t37) * 2.0f) /
                           2.0f);
    t29 = ((t24 + t38) - t36) * t60 * t66 +
          t59 * (t22 - t29 * ((t3 * t34 * 0.05808f - t5 * t33 * 0.05808f) +
                              t51 * (t36 - t38) * 2.0f) /
                           2.0f);
    t2 = t10_tmp + t30 * 0.132f;
    t4 = t12_tmp + t64 * 0.132f;
    t66 = t30 * t73;
    t21 = t66 * t29;
    t64 *= t73;
    t51 = t64 * t29;
    t59 = t2 * t2;
    t60 = 1.0f / t2;
    t30 = t4 * t4 + t59;
    t29 = 1.0f / safe_sqrtf(t30);
    t73 = 1.0f / t30;
    t66 = t10_tmp - t66 * t23 * 0.264f;
    t30 = t12_tmp - t64 * t23 * 0.264f;
    spd[0] = dphi4 * t29 * (t4 * t21 * 0.528f - t2 * t51 * 0.528f) / 2.0f +
             dphi1 * t29 * (t4 * t66 * 2.0f - t2 * t30 * 2.0f) / 2.0f;
    t29 = t4 * (1.0f / t59);
    spd[1] = dphi1 * t59 * t73 * (t60 * t66 + t29 * t30) -
             dphi4 * t59 * t73 *
                 (t60 * (0.0f - t21 * 0.264f) - t29 * (t51 * 0.264f));
}

void vmc_leg_conv(float F, float Tp, float phi1, float phi4, float torque[2]) {
    float t4_tmp = cosf(phi1);
    float t5_tmp = cosf(phi4);
    float t6_tmp = sinf(phi1);
    float t7_tmp = sinf(phi4);
    float t16_tmp = t4_tmp * 0.11f;
    float t18_tmp = t6_tmp * 0.11f;
    float t33_tmp = t4_tmp * 0.02904f;
    float t34_tmp = t5_tmp * 0.02904f;
    float t35_tmp = t6_tmp * 0.02904f;
    float t36_tmp = t7_tmp * 0.02904f;
    float t49_tmp = t16_tmp - t5_tmp * 0.11f;
    float t50_tmp = t18_tmp - t7_tmp * 0.11f;
    float t57_tmp = t33_tmp - t34_tmp;
    float t58_tmp = t35_tmp - t36_tmp;
    float t185 = t6_tmp * t49_tmp * 0.22f;
    float t60_tmp = t7_tmp * t49_tmp * 0.22f;
    float t61_tmp = t4_tmp * t50_tmp * 0.22f;
    float t62_tmp = t5_tmp * t50_tmp * 0.22f;
    float t91_tmp = t49_tmp * t49_tmp + t50_tmp * t50_tmp;
    t49_tmp = 1.0f / ((t34_tmp - t33_tmp) + t91_tmp);
    t50_tmp = t49_tmp * t49_tmp;
    float t112 = safe_sqrtf((t57_tmp * t57_tmp + t58_tmp * t58_tmp) - t91_tmp * t91_tmp);
    float t114_tmp = 1.0f / t112;
    float t118_tmp = (t36_tmp - t35_tmp) + t112;
    float t130 = atanf(t49_tmp * t118_tmp) * 2.0f;
    float t133_tmp = cosf(t130);
    t130 = sinf(t130);
    float t141_tmp = 1.0f / (t50_tmp * (t118_tmp * t118_tmp) + 1.0f);
    t112 = ((t35_tmp + t61_tmp) - t185) * t50_tmp * t118_tmp +
           t49_tmp * (t33_tmp - t114_tmp *
                                    ((t4_tmp * t58_tmp * 0.05808f -
                                      t6_tmp * t57_tmp * 0.05808f) +
                                     t91_tmp * (t185 - t61_tmp) * 2.0f) /
                                    2.0f);
    t6_tmp = ((t36_tmp + t62_tmp) - t60_tmp) * t50_tmp * t118_tmp +
             t49_tmp * (t34_tmp - t114_tmp *
                                      ((t5_tmp * t58_tmp * 0.05808f -
                                        t7_tmp * t57_tmp * 0.05808f) +
                                       t91_tmp * (t60_tmp - t62_tmp) * 2.0f) /
                                      2.0f);
    t33_tmp = t16_tmp + t133_tmp * 0.132f;
    t35_tmp = t18_tmp + t130 * 0.132f;
    t114_tmp = t33_tmp * t33_tmp;
    t185 = 1.0f / t33_tmp;
    t118_tmp = t35_tmp * t35_tmp + t114_tmp;
    t4_tmp = t133_tmp * t141_tmp;
    t91_tmp = t16_tmp - t4_tmp * t112 * 0.264f;
    t130 *= t141_tmp;
    t50_tmp = t18_tmp - t130 * t112 * 0.264f;
    t112 = F * (1.0f / safe_sqrtf(t118_tmp));
    t49_tmp = Tp * t114_tmp * (1.0f / t118_tmp);
    t118_tmp = t35_tmp * (1.0f / t114_tmp);
    torque[0] = t112 * (t35_tmp * t91_tmp * 2.0f - t33_tmp * t50_tmp * 2.0f) / 2.0f +
                t49_tmp * (t185 * t91_tmp + t118_tmp * t50_tmp);
    torque[1] = t112 * (t4_tmp * t35_tmp * t6_tmp * 0.528f -
                        t130 * t33_tmp * t6_tmp * 0.528f) /
                    2.0f -
                t49_tmp * (t185 * (0.0f - t4_tmp * t6_tmp * 0.264f) -
                           t118_tmp * (t130 * t6_tmp * 0.264f));
}
