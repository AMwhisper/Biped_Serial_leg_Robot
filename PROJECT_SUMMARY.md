# RoboMaster C型开发板 - 四足轮腿机器人

## 项目完成总结

本项目是一个基于RoboMaster C型开发板的完整四足轮腿机器人系统。采用达妙电机(6215关节电机、4310驱动电机)，使用STM32F405作为主控制器。

## 项目特性

### 硬件支持
- **开发板**: RoboMaster C型 (STM32F405RG)
- **关节电机**: 达妙6215 (12Nm, 65rad/s)
- **驱动电机**: 达妙4310 (5Nm, 130rad/s)
- **通讯**: CAN总线 (1Mbps)

### 软件架构
```
├── 电机控制层
│   ├── 达妙电机通讯协议实现
│   ├── CAN总线驱动
│   └── 电机反馈处理
├── 腿部控制层
│   ├── 运动学计算 (正逆向)
│   ├── 单腿控制
│   └── 腿部状态反馈
├── 运动规划层
│   ├── 步态规划 (小跑、回避、跳跃等)
│   ├── 速度规划
│   └── 路径规划
└── 系统管理层
    ├── CAN通讯
    ├── 数据记录
    ├── 调试接口
    └── 错误处理
```

## 源代码模块

| 模块 | 文件 | 功能 |
|------|------|------|
| 电机控制 | motor_controller.c/h | 达妙电机驱动和反馈处理 |
| CAN驱动 | can_driver.c/h | STM32 CAN总线驱动 |
| 腿部控制 | leg_controller.c/h | 腿部运动学和控制 |
| 运动规划 | motion_planner.c/h | 步态规划和运动生成 |
| 系统管理 | system.c/h | 系统初始化和管理 |
| 数据记录 | data_logger.c/h | 传感器数据记录 |
| UART | uart_interface.c/h | 调试和通讯接口 |
| 测试框架 | test_framework.c/h | 单元测试框架 |

## API概览

### 基本初始化
```c
system_init();  // 初始化系统
system_update(); // 主循环更新
```

### 电机控制
```c
motor_enable(1);              // 启用电机
motor_set_torque(1, 5.0f);    // 设置力矩
motor_set_speed(1, 10.0f);    // 设置速度
motor_set_position(1, 1.57f); // 设置位置
```

### 腿部控制
```c
leg_position_t pos = {0.1f, 0.0f, -0.2f};
leg_set_position(LEG_FRONT_LEFT, &pos);  // 设置腿位置
leg_get_state(LEG_FRONT_LEFT, &state);   // 获取腿状态
```

### 运动规划
```c
motion_params_t params = {
    .linear_velocity = 0.5f,
    .step_height = 0.05f,
    .gait_period = 1.0f
};
motion_planner_execute(MOTION_WALK_FORWARD, &params);
```

## 文档

- **README.md** - 中文项目说明
- **README_en.md** - 英文项目说明
- **QUICK_START.md** - 快速开始指南
- **DEVELOPMENT_GUIDE.md** - 开发指南
- **BUILD_AND_DEPLOY.md** - 编译和部署指南
- **MOTOR_PROTOCOL.md** - 达妙电机通讯协议
- **PROJECT_STRUCTURE.md** - 项目结构说明
- **API_REFERENCE.md** - 完整API参考

## 编译方式

### 使用PlatformIO (推荐)
```bash
cd software
platformio run -e robomaster_c
```

### 使用CMake
```bash
cd software
mkdir build && cd build
cmake ..
make
```

### 使用Makefile
```bash
cd software
make
```

## 烧写和调试

### 烧写到开发板
```bash
platformio run -e robomaster_c -t upload
```

### 开始调试
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
# 在另一个终端
arm-none-eabi-gdb software/build/firmware.elf
(gdb) target remote localhost:3333
(gdb) load
```

## 系统运行流程

1. **硬件初始化**
   - 时钟和外设配置
   - GPIO初始化

2. **模块初始化** (system_init)
   - CAN总线初始化
   - 电机控制器初始化
   - 腿部控制器初始化
   - 运动规划器初始化

3. **主控制循环** (system_update)
   - 处理CAN反馈消息
   - 更新电机状态
   - 执行运动规划
   - 生成控制命令
   - 记录数据

4. **中断处理**
   - CAN接收中断
   - UART接收中断
   - 定时器中断 (控制周期)

## 实现完成度

✅ **已完成**:
- 项目框架搭建
- 电机驱动接口
- CAN总线驱动框架
- 腿部运动学实现
- 基本步态规划
- 系统管理框架
- 数据记录系统
- 调试接口
- 测试框架
- 完整文档

⏳ **待实现** (基于STM32HAL库):
- STM32 CAN总线实际配置
- STM32 UART实际配置
- 中断处理程序
- 定时器配置
- 系统时钟初始化
- 硬件特定的初始化代码

## 项目扩展建议

1. **传感器集成**
   - IMU (加速度计、陀螺仪)
   - 地面接触传感器
   - 关节负载传感器

2. **高级控制**
   - 姿态稳定控制
   - 步长优化
   - 能量优化

3. **导航功能**
   - 里程计
   - SLAM定位
   - 路径规划

4. **遥控接口**
   - 遥控器支持
   - 蓝牙/WiFi通讯
   - Web控制界面

## 关键特性

### 软件设计
- 模块化架构，便于扩展
- 清晰的接口定义
- 完善的错误处理
- 单元测试框架

### 硬件支持
- 支持多种电机类型
- 灵活的CAN总线配置
- 调试接口丰富

### 文档
- API参考文档
- 开发指南
- 故障排查指南
- 编译部署说明

## 目录结构

```
robomaster-c/
├── software/                 # 源代码和编译配置
│   ├── src/                 # C源文件
│   ├── include/             # 头文件
│   ├── CMakeLists.txt       # CMake配置
│   ├── platformio.ini       # PlatformIO配置
│   └── Makefile             # Makefile
├── config/                  # 系统配置文件
├── hardware/                # 硬件设计文件 (电路、PCB)
├── docs/                    # 文档
└── README.md                # 项目说明

```

## 许可证

本项目遵循项目根目录LICENSE文件

## 联系方式

如有问题或建议，欢迎联系项目维护者。

---

**项目版本**: v1.0.0  
**最后更新**: 2026-05-09  
**开发团队**: RoboMaster FYP Team
