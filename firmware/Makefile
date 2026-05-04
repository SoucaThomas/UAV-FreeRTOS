TARGET  = bluepill
PORT   ?= /dev/cu.usbserial-A50285BI

# Toolchain
CXX     = arm-none-eabi-g++
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

# Dirs
SRC_DIR     = src
INC_DIR     = include
STARTUP_DIR = startup
LINKER_DIR  = linker
BUILD_DIR   = build

# External libraries
EXT_DIR     = external
CMSIS_DIR   = $(EXT_DIR)/cmsis
RTOS_DIR    = $(EXT_DIR)/freertos
RTOS_PORT   = $(RTOS_DIR)/portable/GCC/ARM_CM3
RTOS_HEAP   = $(RTOS_DIR)/portable/MemMang

# Sources
SRCS    = $(wildcard $(SRC_DIR)/*.cpp)
STARTUP = $(STARTUP_DIR)/startup.s
LINKER  = $(LINKER_DIR)/stm32f103c8.ld

# FreeRTOS C sources
RTOS_SRCS = $(RTOS_DIR)/tasks.c \
            $(RTOS_DIR)/queue.c \
            $(RTOS_DIR)/list.c \
            $(RTOS_PORT)/port.c \
            $(RTOS_HEAP)/heap_4.c

OBJS    = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
OBJS   += $(BUILD_DIR)/startup.o
OBJS   += $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(RTOS_SRCS)))

# Flags
MCU     = -mcpu=cortex-m3 -mthumb

INCLUDES = -I$(INC_DIR) \
           -I$(CMSIS_DIR) \
           -I$(RTOS_DIR)/include \
           -I$(RTOS_PORT)

CXXFLAGS = $(MCU) \
           -Os -g \
           -Wall -Wextra \
           -fno-exceptions \
           -fno-rtti \
           -ffunction-sections \
           -fdata-sections \
           -DSTM32F103xB \
           $(INCLUDES)

CFLAGS   = $(MCU) \
           -Os -g \
           -Wall -Wextra \
           -ffunction-sections \
           -fdata-sections \
           -DSTM32F103xB \
           $(INCLUDES)

LDFLAGS  = $(MCU) \
           -T$(LINKER) \
           -Wl,--gc-sections \
           -Wl,-Map=$(BUILD_DIR)/$(TARGET).map \
           --specs=nano.specs \
           -nostdlib \
           -lgcc

# Build rules
all: $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@
	$(SIZE) $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/startup.o: $(STARTUP) | $(BUILD_DIR)
	$(CC) $(MCU) -c $< -o $@

# FreeRTOS kernel
$(BUILD_DIR)/tasks.o: $(RTOS_DIR)/tasks.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/queue.o: $(RTOS_DIR)/queue.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/list.o: $(RTOS_DIR)/list.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/port.o: $(RTOS_PORT)/port.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/heap_4.o: $(RTOS_HEAP)/heap_4.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

flash: $(BUILD_DIR)/$(TARGET).bin
	stm32flash -w $< -v -g 0x08000000 $(PORT)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all flash clean
