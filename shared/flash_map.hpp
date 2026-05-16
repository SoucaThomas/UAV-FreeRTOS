#ifndef FLASH_MAP_HPP
#define FLASH_MAP_HPP

#include <stdint.h>

constexpr uint16_t FLASH_CONFIG_MAGIC = 0x0A4f;
constexpr uint8_t FLASH_CONFIG_VERSION = 1;

constexpr uint32_t FLASH_CONFIG_ADDR = 0x000000;
constexpr uint32_t FLASH_CRASH_ADDR = 0x001000;
constexpr uint32_t FLASH_BLACKBOX_ADDR = 0x002000;
constexpr uint32_t FLASH_SIZE_END = 0x800000;

#endif