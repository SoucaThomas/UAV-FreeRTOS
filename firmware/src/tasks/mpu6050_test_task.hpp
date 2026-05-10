#ifndef MPU6050_TEST_TASK_HPP
#define MPU6050_TEST_TASK_HPP

#include "drivers/i2c.hpp"
#include "drivers/uart.hpp"
#include "led.hpp"
#include "task.hpp"

// MPU6050 registers
constexpr uint8_t MPU_ADDR = 0x68;       // AD0 pin to GND
constexpr uint8_t MPU_WHO_AM_I = 0x75;   // should return 0x68
constexpr uint8_t MPU_PWR_MGMT_1 = 0x6B; // power management
constexpr uint8_t MPU_ACCEL_XOUT_H = 0x3B;
constexpr uint8_t MPU_TEMP_OUT_H = 0x41;
constexpr uint8_t MPU_GYRO_XOUT_H = 0x43;

class Mpu6050TestTask : public Task<Mpu6050TestTask> {
 private:
  I2c i2c;
  Led led;
  Uart uart;

  int16_t readWord(uint8_t reg) {
    uint8_t high = i2c.read(MPU_ADDR, reg);
    uint8_t low = i2c.read(MPU_ADDR, reg + 1);
    return (int16_t)((high << 8) | low);
  }

 public:
  Mpu6050TestTask() : i2c(I2C1), led(GPIOC, 13), uart(USART1, 115200) {};

  void run() {
    uart.print("MPU6050 test\r\n");
    delay(100);

    // Check WHO_AM_I
    uint8_t who = i2c.read(MPU_ADDR, MPU_WHO_AM_I);
    uart.print("WHO_AM_I: 0x");
    uart.printHex(who);
    if (who == 0x68) {
      uart.print(" [MPU6050]\r\n");
    } else if (who == 0x70) {
      uart.print(" [MPU6500]\r\n");
    } else {
      uart.print(" [BAD - check wiring]\r\n");
    }

    // Wake up MPU6050 (it starts in sleep mode)
    i2c.write(MPU_ADDR, MPU_PWR_MGMT_1, 0x00);
    delay(100);

    uart.print("Reading sensors...\r\n\r\n");

    while (true) {
      int16_t ax = readWord(MPU_ACCEL_XOUT_H);
      int16_t ay = readWord(MPU_ACCEL_XOUT_H + 2);
      int16_t az = readWord(MPU_ACCEL_XOUT_H + 4);

      int16_t gx = readWord(MPU_GYRO_XOUT_H);
      int16_t gy = readWord(MPU_GYRO_XOUT_H + 2);
      int16_t gz = readWord(MPU_GYRO_XOUT_H + 4);

      uart.print("A: ");
      uart.printInt(ax);
      uart.print(" ");
      uart.printInt(ay);
      uart.print(" ");
      uart.printInt(az);

      uart.print("  G: ");
      uart.printInt(gx);
      uart.print(" ");
      uart.printInt(gy);
      uart.print(" ");
      uart.printInt(gz);
      uart.print("\r\n");

      led.toggle();
      delay(500);
    }
  }
};

#endif
