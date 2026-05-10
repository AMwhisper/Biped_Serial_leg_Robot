# RoboMaster C型开发板串联腿机器人

## 项目概述
基于RoboMaster C型开发板的轮腿机器人项目，采用达妙电机进行驱动。**已集成MATLAB设计的LQR控制器用于稳定性调节和跳跃控制**。

## 硬件配置
- **开发板**: RoboMaster C型开发板 (STM32F405RG)
- **关节电机**: 达妙6215（4个，用于腿部关节驱动）
- **驱动电机**: 达妙4310（4个，用于轮子驱动）
- **通信**: CAN总线 (1Mbps)
- **采样周期**: 5ms (200Hz)

## 项目结构
```
robomaster-c/
├── hardware/                    # 硬件设计文件
├── software/                    # 软件代码
│   ├── src/                    # 源代码
│   │   ├── motor_controller.c
│   │   ├── can_driver.c
│   │   ├── leg_controller.c
│   │   ├── motion_planner.c
│   │   ├── lqr_controller.c     # ★ LQR反馈控制
│   │   ├── lqr_gains.c          # ★ 增益矩阵计算
│   │   ├── flight_controller.c  # ★ 飞行相位控制
│   │   ├── lqr_system.c         # ★ 系统集成
│   │   └── test_lqr.c           # ★ 单元测试 (11个)
│   ├── include/                # 头文件
│   └── lib/                    # 库文件
├── config/                      # 配置文件
├── docs/
│   └── LQR_INTEGRATION.md       # 详细集成指南
├── LQR_INTEGRATION_REPORT.md    # 集成完成报告
├── LQR_QUICK_REFERENCE.md       # 快速参考卡
└── README.md                    # 本文件
```

## LQR控制器集成 (★新增)

### 集成内容
- ✓ LQR增益矩阵计算 (来自MATLAB lqr_k.m)
- ✓ 离散LQR反馈控制 (采样周期5ms)
- ✓ 飞行相位关节保持 (PD控制)
- ✓ 完整跳跃序列管理
- ✓ 11个单元测试

### 快速开始
```c
#include "lqr_system.h"

// 初始化 (L0=腿长)
lqr_system_t sys;
lqr_system_init(&sys, 0.07f);  // 7cm

// 主控制循环
while (1) {
    lqr_state_t state = read_sensors();
    lqr_system_update(&sys, &state);
    
    float T_body, T_leg;
    lqr_system_get_output(&sys, &T_body, &T_leg);
    motor_set_torque(1, T_body);
    motor_set_torque(2, T_leg);
    
    usleep(5000);  // 5ms
}
```

### 文档
- [LQR_INTEGRATION.md](docs/LQR_INTEGRATION.md) - 完整集成指南
- [LQR_INTEGRATION_REPORT.md](LQR_INTEGRATION_REPORT.md) - 集成报告 (文件清单、性能指标)
- [LQR_QUICK_REFERENCE.md](LQR_QUICK_REFERENCE.md) - 快速参考卡

### 系统参数

**状态空间** (6维):
- θ: 身体俯仰角
- θ̇: 身体角速度
- x: 水平位置
- ẋ: 水平速度
- φ: 腿部角度
- φ̇: 腿部角速度

**控制输入** (2维):
- T: 身体转矩
- Tₚ: 腿部转矩

**权重矩阵**:
- Q = diag([1, 10, 100, 20, 1000, 1]) (状态权重)
- R = diag([1, 1]) (控制权重)

## 软件架构

### 分层架构
```
┌────────────────────────────┐
│  运动规划层                 │
│  (Motion Planner)          │
├────────────────────────────┤
│  LQR控制 + 飞行控制        │ ★ 新增
│  (LQR + Flight Control)    │
├────────────────────────────┤
│  腿部运动学                 │
│  (Leg Kinematics)          │
├────────────────────────────┤
│  电机抽象                   │
│  (Motor Abstraction)       │
├────────────────────────────┤
│  CAN驱动                    │
│  (CAN Driver)              │
└────────────────────────────┘
```

### 核心模块
| 模块 | 文件 | 功能 |
|------|------|------|
| 电机驱动 | motor_controller.c | 8个电机的CAN控制 |
| CAN通信 | can_driver.c | 消息队列、ISR处理 |
| 腿部控制 | leg_controller.c | 正逆运动学 (3链条腿) |
| 运动规划 | motion_planner.c | 步态生成 (小跑) |
| **LQR控制** | **lqr_controller.c** | **反馈稳定性调节** ★ |
| **增益计算** | **lqr_gains.c** | **多项式增益评估** ★ |
| **飞行控制** | **flight_controller.c** | **飞行阶段PD控制** ★ |
| **系统集成** | **lqr_system.c** | **统一管理&模式切换** ★ |

## 编译和烧写

### 环境要求
- CMake 3.10 或更高版本
- ARM GCC 交叉编译工具链 (arm-none-eabi-gcc)
- STM32F405 相关库 (STM32CubeF4)

### 编译步骤
```bash
cd software
mkdir build
cd build
cmake ..
make
```

### 验证编译
```bash
# 应该包含LQR模块的编译
# [100%] Building C object CMakeFiles/firmware.elf.dir/src/lqr_*.c.o
# [100%] Linking C executable firmware.elf
```

## 运行单元测试

```c
// 在main函数中调用测试
run_lqr_tests();  // 执行11个LQR单元测试

// 预期输出:
// [PASS] LQR Gain Computation
// [PASS] LQR Controller Init
// ...
// All tests passed!
```

## 功能清单

### 已完成 ✓
- [x] 电机驱动初始化
- [x] CAN总线通信框架
- [x] 关节电机控制
- [x] 驱动电机控制
- [x] 腿部运动学 (FK/IK)
- [x] 小跑步态规划
- [x] 数据日志记录
- [x] UART调试接口
- [x] **LQR控制器** ★
- [x] **飞行相位控制** ★
- [x] **单元测试框架** ★

### 待完成 ⏳
- [ ] STM32 HAL集成 (CAN/UART初始化)
- [ ] IMU传感器驱动
- [ ] 编码器反馈处理
- [ ] 高级步态规划

## 与MATLAB的关联

| MATLAB文件 | 对应C代码 | 说明 |
|-----------|---------|------|
| sys_calc.m | lqr_gains.c | 增益系数表 |
| lqr_k.m | lqr_gains_compute() | 多项式求值 |
| flight_joint_hold.m | flight_controller.c | PD控制 |
| flight_hold_params.m | flight_params_t | 参数 |

详见 MATLAB文件夹: ../matlab/

## 性能指标

| 指标 | 值 |
|------|-----|
| 增益计算 | O(1) |
| 控制律执行 | O(12) |
| 采样周期 | 5ms (200Hz) |
| 内存占用 | <1KB |
| 代码行数 | ~2000行 |

## 参考资源
- RoboMaster C型开发板文档
- 达妙电机通讯协议
- **MATLAB仿真模型** (../matlab文件夹)
- **LQR控制理论** (../../matlab/中的设计文档)

## 常见问题

**Q: 如何集成LQR到我的应用?**  
A: 参考 [LQR_QUICK_REFERENCE.md](LQR_QUICK_REFERENCE.md)

**Q: 如何调整控制参数?**  
A: 修改MATLAB文件并重新生成，或参考 lqr_gains.c 中的系数表

**Q: 支持哪些控制模式?**  
A: IDLE、LQR_REGULATION、LQR_TRACKING、FLIGHT_HOLD、JUMP_SEQUENCE

---

## 版本信息
- **项目版本**: 1.0.0
- **LQR集成版本**: 1.0.0 ★
- **创建日期**: 2026-05-09
- **最后更新**: 2026-05-09
- **MATLAB对应**: sys_calc.m (2026-03-24)

## 作者
RoboMaster开发团队

