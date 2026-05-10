# RoboMaster C型开发板 - 快速开始指南

## 项目概览

这是一个基于**RoboMaster C型开发板**的四足轮腿机器人项目，采用：
- **关节电机**: 达妙6215（力矩型电机，适用于关节控制）
- **驱动电机**: 达妙4310（高速电机，适用于轮子驱动）

## 目录结构

```
robomaster-c/
├── software/          # 软件源代码和编译配置
├── hardware/          # 硬件设计文件
├── config/            # 系统配置文件
└── docs/              # 文档和教程
```

## 快速开始

### 1. 环境准备

#### Windows用户
```bash
# 安装ARM工具链
# 下载: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain

# 安装CMake (可选，推荐使用PlatformIO)
# 下载: https://cmake.org/download/

# 安装PlatformIO IDE (推荐)
# 下载: https://platformio.org/
```

#### Linux用户
```bash
sudo apt-get update
sudo apt-get install build-essential cmake gcc-arm-none-eabi binutils-arm-none-eabi
```

### 2. 编译项目

#### 方法A: 使用PlatformIO (推荐)
```bash
cd robomaster-c/software
platformio run
```

#### 方法B: 使用CMake
```bash
cd robomaster-c/software
mkdir build && cd build
cmake ..
make
```

### 3. 烧写到开发板

#### 使用PlatformIO烧写
```bash
platformio run --target upload
```

#### 使用OpenOCD烧写
```bash
# 需要ST-Link v2调试器
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
# 在另一个终端执行烧写脚本
```

## 硬件连接

### CAN总线连接
| 信号 | RoboMaster C型板 | 达妙电机 |
|------|-----------------|---------|
| CAN_H | PA12 | CAN_H |
| CAN_L | PA11 | CAN_L |
| GND | GND | GND |
| +24V | Power | +24V |

### 电机ID分配
| 位置 | 电机型号 | CAN ID |
|------|---------|--------|
| 左前关节 | DM6215 | 0x141 |
| 右前关节 | DM6215 | 0x142 |
| 左后关节 | DM6215 | 0x143 |
| 右后关节 | DM6215 | 0x144 |
| 左前轮 | DM4310 | 0x145 |
| 右前轮 | DM4310 | 0x146 |
| 左后轮 | DM4310 | 0x147 |
| 右后轮 | DM4310 | 0x148 |

## 核心代码文件

### 关键文件说明
| 文件 | 用途 |
|-----|------|
| `software/src/main.c` | 主程序和初始化 |
| `software/src/motor_controller.c` | 电机控制逻辑 |
| `config/motor_config.h` | 电机参数配置 |
| `config/hardware_config.h` | 硬件配置 |

## API快速参考

### 初始化
```c
motor_controller_init();  // 初始化电机控制系统
```

### 电机控制
```c
// 启用电机
motor_enable(0x141);      // 启用左前关节电机

// 设置力矩
motor_set_torque(0x141, 5.0f);   // 设置扭矩为5Nm

// 设置速度
motor_set_speed(0x145, 10.0f);   // 设置速度为10 rad/s

// 设置位置
motor_set_position(0x141, 1.57f); // 设置位置为π/2弧度

// 查询状态
motor_t motor_data;
motor_get_status(0x141, &motor_data);
```

## 文档参考

详细文档位于 `docs/` 文件夹:
- **DEVELOPMENT_GUIDE.md** - 完整的开发指南
- **MOTOR_PROTOCOL.md** - 电机通讯协议详解
- **PROJECT_STRUCTURE.md** - 项目结构说明

## 常见问题

### Q: 编译报错找不到STM32库
A: 需要手动添加STM32CubeF4库到 `software/lib/` 目录

### Q: 电机无法控制
A: 检查CAN总线连接和波特率设置（应为1Mbps）

### Q: 烧写失败
A: 确保ST-Link连接正确，驱动程序已安装

## 后续开发步骤

1. ✅ 项目框架搭建
2. ⏳ 实现CAN驱动
3. ⏳ 单电机控制测试
4. ⏳ 四电机协调控制
5. ⏳ 运动规划实现
6. ⏳ 系统集成测试

## 获取帮助

- 查看 `docs/` 文件夹中的详细文档
- 参考 `matlab/` 文件夹中的控制算法
- 查阅达妙电机官方通讯协议文档

## 许可证

本项目遵循项目根目录中的LICENSE文件

---

**最后更新**: 2026-05-09  
**项目版本**: v0.1.0
