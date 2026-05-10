/**
 * @file lqr_gains.c
 * @brief LQR gain polynomial generated from matlab/lqr_k.m.
 */

#include "lqr_gains.h"

static const float K_COEFFS[2][6][4] = {
    {
        {3228.25952f, -834.713379f, -42.9759903f, 0.317834556f},
        {-412.332275f, -24.1315918f, 30.5160351f, -0.606049836f},
        {569.72406f, -160.626801f, -5.97862148f, 0.100342169f},
        {-49.1732292f, -12.402257f, 6.42428493f, -0.102529436f},
        {1367.10657f, -280.185211f, -28.4629879f, 0.547008395f},
        {29.4460297f, -88.8476868f, 20.9805832f, -0.245160371f},
    },
    {
        {1091.43787f, -204.988907f, -31.0228348f, 0.427929342f},
        {95.2390289f, -102.052139f, 20.9717579f, -0.259944558f},
        {-221.810303f, 265.138153f, -108.561981f, 17.995327f},
        {730.836121f, -363.537292f, 71.288269f, 18.7268333f},
        {-37.9140739f, 17.8434963f, -4.83956623f, 0.870167911f},
        {13.0611944f, -7.26515818f, 1.76295102f, 0.93143183f},
    },
};

static float eval_poly3(const float coeffs[4], float x) {
    return ((coeffs[0] * x + coeffs[1]) * x + coeffs[2]) * x + coeffs[3];
}

void lqr_gains_compute(float L0, float K[12]) {
    if (K == 0) {
        return;
    }

    if (L0 < 0.04f) {
        L0 = 0.04f;
    } else if (L0 > 0.14f) {
        L0 = 0.14f;
    }

    for (uint8_t row = 0; row < 2; ++row) {
        for (uint8_t col = 0; col < 6; ++col) {
            K[row * 6U + col] = eval_poly3(K_COEFFS[row][col], L0);
        }
    }
}

float lqr_gains_get_coeff(uint8_t row, uint8_t col, uint8_t coeff) {
    if (row >= 2U || col >= 6U || coeff >= 4U) {
        return 0.0f;
    }
    return K_COEFFS[row][col][coeff];
}
