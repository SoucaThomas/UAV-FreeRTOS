#ifndef SENSOR_TASK_HPP
#define SENSOR_TASK_HPP

#include <math.h>

#include "core/bus.hpp"
#include "core/messages.hpp"
#include "drivers/i2c.hpp"
#include "drivers/mpu6050.hpp"
#include "log.hpp"
#include "task.hpp"

class SensorTask : public Task<SensorTask> {
 private:
  I2c i2c;
  Mpu6050 mpu;
  Logger log;

  float pitch;
  float roll;

 public:
  SensorTask() : i2c(I2C1), mpu(&i2c), log("Sensor"), pitch(0.0f), roll(0.0f){};

  void run() {
    mpu.setI2c(&i2c);
    log.info("Sensor task started");

    delay(100);

    log.info("Initializing MPU...");
    uint8_t who = mpu.init();
    log.info("WHO_AM_I: 0x%x", who);

    log.info("Calibrating...");
    mpu.calibrate(500);
    log.info("Calibration done");

    const float dt = 0.01f;  // 10ms loop = 100Hz

    while (true) {
      float ax = mpu.accelX();
      float ay = mpu.accelY();
      float az = mpu.accelZ();
      float gx = mpu.gyroX();
      float gy = mpu.gyroY();

      // Gyro integration
      float gyroPitch = pitch + gx * dt;
      float gyroRoll = roll + gy * dt;

      // Accel angles
      float accelPitch = atan2f(ax, az) * (180.0f / 3.14159265f);
      float accelRoll = atan2f(ay, az) * (180.0f / 3.14159265f);

      // Complementary filter: 98% gyro + 2% accel
      pitch = 0.98f * gyroPitch + 0.02f * accelPitch;
      roll = 0.98f * gyroRoll + 0.02f * accelRoll;

      SensorMsg msg;
      msg.pitch = pitch;
      msg.roll = roll;
      msg.gyroX = gx;
      msg.gyroY = gy;
      msg.gyroZ = mpu.gyroZ();
      msg.accelX = ax;
      msg.accelY = ay;
      msg.accelZ = az;
      msg.timestamp = xTaskGetTickCount();
      bus.publish(msg);

      delay(10);
    }
  }
};

#endif
