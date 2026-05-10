cmake_minimum_required(VERSION 3.10)
project(RoboMaster_C_Quadruped)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Set ARM toolchain
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Compiler settings
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../config)

# Source files
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.s"
)

# LQR Module sources
set(LQR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lqr_controller.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lqr_gains.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lqr_system.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flight_controller.c
)

# Create executable
add_executable(firmware.elf ${SOURCES})

# Link libraries
target_link_libraries(firmware.elf
    # Add your libraries here
)

# Generate hex and bin files
add_custom_command(TARGET firmware.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex firmware.elf firmware.hex
    COMMAND arm-none-eabi-objcopy -O binary firmware.elf firmware.bin
    COMMENT "Generating HEX and BIN files"
)
