/**
 * @file dbus_receiver.h
 * @brief DJI NDJ6/DR16 DBus remote decoder.
 */

#ifndef DBUS_RECEIVER_H
#define DBUS_RECEIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define DBUS_FRAME_LEN 18U

typedef enum {
    DBUS_SW_UNKNOWN = 0,
    DBUS_SW_UP = 1,
    DBUS_SW_MID = 3,
    DBUS_SW_DOWN = 2,
} dbus_switch_t;

typedef struct {
    int16_t ch[5];          /* centered channels, roughly [-660, 660] */
    dbus_switch_t sw_l;
    dbus_switch_t sw_r;
    int16_t mouse_x;
    int16_t mouse_y;
    int16_t mouse_z;
    uint8_t mouse_l;
    uint8_t mouse_r;
    uint16_t key;
    bool online;
    uint32_t last_update_ms;
} dbus_state_t;

typedef struct {
    float vx;               /* m/s, forward positive */
    float yaw_rate;         /* rad/s, left positive */
    float leg_length;       /* m */
    bool enable;
    bool emergency_stop;
} dbus_motion_cmd_t;

void dbus_init(void);
bool dbus_parse_frame(const uint8_t frame[DBUS_FRAME_LEN]);
bool dbus_rx_byte(uint8_t byte);
void dbus_update_timeout(uint32_t now_ms);
const dbus_state_t *dbus_get_state(void);
dbus_motion_cmd_t dbus_get_motion_cmd(void);

#ifdef __cplusplus
}
#endif

#endif /* DBUS_RECEIVER_H */
