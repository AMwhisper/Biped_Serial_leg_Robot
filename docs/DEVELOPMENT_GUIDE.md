# 项目开发指南

## 开发环境搭建

### 1. 安装工具链
```bash
# Windows 用户 (使用 ARM GNU Toolchain)
# 下载地址: https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads

# Linux/Mac 用户
sudo apt install build-essential cmake gcc-arm-none-eabi binutils-arm-none-eabi arm-none-eabi-gdb
```

### 2. 安装依赖
```bash
# 安装 CMake
# 从 https://cmake.org/download/ 下载安装

# 安装 OpenOCD (烧写调试工具)
# Windows: 下载 OpenOCD-20220820.0-win64 
# Linux: sudo apt install openocd
```

### 3. 克隆并编译
```bash
cd robomaster-c/software
mkdir build
cd build
cmake ..
make
```

## 代码结构

### 核心文件
```
include/
├── motor_controller.h       # 电机控制接口
└── ... (其他头文件)

src/
├── main.c                  # 主程序
├── motor_controller.c      # 电机控制实现
└── ... (其他源文件)

config/
└── motor_config.h          # 电机配置
```

## 开发步骤

### Phase 1: 基础通讯
- [ ] 配置 CAN 总线接口
- [ ] 实现 CAN 消息发送/接收
- [ ] 电机使能/关闭命令
- [ ] 电机状态查询

### Phase 2: 单电机控制
- [ ] 实现力矩控制
- [ ] 实现速度控制
- [ ] 实现位置控制
- [ ] 电机反馈处理

### Phase 3: 多电机协调
- [ ] 4个6215关节电机同步控制
- [ ] 4个4310驱动电机同步控制
- [ ] 混合控制模式实现

### Phase 4: 运动控制
- [ ] 腿部正向运动学
- [ ] 腿部逆向运动学
- [ ] 步态规划
- [ ] 稳定性控制

### Phase 5: 高级功能
- [ ] 遥控接口
- [ ] 路径规划
- [ ] 自主导航
- [ ] 数据记录和分析

## 调试技巧

### 1. CAN 总线监控
```bash
# Linux 下监控 CAN 消息
candump can0

# 发送测试消息
cansend can0 141#8800000000000000
```

### 2. 使用 OpenOCD 调试
```bash
# 启动 OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

# 连接 GDB
arm-none-eabi-gdb firmware.elf
(gdb) target remote localhost:3333
(gdb) load
(gdb) break main
(gdb) continue
```

### 3. 串口输出调试
```c
// 在 main.c 中添加
printf("Motor 1 Position: %.2f rad\n", motor_get(1)->position);
```

## 常见问题

### Q1: 电机无反应
- 检查 CAN 总线连接
- 确认电源供电 (24V)
- 验证 CAN 波特率配置 (1Mbps)
- 检查电机 CAN ID 配置

### Q2: 反馈数据异常
- 校验 CAN 消息格式
- 检查数据转换公式
- 使用示波器测量 CAN 信号

### Q3: 控制不稳定
- 调整 PID 参数
- 降低更新频率进行测试
- 检查电源质量

## 参考资源

### 文档
- [RoboMaster C型开发板官方文档](#)
- [达妙电机通讯协议](#)
- [ARM Cortex-M4 处理器手册](#)

### 代码示例
- 参考 matlab 文件夹的控制算法
- 参考 esp32-controller 的通讯实现

## 联系方式
如有问题，请参考项目文档或联系技术支持。

---
最后更新: 2026-05-09
