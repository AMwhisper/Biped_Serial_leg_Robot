/**
 * @file dbus_receiver.c
 * @brief DJI NDJ6/DR16 DBus remote decoder.
 */

#include "dbus_receiver.h"
#include "hardware_config.h"
#include <string.h>

#define DBUS_CH_CENTER       1024
#define DBUS_CH_MAX_ABS      660.0f
#define DBUS_TIMEOUT_MS      300U
#define DBUS_DEAD_BAND       20
#define DBUS_MAX_VX          MAX_LINEAR_VELOCITY
#define DBUS_MAX_YAW_RATE    MAX_ANGULAR_VELOCITY
#define DBUS_MIN_LEG_LENGTH  0.07f
#define DBUS_MAX_LEG_LENGTH  0.14f
#define DBUS_NOM_LEG_LENGTH  0.11f

static dbus_state_t state;
static uint8_t frame_buf[DBUS_FRAME_LEN];
static uint8_t frame_pos;

extern uint32_t HAL_GetTick(void);

static float clampf(float value, float lo, float hi) {
    if (value < lo) {
        return lo;
    }
    if (value > hi) {
        return hi;
    }
    return value;
}

static float normalize_channel(int16_t raw) {
    if (raw > -DBUS_DEAD_BAND && raw < DBUS_DEAD_BAND) {
        return 0.0f;
    }
    return clampf((float)raw / DBUS_CH_MAX_ABS, -1.0f, 1.0f);
}

static bool switch_valid(dbus_switch_t sw) {
    return sw == DBUS_SW_UP || sw == DBUS_SW_MID || sw == DBUS_SW_DOWN;
}

void dbus_init(void) {
    memset(&state, 0, sizeof(state));
    memset(frame_buf, 0, sizeof(frame_buf));
    frame_pos = 0;
    state.sw_l = DBUS_SW_UNKNOWN;
    state.sw_r = DBUS_SW_UNKNOWN;
}

bool dbus_parse_frame(const uint8_t frame[DBUS_FRAME_LEN]) {
    if (frame == 0) {
        return false;
    }

    dbus_state_t next = state;
    next.ch[0] = (int16_t)(((frame[0] | (frame[1] << 8)) & 0x07FF) - DBUS_CH_CENTER);
    next.ch[1] = (int16_t)((((frame[1] >> 3) | (frame[2] << 5)) & 0x07FF) - DBUS_CH_CENTER);
    next.ch[2] = (int16_t)((((frame[2] >> 6) | (frame[3] << 2) | (frame[4] << 10)) & 0x07FF) - DBUS_CH_CENTER);
    next.ch[3] = (int16_t)((((frame[4] >> 1) | (frame[5] << 7)) & 0x07FF) - DBUS_CH_CENTER);
    next.sw_l = (dbus_switch_t)(((frame[5] >> 4) & 0x000C) >> 2);
    next.sw_r = (dbus_switch_t)((frame[5] >> 4) & 0x0003);
    next.mouse_x = (int16_t)(frame[6] | (frame[7] << 8));
    next.mouse_y = (int16_t)(frame[8] | (frame[9] << 8));
    next.mouse_z = (int16_t)(frame[10] | (frame[11] << 8));
    next.mouse_l = frame[12];
    next.mouse_r = frame[13];
    next.key = (uint16_t)(frame[14] | (frame[15] << 8));
    next.ch[4] = (int16_t)(((frame[16] | (frame[17] << 8)) & 0x07FF) - DBUS_CH_CENTER);

    if (!switch_valid(next.sw_l) || !switch_valid(next.sw_r)) {
        state.online = false;
        return false;
    }

    next.online = true;
    next.last_update_ms = HAL_GetTick();
    state = next;
    return true;
}

bool dbus_rx_byte(uint8_t byte) {
    frame_buf[frame_pos++] = byte;
    if (frame_pos < DBUS_FRAME_LEN) {
        return false;
    }

    frame_pos = 0;
    return dbus_parse_frame(frame_buf);
}

void dbus_update_timeout(uint32_t now_ms) {
    if (state.online && (now_ms - state.last_update_ms > DBUS_TIMEOUT_MS)) {
        state.online = false;
    }
}

const dbus_state_t *dbus_get_state(void) {
    return &state;
}

dbus_motion_cmd_t dbus_get_motion_cmd(void) {
    dbus_motion_cmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.leg_length = DBUS_NOM_LEG_LENGTH;

    if (!state.online) {
        cmd.emergency_stop = true;
        return cmd;
    }

    cmd.enable = (state.sw_r == DBUS_SW_UP || state.sw_r == DBUS_SW_MID);
    cmd.emergency_stop = (state.sw_r == DBUS_SW_DOWN);

    float vx = normalize_channel(state.ch[1]);
    float yaw = normalize_channel(state.ch[0]);
    float height = normalize_channel(state.ch[2]);

    cmd.vx = vx * DBUS_MAX_VX;
    cmd.yaw_rate = yaw * DBUS_MAX_YAW_RATE;
    cmd.leg_length = DBUS_NOM_LEG_LENGTH +
                     height * (DBUS_MAX_LEG_LENGTH - DBUS_MIN_LEG_LENGTH) * 0.5f;
    cmd.leg_length = clampf(cmd.leg_length, DBUS_MIN_LEG_LENGTH, DBUS_MAX_LEG_LENGTH);

    return cmd;
}
