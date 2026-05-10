# 编译和部署指南

## 编译工具链

### Windows环境

#### 1. 安装ARM工具链
下载并安装 ARM GNU Toolchain:
- 下载链接: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
- 选择最新版本 (如 arm-gnu-toolchain-13.2.rel1-mingw-w64-i686-arm-none-eabi)
- 安装到默认路径或自定义路径

#### 2. 安装CMake
- 下载: https://cmake.org/download/
- 选择 Windows x64 Installer
- 安装并添加到系统PATH

#### 3. 安装PlatformIO (推荐)
```bash
# 通过pip安装
pip install platformio

# 或在VS Code中安装PlatformIO扩展
```

#### 4. 安装OpenOCD (调试)
- 下载: https://github.com/openocd-org/openocd/releases
- 选择 openocd-20240326.0-i686-w64-mingw32.zip
- 解压到任意目录，添加bin目录到PATH

### Linux/Mac环境

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake gcc-arm-none-eabi binutils-arm-none-eabi gdb-arm-none-eabi openocd

# macOS (使用Homebrew)
brew install cmake arm-none-eabi-gcc openocd
```

## 编译项目

### 方法1: 使用PlatformIO (推荐)

#### 快速编译
```bash
cd robomaster-c/software
platformio run -e robomaster_c
```

#### 完整构建 (包括链接和优化)
```bash
platformio run -e robomaster_c --target clean
platformio run -e robomaster_c
```

#### 编译特定文件
```bash
platformio run -e robomaster_c -- --verbose
```

### 方法2: 使用CMake

#### 初始编译
```bash
cd robomaster-c/software
mkdir build
cd build
cmake ..
make
```

#### 增量编译
```bash
cd build
make
```

#### 清除编译文件
```bash
cd build
make clean
```

#### 指定编译器 (如果CMake无法自动找到)
```bash
cmake .. -DCMAKE_C_COMPILER=arm-none-eabi-gcc \
         -DCMAKE_CXX_COMPILER=arm-none-eabi-g++ \
         -DCMAKE_SYSTEM_NAME=Generic \
         -DCMAKE_SYSTEM_PROCESSOR=arm
```

## 生成输出文件

编译完成后，会生成以下文件:

### 在 build/ 目录下 (CMake):
- `firmware.elf` - 可执行映像 (用于调试)
- `firmware.hex` - Intel HEX格式 (用于烧写)
- `firmware.bin` - 二进制文件 (用于烧写)

### 在 .pio/build/robomaster_c/ (PlatformIO):
- `firmware.elf`
- `firmware.hex`
- `firmware.bin`

## 烧写到开发板

### 使用PlatformIO烧写

#### 连接ST-Link V2调试器到开发板
```
ST-Link -> RoboMaster C型板
GND     -> GND
SWCLK   -> SWCLK
SWDIO   -> SWDIO
+3.3V   -> +3.3V (可选)
```

#### 烧写固件
```bash
cd robomaster-c/software
platformio run -e robomaster_c -t upload
```

### 使用OpenOCD烧写

#### 启动OpenOCD服务器
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
```

#### 在另一个终端执行烧写脚本
```bash
arm-none-eabi-gdb firmware.elf
(gdb) target remote localhost:3333
(gdb) load
(gdb) reset
(gdb) quit
```

### 使用STM32CubeProgrammer

1. 下载并安装 STM32CubeProgrammer
2. 连接ST-Link到开发板
3. 打开STM32CubeProgrammer
4. 选择 Erasing & Programming
5. 选择 firmware.bin 或 firmware.hex
6. 点击 Start Programming

## 调试

### 使用GDB调试

#### 启动OpenOCD (如果未启动)
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
```

#### 启动GDB调试会话
```bash
arm-none-eabi-gdb firmware.elf
(gdb) target remote localhost:3333
(gdb) load                    # 烧写程序
(gdb) break main              # 在main()设置断点
(gdb) continue                # 运行到断点
(gdb) step                    # 单步
(gdb) print variable_name     # 打印变量
(gdb) quit                    # 退出
```

### 使用VS Code调试

#### 1. 安装扩展
- Cortex-Debug
- C/C++

#### 2. 创建 .vscode/launch.json
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Cortex Debug",
            "cwd": "${workspaceRoot}/software",
            "executable": "${workspaceRoot}/software/build/firmware.elf",
            "request": "launch",
            "type": "cortex-debug",
            "device": "STM32F405RG",
            "interface": "swd",
            "servertype": "openocd"
        }
    ]
}
```

#### 3. 启动调试
- 按 F5 或在Debug菜单选择"Start Debugging"

## 故障排除

### 编译错误

#### 找不到arm-none-eabi-gcc
```bash
# 检查工具链是否安装
arm-none-eabi-gcc --version

# 添加到PATH (Windows)
set PATH=%PATH%;C:\Program Files\arm\arm-gnu-toolchain-13.2.rel1-mingw-w64-i686-arm-none-eabi\bin

# 或在CMake中指定编译器路径
cmake .. -DCMAKE_C_COMPILER="C:/Program Files/arm/arm-gnu-toolchain-13.2.rel1-mingw-w64-i686-arm-none-eabi/bin/arm-none-eabi-gcc.exe"
```

#### 库文件找不到
确保STM32CubeF4库已正确放置到 software/lib/ 目录:
```bash
ls software/lib/
# 应该包含 STM32CubeF4/ 目录
```

### 烧写错误

#### ST-Link连接失败
- 检查USB连接
- 检查驱动程序是否安装
- 尝试重新插拔ST-Link

#### 烧写超时
```bash
# 增加超时时间
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "set CONNECT_UNDER_RESET 1"
```

### 调试连接失败
```bash
# 重启OpenOCD服务器
# 检查防火墙设置
# 确保localhost:3333端口未被占用
netstat -an | find "3333"
```

## 性能优化

### 编译优化选项

编辑 software/CMakeLists.txt:
```cmake
# 优化级别: -O0 (无优化), -O1, -O2, -Os (大小优化)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

# 启用LTO (Link Time Optimization)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto")

# 减小二进制大小
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
```

### 内存使用优化

- 使用 `--gc-sections` 移除未使用代码
- 优化栈大小
- 使用常量存储重数据

## 持续集成

### GitHub Actions示例

在 .github/workflows/build.yml:
```yaml
name: Build Firmware

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install Tools
        run: |
          sudo apt-get update
          sudo apt-get install -y gcc-arm-none-eabi cmake
      - name: Build
        run: |
          cd robomaster-c/software
          mkdir build && cd build
          cmake ..
          make
```

---

最后更新: 2026-05-09
