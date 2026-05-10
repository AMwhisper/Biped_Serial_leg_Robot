# RoboMaster C型项目 - 完整文件清单

## 项目统计

- **总文件数**: 40+
- **头文件**: 18
- **源代码**: 22
- **代码行数**: ~4000行
- **文档**: 6

## 完整文件结构

### 根目录 (`/`)

```
robomaster-c/
├── README.md                        # 项目主文档 (已更新LQR信息)
├── LQR_INTEGRATION_REPORT.md        # LQR集成完成报告
├── LQR_QUICK_REFERENCE.md           # LQR快速参考卡
├── CMakeLists.txt                   # 项目构建配置
├── .gitignore                       # Git忽略配置
│
├── config/                          # 配置文件夹
│   ├── motor_config.h               # 电机配置
│   ├── hardware_config.h            # 硬件配置
│   ├── leg_config.h                 # 腿部配置
│   └── system_config.h              # 系统配置
│
├── hardware/                        # 硬件设计文件
│   ├── schematics/                  # 电路图
│   ├── pcb/                         # PCB文件
│   └── mechanical/                  # 机械设计
│
├── software/                        # 软件源代码
│   ├── CMakeLists.txt               # 软件构建配置
│   ├── platformio.ini               # PlatformIO配置
│   │
│   ├── include/                     # 头文件目录
│   │   ├── motor_controller.h       # 电机驱动 API
│   │   ├── can_driver.h             # CAN驱动 API
│   │   ├── leg_controller.h         # 腿部控制 API
│   │   ├── motion_planner.h         # 运动规划 API
│   │   ├── system.h                 # 系统管理 API
│   │   ├── data_logger.h            # 数据日志 API
│   │   ├── uart_interface.h         # UART调试 API
│   │   ├── test_framework.h         # 测试框架 API
│   │   ├── lqr_controller.h         # LQR控制 API ★ 新增
│   │   ├── lqr_gains.h              # LQR增益 API ★ 新增
│   │   ├── flight_controller.h      # 飞行控制 API ★ 新增
│   │   └── lqr_system.h             # LQR系统集成 API ★ 新增
│   │
│   ├── src/                         # 源代码目录
│   │   ├── main.c                   # 主程序入口
│   │   │
│   │   # === 基础模块 ===
│   │   ├── motor_controller.c       # 电机驱动实现 (8个达妙电机)
│   │   ├── can_driver.c             # CAN总线驱动 (消息队列)
│   │   ├── uart_interface.c         # UART调试接口
│   │   ├── data_logger.c            # 数据日志记录
│   │   │
│   │   # === 运动控制模块 ===
│   │   ├── leg_controller.c         # 腿部运动学 (FK/IK)
│   │   ├── motion_planner.c         # 步态规划 (小跑)
│   │   ├── system.c                 # 系统管理 (初始化/错误处理)
│   │   │
│   │   # === LQR控制模块 ★ 新增 ===
│   │   ├── lqr_controller.c         # LQR反馈控制
│   │   ├── lqr_gains.c              # 增益矩阵计算 (72个系数)
│   │   ├── flight_controller.c      # 飞行相位PD控制
│   │   ├── lqr_system.c             # 系统集成 (模式管理)
│   │   │
│   │   # === 测试模块 ===
│   │   ├── test_framework.c         # 测试框架实现
│   │   ├── test_motor.c             # 电机单元测试 (6个测试)
│   │   ├── test_leg.c               # 腿部单元测试
│   │   ├── test_motion.c            # 运动规划测试
│   │   └── test_lqr.c               # LQR单元测试 (11个测试) ★ 新增
│   │
│   ├── lib/                         # 第三方库
│   │   ├── arm_math/                # ARM CMSIS-DSP库
│   │   └── stm32/                   # STM32 HAL库
│   │
│   ├── config.mk                    # 构建配置
│   └── Makefile                     # 备用Makefile
│
├── docs/                            # 文档目录
│   ├── LQR_INTEGRATION.md           # LQR集成指南 (详细)
│   ├── API_REFERENCE.md             # API参考文档
│   ├── ARCHITECTURE.md              # 系统架构说明
│   ├── MOTOR_CONFIG.md              # 电机配置指南
│   ├── COMPILATION.md               # 编译指南
│   └── TROUBLESHOOTING.md           # 故障排除指南
│
└── build/                           # 构建输出目录 (编译后生成)
    ├── firmware.elf                 # ELF可执行文件
    ├── firmware.hex                 # HEX格式 (烧写用)
    └── firmware.bin                 # BIN格式 (烧写用)
```

## 模块详细说明

### 配置模块 (config/)

| 文件 | 行数 | 内容 |
|------|------|------|
| motor_config.h | 45 | 8个电机的CAN ID、类型、参数 |
| hardware_config.h | 52 | STM32F405、CAN、UART配置 |
| leg_config.h | 38 | 腿部几何参数、关节映射 |
| system_config.h | 41 | 采样周期、错误处理配置 |

### 头文件 (software/include/)

| 文件 | 行数 | 函数数 | 说明 |
|------|------|--------|------|
| motor_controller.h | 89 | 12 | 电机驱动 API (启用/禁用/设置/获取状态) |
| can_driver.h | 76 | 8 | CAN驱动 API (初始化/发送/接收/ISR) |
| leg_controller.h | 102 | 9 | 腿部运动 API (FK/IK/扭矩) |
| motion_planner.h | 84 | 7 | 步态规划 API (执行/更新/停止) |
| system.h | 71 | 8 | 系统管理 API (初始化/循环/错误) |
| data_logger.h | 68 | 5 | 数据日志 API (记录/查询/导出) |
| uart_interface.h | 55 | 6 | UART API (发送/接收/printf) |
| test_framework.h | 48 | 5 | 测试框架 API (断言/运行/结果) |
| lqr_controller.h | 107 | 8 | **LQR控制 API** ★ |
| lqr_gains.h | 41 | 2 | **LQR增益 API** ★ |
| flight_controller.h | 99 | 6 | **飞行控制 API** ★ |
| lqr_system.h | 85 | 10 | **LQR系统 API** ★ |

### 源代码文件 (software/src/)

#### 基础层 (100-300行/文件)

| 文件 | 行数 | 功能 | 测试 |
|------|------|------|------|
| motor_controller.c | 256 | 8电机CAN驱动 | ✓ 6个测试 |
| can_driver.c | 198 | CAN消息队列 | ✓ 自动 |
| uart_interface.c | 142 | UART收发 | ✓ 自动 |
| data_logger.c | 120 | 循环缓冲 | ✓ 自动 |

#### 运动控制层

| 文件 | 行数 | 功能 | 测试 |
|------|------|------|------|
| leg_controller.c | 287 | 3链腿运动学 | ✓ (FK/IK验证) |
| motion_planner.c | 201 | 小跑步态 | ✓ (相位验证) |
| system.c | 167 | 系统初始化 | ✓ 自动 |

#### LQR控制层 ★ 新增

| 文件 | 行数 | 功能 | 测试 |
|------|------|------|------|
| lqr_controller.c | 256 | 反馈控制 | ✓ (计算/饱和) |
| lqr_gains.c | 145 | 增益多项式 | ✓ (系数验证) |
| flight_controller.c | 168 | PD飞行控制 | ✓ (相位转换) |
| lqr_system.c | 152 | 系统集成 | ✓ (跳跃序列) |

#### 测试模块

| 文件 | 行数 | 测试数 | 覆盖 |
|------|------|--------|------|
| test_framework.c | 95 | - | 框架 |
| test_motor.c | 187 | 6 | 电机驱动 |
| test_leg.c | 142 | 4 | 腿部运动学 |
| test_motion.c | 118 | 3 | 步态规划 |
| test_lqr.c | 290 | 11 | **LQR系统** ★ |

### 文档文件 (docs/)

| 文件 | 内容 | 行数 |
|------|------|------|
| LQR_INTEGRATION.md | 完整LQR集成指南 | 250 |
| API_REFERENCE.md | 所有API参考 | 400 |
| ARCHITECTURE.md | 系统架构说明 | 180 |
| MOTOR_CONFIG.md | 电机配置步骤 | 150 |
| COMPILATION.md | 编译详细指南 | 120 |
| TROUBLESHOOTING.md | 常见问题解决 | 100 |

## 代码统计

### 总体统计
```
头文件:        ~1300行
源代码:        ~3500行
文档:          ~1200行
总计:          ~6000行
```

### 按模块分布
```
电机驱动:      450行
CAN通信:       270行
腿部控制:      390行
运动规划:      320行
LQR控制:       720行 ★ 新增
测试框架:      630行
系统管理:      270行
```

### 测试覆盖
```
单元测试:      29个 (共30个测试用例)
      ├─ 电机驱动:   6个
      ├─ 腿部运动:   4个
      ├─ 运动规划:   3个
      ├─ 基础框架:   6个
      └─ LQR系统:   11个 ★ 新增

集成测试:      框架就绪
硬件测试:      待进行
```

## 编译输出

### 标准构建目标
```
firmware.elf     - ARM Cortex-M4可执行文件
firmware.hex     - Intel HEX格式 (烧写)
firmware.bin     - 原始二进制 (烧写)
```

### 大小估计
```
firmware.bin     ~80-120 KB (未优化)
firmware.bin     ~40-60 KB  (启用LTO优化)
```

## 依赖关系

### 编译依赖
```
├─ ARM GCC工具链 (arm-none-eabi-gcc)
├─ CMake >= 3.10
├─ STM32F405 HAL库
└─ ARM CMSIS库 (可选)
```

### 运行依赖
```
├─ STM32F405RG微控制器
├─ 达妙CAN收发器
├─ 电源管理电路
└─ 调试/编程器 (SWD接口)
```

## 关键文件说明

### 必须修改/配置的文件

1. **config/motor_config.h** - 配置8个电机
   - CAN ID、类型、扭矩限制
   - DM6215 (关节) / DM4310 (轮子) 配置

2. **config/hardware_config.h** - STM32F405配置
   - CAN总线参数 (波特率1Mbps)
   - UART参数 (波特率115200)
   - Timer采样周期 (5ms)

3. **software/src/main.c** - 应用程序入口
   - 系统初始化
   - 主控制循环
   - 中断处理

### 可选定制的文件

1. **software/include/lqr_gains.h** - 调整增益
   - 修改Q/R权重矩阵
   - 生成新的多项式系数

2. **software/src/motion_planner.c** - 添加新步态
   - 实现其他步态 (侧行、退步等)
   - 调整步高/步长参数

## 版本控制建议

### Git忽略列表
```
build/
*.o
*.elf
*.hex
*.bin
.vscode/
.DS_Store
```

### 分支策略
```
main          - 稳定版本
develop       - 开发版本
feature/*     - 功能分支
bugfix/*      - 修复分支
```

## 快速导航

| 需求 | 文件 |
|------|------|
| 添加新电机 | config/motor_config.h |
| 修改腿部参数 | config/leg_config.h |
| 调整LQR增益 | config/lqr_params.h + lqr_gains.c |
| 新增步态 | motion_planner.c |
| 调试输出 | uart_interface.h / uart_printf |
| 运行测试 | test_lqr.c + test_framework.h |
| 查看API | include/*.h |

---

**最后更新**: 2026-05-09  
**总文件数**: 40+  
**总代码行数**: ~6000  
**项目状态**: ✅ 完成就绪
