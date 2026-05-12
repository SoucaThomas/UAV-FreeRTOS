#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <stdint.h>

#include "state_machine.hpp"

enum class MsgType : uint8_t {
  SENSOR,
  SERVO,
  RADIO,
  STATE,
};

struct SensorMsg {
  static constexpr MsgType TYPE = MsgType::SENSOR;
  float pitch;
  float roll;
  float gyroX, gyroY, gyroZ;
  float accelX, accelY, accelZ;

  uint32_t timestamp;
};

struct ServoMsg {
  static constexpr MsgType TYPE = MsgType::SERVO;
  uint16_t elevator;
  uint16_t aileronL;
  uint16_t aileronR;
  uint16_t rudder;
  uint16_t throttle;

  uint32_t timestamp;
};

struct RadioMsg {
  static constexpr MsgType TYPE = MsgType::RADIO;
  uint8_t data[32];
  bool valid;
  uint32_t timestamp;
};

struct StateMsg {
  static constexpr MsgType TYPE = MsgType::STATE;
  State state;
  bool armed;
  bool radioLinked;
  float batteryVolt;
  uint32_t timestamp;
};

#endif