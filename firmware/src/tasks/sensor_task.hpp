#ifndef SENSOR_TASK_HPP
#define SENSOR_TASK_HPP

#include <math.h>

#include "drivers/i2c.hpp"
#include "drivers/mpu6050.hpp"
#include "drivers/uart.hpp"
#include "led.hpp"
#include "task.hpp"

class SensorTask : public Task<SensorTask> {
 private:
  I2c i2c;
  Mpu6050 mpu;
  Led led;
  Uart uart;

  float pitch;
  float roll;

 public:
  SensorTask()
      : i2c(I2C1), mpu(&i2c), led(GPIOC, 13), uart(USART1, 115200),
        pitch(0.0f), roll(0.0f) {};

  void run() {
    mpu.setI2c(&i2c);

    uart.print("Sensor fusion task\r\n");
    delay(100);

    uint8_t who = mpu.init();
    uart.print("WHO_AM_I: 0x");
    uart.printHex(who);
    uart.print("\r\n");

    uart.print("Calibrating (keep still)...\r\n");
    mpu.calibrate(500);
    uart.print("Done\r\n\r\n");

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

      uart.print("P:");
      uart.printFloat(pitch, 2);
      uart.print(" R:");
      uart.printFloat(roll, 2);
      uart.print("\r\n");

      led.toggle();
      delay(10);
    }
  }
};

#endif
