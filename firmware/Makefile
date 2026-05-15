# Firmware target: uav (default) or ground
#   make all APP=uav
#   make all APP=ground
APP ?= uav

ifeq ($(APP),uav)
  TARGET = uav
else ifeq ($(APP),ground)
  TARGET = ground
else
  $(error Unknown APP=$(APP). Use APP=uav or APP=ground)
endif

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
BUILD_DIR   = build/$(APP)

# External libraries
EXT_DIR     = external
CMSIS_DIR   = $(EXT_DIR)/cmsis
RTOS_DIR    = $(EXT_DIR)/freertos
RTOS_PORT   = $(RTOS_DIR)/portable/GCC/ARM_CM4F
RTOS_HEAP   = $(RTOS_DIR)/portable/MemMang

# Sources — app-specific main + any shared src/*.cpp files
APP_SRCS = $(SRC_DIR)/$(APP)/main.cpp
STARTUP  = $(STARTUP_DIR)/startup.s
LINKER   = $(LINKER_DIR)/stm32f411ce.ld

# FreeRTOS C sources
RTOS_SRCS = $(RTOS_DIR)/tasks.c \
            $(RTOS_DIR)/queue.c \
            $(RTOS_DIR)/list.c \
            $(RTOS_PORT)/port.c \
            $(RTOS_HEAP)/heap_4.c

OBJS    = $(BUILD_DIR)/main.o
OBJS   += $(BUILD_DIR)/startup.o
OBJS   += $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(RTOS_SRCS)))

# Flags — Cortex-M4 with hardware FPU
MCU     = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

INCLUDES = -I$(INC_DIR) \
           -I$(SRC_DIR) \
           -I$(CMSIS_DIR) \
           -I$(RTOS_DIR)/include \
           -I$(RTOS_PORT) \
           -I../shared

CXXFLAGS = $(MCU) \
           -Os -g \
           -Wall -Wextra \
           -fno-exceptions \
           -fno-rtti \
           -ffunction-sections \
           -fdata-sections \
           -DSTM32F411xE \
           $(INCLUDES)

CFLAGS   = $(MCU) \
           -Os -g \
           -Wall -Wextra \
           -ffunction-sections \
           -fdata-sections \
           -DSTM32F411xE \
           $(INCLUDES)

LDFLAGS  = $(MCU) \
           -T$(LINKER) \
           -Wl,--gc-sections \
           -Wl,-Map=$(BUILD_DIR)/$(TARGET).map \
           --specs=nano.specs \
           -nostdlib

LDLIBS   = -lm -lgcc

# Build rules
all: $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	$(OBJCOPY) -O ihex $< $(BUILD_DIR)/$(TARGET).hex
	arm-none-eabi-objdump -d $< > $(BUILD_DIR)/$(TARGET).dis

# App-specific main
$(BUILD_DIR)/main.o: $(SRC_DIR)/$(APP)/main.cpp | $(BUILD_DIR)
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
	dfu-util -a 0 -s 0x08000000:leave -D $<

flash-stlink: $(BUILD_DIR)/$(TARGET).elf
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
		-c "program $< verify reset exit"

clean:
	rm -rf build

.PHONY: all flash flash-stlink clean
