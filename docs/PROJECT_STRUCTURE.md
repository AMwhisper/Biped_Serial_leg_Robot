# 项目结构说明 (Project Structure Documentation)

## 目录树
```
robomaster-c/
├── README.md                           # 中文项目说明
├── README_en.md                        # English README
├── .gitignore                          # Git忽略文件
│
├── hardware/                           # 硬件设计文件夹
│   ├── schematics/                     # 电路原理图
│   ├── pcb/                            # PCB设计文件
│   ├── 3d_models/                      # 3D模型（STL/STEP）
│   └── BOM.xlsx                        # 物料清单
│
├── software/                           # 软件源代码
│   ├── CMakeLists.txt                  # CMake构建配置
│   ├── platformio.ini                  # PlatformIO配置
│   ├── src/                            # C/C++ 源文件
│   │   ├── main.c                      # 主程序入口
│   │   ├── motor_controller.c          # 电机控制实现
│   │   ├── can_driver.c                # CAN驱动实现
│   │   └── ...
│   ├── include/                        # C/C++ 头文件
│   │   ├── motor_controller.h          # 电机控制接口
│   │   ├── can_driver.h                # CAN驱动接口
│   │   └── ...
│   └── lib/                            # 第三方库和依赖
│       ├── STM32CubeF4/                # STM32 HAL库
│       └── ...
│
├── config/                             # 配置文件
│   ├── motor_config.h                  # 电机配置参数
│   ├── hardware_config.h               # 硬件配置参数
│   └── system_config.h                 # 系统配置参数
│
└── docs/                               # 文档文件
    ├── MOTOR_PROTOCOL.md               # 电机通讯协议说明
    ├── DEVELOPMENT_GUIDE.md            # 开发指南
    ├── API_REFERENCE.md                # API参考文档
    ├── HARDWARE_DESIGN.md              # 硬件设计文档
    └── TROUBLESHOOTING.md              # 故障排查指南
```

## 文件说明

### 根目录文件
- **README.md** - 项目中文说明，包含快速开始指南
- **README_en.md** - 英文项目说明
- **.gitignore** - Git版本控制忽略文件列表

### software/ 文件夹

#### 顶层配置
- **CMakeLists.txt** - CMake构建脚本，用于交叉编译ARM二进制文件
- **platformio.ini** - PlatformIO工程配置，支持快速编译和烧写

#### src/ - 源代码
| 文件 | 用途 |
|-----|------|
| main.c | 主程序入口，初始化系统和主控制循环 |
| motor_controller.c | 电机控制逻辑实现 |
| can_driver.c | CAN总线驱动(待实现) |
| leg_controller.c | 腿部运动控制(待实现) |
| motion_planner.c | 运动规划算法(待实现) |

#### include/ - 头文件
| 文件 | 说明 |
|-----|------|
| motor_controller.h | 电机控制API声明 |
| can_driver.h | CAN驱动API声明 |
| leg_controller.h | 腿部控制API声明 |
| types.h | 共用数据类型定义 |

#### lib/ - 库文件
- **STM32CubeF4/** - STM32F405 HAL库（需下载）
- **RTOS/** - 实时操作系统库（可选）

### config/ 文件夹

| 文件 | 内容 |
|-----|------|
| motor_config.h | 电机ID、最大力矩、速度等参数 |
| hardware_config.h | 引脚定义、CAN波特率、安全参数 |
| system_config.h | 系统级参数(待创建) |

### docs/ 文件夹

| 文档 | 内容 |
|-----|------|
| MOTOR_PROTOCOL.md | 达妙电机CAN通讯协议详解 |
| DEVELOPMENT_GUIDE.md | 开发环境搭建和调试指南 |
| API_REFERENCE.md | C API参考(待创建) |
| HARDWARE_DESIGN.md | 电路设计和PCB设计(待创建) |
| TROUBLESHOOTING.md | 常见问题解答(待创建) |

## 开发工作流

### 1. 编译流程
```bash
# 方式1: 使用CMake
cd software
mkdir build && cd build
cmake ..
make

# 方式2: 使用PlatformIO
cd software
pio run -e robomaster_c
```

### 2. 烧写流程
```bash
# 使用OpenOCD和ST-Link
pio run -e robomaster_c -t upload

# 或使用STM32CubeProgrammer
```

### 3. 调试流程
```bash
# 启用ST-Link调试
pio debug -e robomaster_c
```

## 代码规范

### 命名规范
- 函数名: `module_function_name()`  例: `motor_set_torque()`
- 变量名: `variable_name` (小写下划线)
- 常量名: `CONSTANT_NAME` (全大写下划线)
- 结构体: `struct_name_t` (小写下划线+_t后缀)

### 文件头注释
```c
/**
 * @file filename.c
 * @brief 文件简要说明
 * @author 作者名
 * @date 创建日期
 */
```

### 函数注释
```c
/**
 * @brief 函数功能简述
 * @param param1 参数1说明
 * @param param2 参数2说明
 * @return 返回值说明
 */
```

## 电机接线图

### CAN总线连接
```
RoboMaster C型板 ---- CAN总线 ---- 达妙电机1
                   ├-------------- 达妙电机2
                   ├-------------- 达妙电机3
                   └-------------- 达妙电机4
```

### 电源分配
- 24V电源轨: 提供给所有达妙电机
- GND: 电机和控制板共地

## 版本历史

- **v0.1.0** (2026-05-09) - 初始项目框架搭建
  - 创建基本目录结构
  - 添加电机控制框架代码
  - 编写电机协议文档

## 后续计划

- [ ] 完成CAN驱动实现
- [ ] 实现四条腿的同步控制
- [ ] 添加路径规划算法
- [ ] 完成硬件电路设计
- [ ] 进行系统集成测试

---
最后更新: 2026-05-09
维护者: 项目开发团队
