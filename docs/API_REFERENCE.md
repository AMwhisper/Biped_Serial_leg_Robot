# API 参考文档

## 电机控制 API (motor_controller.h)

### 初始化
```c
bool motor_controller_init(void);
```
初始化电机控制系统。必须在程序启动时调用。

### 电机使能/禁用
```c
bool motor_enable(uint8_t motor_id);
bool motor_disable(uint8_t motor_id);
```
启用或禁用指定ID的电机。

### 力矩控制
```c
bool motor_set_torque(uint8_t motor_id, float torque);
```
设置电机输出力矩。参数单位为Nm。
- DM6215: ±12 Nm
- DM4310: ±5 Nm

### 速度控制
```c
bool motor_set_speed(uint8_t motor_id, float speed);
```
设置电机转速。参数单位为rad/s。
- 最大速度: 130 rad/s

### 位置控制
```c
bool motor_set_position(uint8_t motor_id, float position);
```
设置电机位置。参数单位为弧度(rad)。

### 状态查询
```c
motor_t* motor_get(uint8_t motor_id);
bool motor_get_status(uint8_t motor_id, motor_t *motor);
bool motor_update(void);
```
获取电机状态信息。motor_update() 应在主循环中持续调用。

---

## 腿部控制 API (leg_controller.h)

### 初始化
```c
bool leg_controller_init(void);
```
初始化腿部控制系统。

### 位置控制
```c
bool leg_set_position(leg_id_t leg_id, const leg_position_t *pos);
```
设置腿的目标位置。
- leg_id: LEG_FRONT_LEFT(0), LEG_FRONT_RIGHT(1), LEG_BACK_LEFT(2), LEG_BACK_RIGHT(3)
- pos: 包含x, y, z坐标的目标位置

### 速度控制
```c
bool leg_set_velocity(leg_id_t leg_id, const leg_velocity_t *vel);
```
设置腿的目标速度。

### 力矩控制
```c
bool leg_set_torques(leg_id_t leg_id, const float torques[3]);
```
直接设置腿的三个关节力矩。

### 状态查询
```c
bool leg_get_state(leg_id_t leg_id, leg_state_t *state);
```
获取腿的当前状态（位置、速度、力矩）。

### 运动学
```c
bool leg_forward_kinematics(const float angles[3], leg_position_t *position);
bool leg_inverse_kinematics(const leg_position_t *position, float angles[3]);
```
正逆向运动学计算。

---

## 运动规划 API (motion_planner.h)

### 初始化
```c
bool motion_planner_init(void);
```

### 运动命令
```c
bool motion_planner_execute(motion_cmd_t cmd, const motion_params_t *params);
```
执行运动命令。命令包括:
- MOTION_IDLE - 空闲
- MOTION_WALK_FORWARD - 向前走
- MOTION_WALK_BACKWARD - 向后走
- MOTION_TURN_LEFT - 左转
- MOTION_TURN_RIGHT - 右转
- MOTION_JUMP - 跳跃
- MOTION_DANCE - 舞蹈

### 停止运动
```c
bool motion_planner_stop(void);
```

---

## CAN总线 API (can_driver.h)

### 初始化
```c
bool can_driver_init(void);
void can_driver_deinit(void);
```

### 消息收发
```c
can_status_t can_send(const can_message_t *msg);
can_status_t can_receive(can_message_t *msg);
bool can_message_available(void);
```

### 配置
```c
bool can_set_filter(uint32_t id, uint32_t mask);
can_status_t can_get_status(void);
```

---

## 系统管理 API (system.h)

### 初始化
```c
bool system_init(void);
```
初始化整个系统（包括所有子模块）。

### 状态管理
```c
system_state_t system_get_state(void);
void system_set_state(system_state_t state);
```

### 系统更新
```c
bool system_update(void);
```
主系统更新函数，应在控制循环中调用。

### 其他
```c
uint32_t system_get_uptime(void);
uint32_t system_get_error(void);
void system_emergency_stop(void);
void system_shutdown(void);
```

---

## 数据记录 API (data_logger.h)

### 初始化
```c
bool data_logger_init(void);
```

### 日志记录
```c
bool data_logger_log_state(void);
```
记录当前系统状态到日志缓冲区。

### 查询日志
```c
uint32_t data_logger_get_count(void);
bool data_logger_get_entry(uint32_t index, log_entry_t *entry);
```

### 管理
```c
void data_logger_clear(void);
bool data_logger_export(const char *filename);
```

---

## UART通讯 API (uart_interface.h)

### 初始化
```c
bool uart_init(void);
```

### 收发数据
```c
uint32_t uart_send(const uint8_t *data, uint32_t len);
uint32_t uart_receive(uint8_t *data, uint32_t len);
uint32_t uart_available(void);
```

### 输出
```c
void uart_printf(const char *format, ...);
void uart_debug_msg(const char *msg);
```

---

## 数据结构

### motor_t
```c
typedef struct {
    uint8_t motor_id;
    uint8_t motor_type;      // 0x01: DM6215, 0x02: DM4310
    float position;          // rad
    float velocity;          // rad/s
    float torque;           // Nm
    float target_torque;    // Nm
    float target_velocity;  // rad/s
    float target_position;  // rad
    uint16_t temperature;   // °C
    bool enabled;
} motor_t;
```

### leg_position_t
```c
typedef struct {
    float x;  // m
    float y;  // m
    float z;  // m
} leg_position_t;
```

### leg_state_t
```c
typedef struct {
    leg_position_t position;
    leg_velocity_t velocity;
    float joint_angles[3];
    float joint_velocities[3];
    float joint_torques[3];
} leg_state_t;
```

### can_message_t
```c
typedef struct {
    uint32_t can_id;
    uint8_t dlc;
    uint8_t data[8];
    uint32_t timestamp;
} can_message_t;
```

---

## 使用示例

### 基本初始化
```c
#include "system.h"
#include "motor_controller.h"

int main(void) {
    SystemInit();
    system_init();
    
    while (1) {
        system_update();
    }
}
```

### 控制电机
```c
// 启用电机1
motor_enable(1);

// 设置力矩为5Nm
motor_set_torque(1, 5.0f);

// 查询状态
motor_t motor_status;
motor_get_status(1, &motor_status);
printf("Position: %.2f rad\n", motor_status.position);
```

### 腿部运动
```c
#include "leg_controller.h"

leg_position_t target = {0.1f, 0.0f, -0.2f};
leg_set_position(LEG_FRONT_LEFT, &target);
```

### 四足行走
```c
#include "motion_planner.h"

motion_params_t params = {
    .linear_velocity = 0.5f,
    .step_height = 0.05f,
    .step_length = 0.1f,
    .gait_period = 1.0f
};

motion_planner_execute(MOTION_WALK_FORWARD, &params);
```

---

最后更新: 2026-05-09
