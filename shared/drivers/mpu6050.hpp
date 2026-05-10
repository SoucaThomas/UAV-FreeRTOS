#ifndef MPU6050_HPP
#define MPU6050_HPP

#include "drivers/i2c.hpp"

// Register addresses
constexpr uint8_t MPU_ADDR = 0x68;
constexpr uint8_t MPU_WHO_AM_I = 0x75;
constexpr uint8_t MPU_PWR_MGMT_1 = 0x6B;
constexpr uint8_t MPU_SMPLRT_DIV = 0x19;
constexpr uint8_t MPU_CONFIG = 0x1A;
constexpr uint8_t MPU_GYRO_CONFIG = 0x1B;
constexpr uint8_t MPU_ACCEL_CONFIG = 0x1C;
constexpr uint8_t MPU_ACCEL_XOUT_H = 0x3B;
constexpr uint8_t MPU_GYRO_XOUT_H = 0x43;

class Mpu6050 {
 private:
  I2c* i2c;

  // Calibration Offsets
  int16_t gxOff, gyOff, gzOff;
  int16_t axOff, ayOff, azOff;

  int16_t readWord(uint8_t reg) {
    uint8_t high = i2c->read(MPU_ADDR, reg);
    uint8_t low = i2c->read(MPU_ADDR, reg + 1);

    return (uint16_t)((high << 8) + low);
  }

 public:
  void setI2c(I2c* newI2c) { i2c = newI2c; }

  Mpu6050(I2c* i2c) : i2c(i2c), gxOff(0), gyOff(0), gzOff(0), axOff(0), ayOff(0), azOff(0) {}

  uint8_t init() {
    // Wake up
    i2c->write(MPU_ADDR, MPU_PWR_MGMT_1, 0x00);

    // Sample rate = 1kHz
    i2c->write(MPU_ADDR, MPU_SMPLRT_DIV, 0x00);

    // DLPF = 44Hz bandwidth -> smooths out noise
    i2c->write(MPU_ADDR, MPU_CONFIG, 0x03);

    // Gyro +- 500 deg/s
    i2c->write(MPU_ADDR, MPU_GYRO_CONFIG, 0x08);

    // Accel +- 4g
    i2c->write(MPU_ADDR, MPU_ACCEL_CONFIG, 0x08);

    return i2c->read(MPU_ADDR, MPU_WHO_AM_I);
  }

  void calibrate(uint16_t samples) {
    int32_t sgx = 0, sgy = 0, sgz = 0;
    int32_t sax = 0, say = 0, saz = 0;

    for (uint16_t i = 0; i < samples; i++) {
      sax += readWord(MPU_ACCEL_XOUT_H);
      say += readWord(MPU_ACCEL_XOUT_H + 2);
      saz += readWord(MPU_ACCEL_XOUT_H + 4);
      sgx += readWord(MPU_GYRO_XOUT_H);
      sgy += readWord(MPU_GYRO_XOUT_H + 2);
      sgz += readWord(MPU_GYRO_XOUT_H + 4);
    }

    axOff = sax / samples;
    ayOff = say / samples;
    azOff = (saz / samples) - 8192;  // subtract 1g (at ±4g: 8192 LSB/g)
    gxOff = sgx / samples;
    gyOff = sgy / samples;
    gzOff = sgz / samples;
  }

  // Accel in g (at ±4g: 8192 LSB/g)
  float accelX() { return (float)(readWord(MPU_ACCEL_XOUT_H) - axOff) / 8192.0f; }
  float accelY() { return (float)(readWord(MPU_ACCEL_XOUT_H + 2) - ayOff) / 8192.0f; }
  float accelZ() { return (float)(readWord(MPU_ACCEL_XOUT_H + 4) - azOff) / 8192.0f; }

  // Gyro in deg/s (at ±500: 65.5 LSB per deg/s)
  float gyroX() { return (float)(readWord(MPU_GYRO_XOUT_H) - gxOff) / 65.5f; }
  float gyroY() { return (float)(readWord(MPU_GYRO_XOUT_H + 2) - gyOff) / 65.5f; }
  float gyroZ() { return (float)(readWord(MPU_GYRO_XOUT_H + 4) - gzOff) / 65.5f; }
};

#endif