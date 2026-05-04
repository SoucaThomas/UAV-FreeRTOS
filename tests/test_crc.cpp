#include <gtest/gtest.h>

#include "crc.h"

TEST(CRC16, KnownVector) {
  const uint8_t data[] = "123456789";
  EXPECT_EQ(crc16(data, 9), 0x29B1);
}

TEST(CRC16, EmptyData) { EXPECT_EQ(crc16(nullptr, 0), 0xFFFF); }

TEST(CRC16, SingleByte) {
  const uint8_t data[] = {0x00};
  uint16_t result = crc16(data, 1);
  // just verify it doesn't crash and returns something other than init
  EXPECT_NE(result, 0xFFFF);
}

TEST(CRC16, DifferentDataDifferentCRC) {
  const uint8_t a[] = {0x01, 0x02, 0x03};
  const uint8_t b[] = {0x01, 0x02, 0x04};
  EXPECT_NE(crc16(a, 3), crc16(b, 3));
}

TEST(CRC16, SingleBitFlipDetected) {
  uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
  uint16_t original = crc16(data, 4);

  data[2] ^= 0x01;  // flip one bit
  uint16_t corrupted = crc16(data, 4);

  EXPECT_NE(original, corrupted);
}
