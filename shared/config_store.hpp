#ifndef CONFIG_STORE_HPP
#define CONFIG_STORE_HPP

#include <stdint.h>

#include "crc.h"
#include "drivers/w25q64.hpp"
#include "flash_map.hpp"

struct __attribute__((packed)) Config {
  // Header
  uint16_t magic;
  uint8_t version;

  // PID gains
  float pitchP, pitchI, pitchD;
  float rollP, rollI, rollD;

  // Servo limits (microseconds)
  uint16_t servoMin, servoMax;
  uint16_t throttleMin, throttleMax;

  // Safety
  uint16_t radioTimeoutMs;
  uint16_t autoDisarmMs;

  // Radio
  uint8_t channel;
  uint8_t txPower;

  // Checksum — must be last
  uint16_t crc;
};

class ConfigStore {
 public:
  static Config defaults() {
    Config cfg = {};
    cfg.magic = FLASH_CONFIG_MAGIC;
    cfg.version = FLASH_CONFIG_VERSION;

    cfg.pitchP = 2.0f;
    cfg.pitchI = 0.1f;
    cfg.pitchD = 0.5f;
    cfg.rollP = 2.0f;
    cfg.rollI = 0.1f;
    cfg.rollD = 0.5f;

    cfg.servoMin = 1000;
    cfg.servoMax = 2000;
    cfg.throttleMin = 1000;
    cfg.throttleMax = 2000;

    cfg.radioTimeoutMs = 500;
    cfg.autoDisarmMs = 60000;

    cfg.channel = 0;
    cfg.txPower = 0x06;

    cfg.crc = computeCrc(cfg);
    return cfg;
  }

  static Config load(W25q64& flash) {
    Config cfg = {};
    flash.read(FLASH_CONFIG_ADDR, (uint8_t*)&cfg, sizeof(Config));

    if (cfg.magic != FLASH_CONFIG_MAGIC || cfg.crc != computeCrc(cfg)) {
      return defaults();
    }

    return cfg;
  }

  static bool save(W25q64& flash, Config& cfg) {
    cfg.magic = FLASH_CONFIG_MAGIC;
    cfg.version = FLASH_CONFIG_VERSION;
    cfg.crc = computeCrc(cfg);

    flash.eraseSector(FLASH_CONFIG_ADDR);
    flash.writePage(FLASH_CONFIG_ADDR, (const uint8_t*)&cfg, sizeof(Config));

    // Read back and verify
    Config verify = {};
    flash.read(FLASH_CONFIG_ADDR, (uint8_t*)&verify, sizeof(Config));

    return verify.crc == cfg.crc && verify.magic == cfg.magic;
  }

 private:
  static uint16_t computeCrc(const Config& cfg) {
    return crc16((const uint8_t*)&cfg, sizeof(Config) - sizeof(uint16_t));
  }
};

#endif
