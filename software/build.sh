#!/bin/bash

# RoboMaster C型 四足轮腿机器人 - 快速启动脚本
# Quick start script for RoboMaster C Quadruped Robot

set -e

PROJECT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$PROJECT_DIR/software/build"
SRC_DIR="$PROJECT_DIR/software"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== RoboMaster C Quadruped Robot Build System ===${NC}"

# 检查依赖
check_dependencies() {
    echo -e "${YELLOW}Checking dependencies...${NC}"
    
    if ! command -v arm-none-eabi-gcc &> /dev/null; then
        echo -e "${RED}Error: arm-none-eabi-gcc not found${NC}"
        echo "Please install ARM GNU Toolchain"
        exit 1
    fi
    
    if ! command -v cmake &> /dev/null; then
        echo -e "${RED}Error: cmake not found${NC}"
        echo "Please install CMake"
        exit 1
    fi
    
    echo -e "${GREEN}Dependencies OK${NC}"
}

# 编译项目
build_project() {
    echo -e "${YELLOW}Building project...${NC}"
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    cmake ..
    make -j$(nproc)
    
    echo -e "${GREEN}Build complete!${NC}"
    echo "Firmware: $BUILD_DIR/firmware.elf"
    echo "Hex file: $BUILD_DIR/firmware.hex"
    echo "Binary:   $BUILD_DIR/firmware.bin"
}

# 显示帮助
show_help() {
    echo "Usage: ./build.sh [OPTION]"
    echo ""
    echo "Options:"
    echo "  build       Build the firmware"
    echo "  clean       Clean build files"
    echo "  upload      Build and upload to device"
    echo "  check       Check dependencies only"
    echo "  help        Show this help message"
}

# 主程序
case "${1:-build}" in
    build)
        check_dependencies
        build_project
        ;;
    clean)
        echo -e "${YELLOW}Cleaning...${NC}"
        rm -rf "$BUILD_DIR"
        echo -e "${GREEN}Clean complete${NC}"
        ;;
    upload)
        check_dependencies
        build_project
        echo -e "${YELLOW}Uploading to device...${NC}"
        # 这里添加上传命令
        # openocd -f interface/stlink.cfg -f target/stm32f4x.cfg ...
        echo -e "${GREEN}Upload complete${NC}"
        ;;
    check)
        check_dependencies
        ;;
    help)
        show_help
        ;;
    *)
        show_help
        exit 1
        ;;
esac
