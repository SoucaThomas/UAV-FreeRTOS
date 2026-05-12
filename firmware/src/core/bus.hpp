#ifndef BUS_HPP
#define BUS_HPP

#include "FreeRTOS.h"
#include "messages.hpp"
#include "queue.h"

class Bus {
 private:
  QueueHandle_t sensorQ;
  QueueHandle_t servoQ;
  QueueHandle_t radioQ;
  QueueHandle_t stateQ;

 public:
  void init() {
    sensorQ = xQueueCreate(1, sizeof(SensorMsg));
    servoQ = xQueueCreate(1, sizeof(ServoMsg));
    radioQ = xQueueCreate(1, sizeof(RadioMsg));
    stateQ = xQueueCreate(1, sizeof(StateMsg));
  }

  // Publish
  void publish(const SensorMsg& msg) { xQueueOverwrite(sensorQ, &msg); }
  void publish(const ServoMsg& msg) { xQueueOverwrite(servoQ, &msg); }
  void publish(const RadioMsg& msg) { xQueueOverwrite(radioQ, &msg); }
  void publish(const StateMsg& msg) { xQueueOverwrite(stateQ, &msg); }

  // Peek
  bool peek(SensorMsg& msg) { return xQueuePeek(sensorQ, &msg, 0) == pdTRUE; }
  bool peek(ServoMsg& msg) { return xQueuePeek(servoQ, &msg, 0) == pdTRUE; }
  bool peek(RadioMsg& msg) { return xQueuePeek(radioQ, &msg, 0) == pdTRUE; }
  bool peek(StateMsg& msg) { return xQueuePeek(stateQ, &msg, 0) == pdTRUE; }

  // Wait
  bool wait(SensorMsg& msg, uint32_t timeoutMs = 100) {
    return xQueuePeek(sensorQ, &msg, pdMS_TO_TICKS(timeoutMs)) == pdTRUE;
  }
  bool wait(ServoMsg& msg, uint32_t timeoutMs = 100) {
    return xQueuePeek(servoQ, &msg, pdMS_TO_TICKS(timeoutMs)) == pdTRUE;
  }
  bool wait(RadioMsg& msg, uint32_t timeoutMs = 100) {
    return xQueuePeek(radioQ, &msg, pdMS_TO_TICKS(timeoutMs)) == pdTRUE;
  }
  bool wait(StateMsg& msg, uint32_t timeoutMs = 100) {
    return xQueuePeek(stateQ, &msg, pdMS_TO_TICKS(timeoutMs)) == pdTRUE;
  }
};

extern Bus bus;

#endif