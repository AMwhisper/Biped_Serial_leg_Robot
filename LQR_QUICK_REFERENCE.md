# LQR控制器 - 快速参考卡

## 文件结构
```
robomaster-c/
├── software/
│   ├── include/
│   │   ├── lqr_controller.h     ← LQR控制核心
│   │   ├── lqr_gains.h          ← 增益矩阵计算
│   │   ├── flight_controller.h  ← 飞行阶段控制
│   │   └── lqr_system.h         ← 系统集成
│   └── src/
│       ├── lqr_controller.c
│       ├── lqr_gains.c
│       ├── flight_controller.c
│       ├── lqr_system.c
│       └── test_lqr.c           ← 11个单元测试
└── docs/
    └── LQR_INTEGRATION.md       ← 完整指南
```

## 核心概念

### 系统模型
```
状态 x[6] = [θ, θ̇, x, ẋ, φ, φ̇]
控制 u[2] = [T, Tₚ]
```

### 控制律
```
u = -K·x  (反馈稳定性调节)
```

### 采样周期
```
Ts = 5ms (200 Hz)
```

## 最小化集成代码

```c
#include "lqr_system.h"

int main(void) {
    // 1. 初始化
    lqr_system_t lqr_sys;
    lqr_system_init(&lqr_sys, 0.07f);  // L0 = 0.07m
    
    while (1) {
        // 2. 读取传感器状态
        lqr_state_t state;
        state.theta = get_pitch();
        state.dtheta = get_pitch_rate();
        state.x = get_position();
        state.dx = get_velocity();
        state.phi = get_leg_angle();
        state.dphi = get_leg_rate();
        
        // 3. 更新控制器
        lqr_system_update(&lqr_sys, &state);
        
        // 4. 获取控制输出
        float T_body, T_leg;
        lqr_system_get_output(&lqr_sys, &T_body, &T_leg);
        
        // 5. 发送电机命令
        motor_set_torque(1, T_body);  // 身体转矩
        motor_set_torque(2, T_leg);   // 腿部转矩
        
        usleep(5000);  // 5ms采样
    }
}
```

## API快速查询

### 初始化
```c
lqr_system_init(&sys, leg_length)           // 初始化系统
lqr_system_set_mode(&sys, mode)             // 设置控制模式
```

### 主循环
```c
lqr_system_update(&sys, &state)             // 更新控制器
lqr_system_get_output(&sys, &T_body, &T_leg) // 获取输出
```

### 跳跃控制
```c
lqr_system_execute_jump(&sys, &state, phase) // 执行跳跃
```

### 增益计算
```c
lqr_gains_compute(L0, K)                    // 计算K矩阵
lqr_gains_get_coeff(row, col, coeff)        // 获取系数
```

### 飞行控制
```c
flight_controller_update(&fc, phase, ...)   // 更新飞行控制
flight_controller_get_torques(&fc, &T1, &T2) // 获取转矩
flight_controller_is_flying(&fc)            // 检查是否飞行
```

## 控制模式

| 模式 | 用途 | 触发条件 |
|------|------|---------|
| IDLE | 无控制 | 启动/停止 |
| LQR_REGULATION | 稳定性调节 | 平衡控制 |
| LQR_TRACKING | 轨迹跟踪 | 路径规划 |
| FLIGHT_HOLD | 飞行保持 | 飞行阶段 |
| JUMP_SEQUENCE | 完整跳跃 | 跳跃命令 |

## 参数配置

```c
// LQR权重 (来自MATLAB)
Q = {1, 10, 100, 20, 1000, 1}     // 状态权重
R = {1, 1}                         // 控制权重

// 飞行控制参数 (来自flight_hold_params)
kp = 0.6                           // PD位置增益
kd = 0.03                          // PD速度增益
torque_limit = 0.08                // 转矩限制
```

## 常见操作

### 切换到LQR稳定性调节
```c
lqr_system_set_mode(&sys, CONTROL_MODE_LQR_REGULATION);
```

### 执行跳跃
```c
lqr_system_execute_jump(&sys, &state, FLIGHT_PHASE_STANCE);
lqr_system_execute_jump(&sys, &state, FLIGHT_PHASE_THRUST);
lqr_system_execute_jump(&sys, &state, FLIGHT_PHASE_FLIGHT);
lqr_system_execute_jump(&sys, &state, FLIGHT_PHASE_LANDING);
```

### 获取系统状态
```c
float error_norm = lqr_get_error_norm(&sys.lqr);
bool in_flight = flight_controller_is_flying(&sys.flight);
```

### 调试输出
```c
uart_printf("Control: T=%.3f Tp=%.3f\n", T_body, T_leg);
uart_printf("State error: %.6f\n", lqr_get_error_norm(&sys.lqr));
```

## 性能数据

| 指标 | 值 |
|------|-----|
| 增益计算 | O(1) |
| 控制律执行 | O(12) |
| 采样周期 | 5ms |
| 内存占用 | <1KB |
| 代码行数 | ~1000 |

## 测试运行

```c
run_lqr_tests();  // 执行11个单元测试

// 预期输出:
// [PASS] LQR Gain Computation
// [PASS] LQR Controller Init
// [PASS] LQR Control Computation
// ...
// [PASS] LQR Gain Coefficients
// All tests passed!
```

## 故障排查

| 问题 | 原因 | 解决 |
|------|------|------|
| 编译错误 | 缺少include目录 | 检查CMakeLists.txt |
| 转矩过大 | 增益太高 | 降低Q权重 |
| 无响应 | 增益过低 | 提高Q权重 |
| 飞行失败 | 转矩限制 | 增加torque_limit |

## 与MATLAB的对应

```
MATLAB                    C
sys_calc.m         →  lqr_gains.c      (系数表)
lqr_k.m            →  lqr_gains_compute() (多项式)
flight_joint_hold  →  flight_controller.c (PD控制)
```

## MATLAB系数示例 (L0=0.07m)

```
K = [
    -0.0714   0.0612  9.9145  0.0081 39.003  -0.565
     0.0279  0.0108  -32.289  4.4054  -11.605 0.0713
]
```

## 编译指令

```bash
# CMake编译
cd robomaster-c/software
mkdir build && cd build
cmake ..
make

# 或PlatformIO
platformio run -e robomaster_c
```

## 关键函数签名

```c
// 增益计算
void lqr_gains_compute(float L0, float K[12]);

// LQR控制
bool lqr_compute_control(lqr_controller_t *c);
void lqr_get_control(lqr_controller_t *c, float torque[2]);
float lqr_get_error_norm(const lqr_controller_t *c);

// 飞行控制
bool flight_controller_update(flight_joint_controller_t *c,
    flight_phase_t phase, float phi1, float phi4,
    float dphi1, float dphi4);

// 系统集成
bool lqr_system_update(lqr_system_t *sys, const lqr_state_t *state);
void lqr_system_get_output(lqr_system_t *sys, 
    float *T_body, float *T_leg);
bool lqr_system_execute_jump(lqr_system_t *sys,
    const lqr_state_t *state, flight_phase_t phase);
```

## 更多信息

- 详细指南: docs/LQR_INTEGRATION.md
- API参考: docs/API_REFERENCE.md
- MATLAB源码: ../matlab/sys_calc.m
- 单元测试: src/test_lqr.c

---
**快速参考卡** | 2026-05-09 | v1.0
