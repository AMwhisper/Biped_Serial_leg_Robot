# Makefile 简化编译

include config/build.mk

# 编译目标
TARGET = firmware

# 构建目录
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
DEP_DIR = $(BUILD_DIR)/dep

# 源文件
C_SOURCES = $(shell find software/src -name "*.c")
INCLUDE_DIRS = -Isoftware/include -Iconfig

# 编译器配置
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size

# 编译标志
CFLAGS = -mcpu=cortex-m4 -mthumb -O2 -Wall -Wextra $(INCLUDE_DIRS)
LDFLAGS = -Tstm32f405xx.ld -Wl,--gc-sections

# 目标文件
OBJECTS = $(C_SOURCES:software/src/%.c=$(OBJ_DIR)/%.o)

# 默认目标
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# 构建ELF文件
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^
	$(SIZE) $@

# 编译源文件
$(OBJ_DIR)/%.o: software/src/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 生成HEX文件
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

# 生成BIN文件
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# 清除编译文件
clean:
	rm -rf $(BUILD_DIR)

# 查看反汇编
disasm: $(BUILD_DIR)/$(TARGET).elf
	$(OBJDUMP) -d $< > $(BUILD_DIR)/$(TARGET).asm

# 帮助信息
help:
	@echo "Available targets:"
	@echo "  make all     - Build firmware"
	@echo "  make clean   - Remove build files"
	@echo "  make disasm  - Generate disassembly"
	@echo "  make help    - Show this help message"

.PHONY: all clean disasm help
