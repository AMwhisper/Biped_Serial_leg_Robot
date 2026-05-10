# LQR控制器集成指南

## 概览

本项目集成了基于MATLAB设计的LQR（线性二次调节器）控制器。LQR用于四足机器人的以下功能：

1. **稳定性调节** - 调节身体姿态到平衡状态
2. **推力生成** - 跳跃序列中的推力控制
3. **飞行关节保持** - 飞行阶段的腿部姿态维持

## 系统模型

### 状态空间
```
状态向量 x = [θ, θ̇, x, ẋ, φ, φ̇]ᵀ

θ:  身体俯仰角 (rad)
θ̇:  身体角速度 (rad/s)
x:  水平位置 (m)
ẋ:  水平速度 (m/s)
φ:  腿部角度 (rad)
φ̇:  腿部角速度 (rad/s)
```

### 控制输入
```
u = [T, Tₚ]ᵀ

T:   身体转矩 (Nm)
Tₚ:  腿部转矩 (Nm)
```

## 控制器架构

```
┌─────────────────────────────────────────────────┐
│           LQR System Manager                    │
│                                                 │
│  ┌──────────────────────────────────────────┐  │
│  │ Control Mode Selection                   │  │
│  │ - LQR Regulation                         │  │
│  │ - LQR Tracking                           │  │
│  │ - Flight Hold                            │  │
│  │ - Jump Sequence                          │  │
│  └──────────────────────────────────────────┘  │
│           │                                    │
│           ├─ LQR Controller ┐                  │
│           │                 ├─ u = -K·x        │
│           └─ Flight Controller ┘               │
│                                                 │
└─────────────────────────────────────────────────┘
```

## 关键模块

### lqr_controller.h/c
- **功能**: LQR反馈控制核心
- **接口**:
  - `lqr_controller_init()` - 初始化
  - `lqr_compute_control()` - 计算控制输入
  - `lqr_controller_set_state()` - 更新系统状态

### lqr_gains.h/c
- **功能**: 增益矩阵计算
- **来源**: MATLAB sys_calc.m 自动生成
- **范围**: L0 ∈ [0.04, 0.14] m
- **形式**: 3次多项式拟合

### flight_controller.h/c
- **功能**: 飞行阶段关节保持控制
- **来源**: MATLAB flight_joint_hold.m
- **控制律**: PD反馈

### lqr_system.h/c
- **功能**: 整体系统集成
- **支持的模式**:
  - CONTROL_MODE_LQR_REGULATION
  - CONTROL_MODE_LQR_TRACKING
  - CONTROL_MODE_FLIGHT_HOLD
  - CONTROL_MODE_JUMP_SEQUENCE

## 使用示例

### 初始化
```c
#include "lqr_system.h"

lqr_system_t lqr_sys;
float leg_length = 0.07f;  // 7cm

lqr_system_init(&lqr_sys, leg_length);
lqr_system_set_mode(&lqr_sys, CONTROL_MODE_LQR_REGULATION);
```

### 主控制循环
```c
while (1) {
    // 读取传感器
    lqr_state_t state;
    state.theta = read_body_angle();
    state.dtheta = read_body_velocity();
    state.x = read_position();
    state.dx = read_velocity();
    state.phi = read_leg_angle();
    state.dphi = read_leg_velocity();
    
    // 更新LQR系统
    lqr_system_update(&lqr_sys, &state);
    
    // 获取控制输出
    float T_body, T_leg;
    lqr_system_get_output(&lqr_sys, &T_body, &T_leg);
    
    // 发送到电机
    motor_set_torque(BODY_MOTOR_ID, T_body);
    motor_set_torque(LEG_MOTOR_ID, T_leg);
    
    sleep(5);  // 5ms采样周期
}
```

### 跳跃控制序列
```c
// 跳跃序列中的LQR+飞行控制
lqr_system_execute_jump(&lqr_sys, &state, FLIGHT_PHASE_STANCE);  // 准备
lqr_system_execute_jump(&lqr_sys, &state, FLIGHT_PHASE_THRUST);  // 推力
lqr_system_execute_jump(&lqr_sys, &state, FLIGHT_PHASE_FLIGHT);  // 飞行
lqr_system_execute_jump(&lqr_sys, &state, FLIGHT_PHASE_LANDING); // 着陆
```

## 参数配置

### LQR权重矩阵

Q矩阵（状态权重，来自MATLAB sys_calc.m）：
```
Q = diag([1, 10, 100, 20, 1000, 1])

权重解释:
- θ:    1    (低优先级)
- θ̇:   10   (中等)
- x:   100   (高 - 水平稳定性)
- ẋ:    20   (中高)
- φ:  1000   (最高 - 腿部配置)
- φ̇:    1    (低)
```

R矩阵（控制输入权重）：
```
R = diag([1, 1])  (平衡的控制成本)
```

### 可调参数

修改 `flight_hold_params.m` 中的参数：
```c
float_params_t params = {
    .kp = 0.6f,              // 位置增益
    .kd = 0.03f,             // 速度增益
    .torque_limit = 0.08f    // 转矩限制
};
```

## 性能指标

### 计算复杂度
- **LQR增益计算**: 3次多项式求值 → O(1)
- **控制律执行**: 矩阵-向量乘积 2×6 → O(12)
- **采样周期**: 5ms (200 Hz)

### 内存使用
- 增益矩阵: 2×6 × 4字节 = 48字节
- 系统状态: ~200字节
- 总计: <1 KB

## 验证测试

运行单元测试验证实现：
```c
run_lqr_tests();  // 执行11个LQR单元测试
```

测试覆盖：
- ✓ 增益矩阵计算
- ✓ 控制器初始化
- ✓ 控制量计算
- ✓ 饱和限制
- ✓ 错误范数计算
- ✓ 飞行相位转换
- ✓ 系统集成
- ✓ 跳跃序列执行

## 调试技巧

### 查看增益矩阵
```c
float K[12];
lqr_gains_compute(0.07f, K);  // 计算L0=0.07m时的增益

for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 6; j++) {
        printf("K[%d][%d] = %.6f\n", i, j, K[i*6+j]);
    }
}
```

### 监控控制量
```c
float T[2];
lqr_get_control(&controller, T);
uart_printf("Body torque: %.3f Nm, Leg torque: %.3f Nm\n", T[0], T[1]);
```

### 检查系统状态
```c
float error = lqr_get_error_norm(&controller);
uart_printf("State error norm: %.6f\n", error);
```

## 与MATLAB的对应关系

| MATLAB文件 | C模块 | 映射 |
|-----------|------|------|
| sys_calc.m | lqr_gains.c | 增益系数表 |
| lqr_k.m | lqr_gains_compute() | 多项式求值 |
| flight_joint_hold.m | flight_controller.c | PD控制律 |
| flight_hold_params.m | flight_params_t | 参数结构 |

## 扩展建议

1. **自适应增益** - 根据实时估计的腿长调整增益
2. **参考轨迹跟踪** - 添加期望轨迹和积分项
3. **鲁棒控制** - 考虑模型不确定性
4. **学习** - 在线参数优化

## 参考资源

- MATLAB文件: ../matlab/sys_calc.m, lqr_k.m
- 论文: [相关控制论文]
- 标准: IEEE Control Systems Magazine

---

**最后更新**: 2026-05-09  
**版本**: 1.0.0
