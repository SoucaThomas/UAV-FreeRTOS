#include <gtest/gtest.h>
#include <string.h>

#include "packet.h"

TEST(Packet, BuildSetsHeader) {
  const uint8_t payload[] = {0xAA, 0xBB, 0xCC};
  Packet pkt = buildPacket(1, HEARTBEAT, payload);

  EXPECT_EQ(pkt.seq, 1);
  EXPECT_EQ(pkt.type, HEARTBEAT);
  EXPECT_EQ(pkt.length, 3);
}

TEST(Packet, BuildCopiesPayload) {
  const uint8_t payload[] = {0xDE, 0xAD, 0xBE, 0xEF};
  Packet pkt = buildPacket(0, CMD, payload);

  EXPECT_EQ(pkt.payload[0], 0xDE);
  EXPECT_EQ(pkt.payload[1], 0xAD);
  EXPECT_EQ(pkt.payload[2], 0xBE);
  EXPECT_EQ(pkt.payload[3], 0xEF);
}

TEST(Packet, BuildZeroPadsPayload) {
  const uint8_t payload[] = {0x01};
  Packet pkt = buildPacket(0, CMD, payload);

  for (int i = 1; i < MAX_PAYLOAD_SIZE; i++) {
    EXPECT_EQ(pkt.payload[i], 0) << "payload[" << i << "] not zero-padded";
  }
}

TEST(Packet, BuildSetsCRC) {
  const uint8_t payload[] = {0x01, 0x02};
  Packet pkt = buildPacket(0, TELEMETRY, payload);

  uint16_t expected = crc16((const uint8_t*)&pkt, 3 + 2);
  EXPECT_EQ(pkt.crc, expected);
}

TEST(Packet, RoundTrip) {
  const uint8_t payload[] = {0x10, 0x20, 0x30, 0x40, 0x50};
  Packet built = buildPacket(42, CMD, payload);

  // Simulate receiving: cast to raw buffer, then parse
  Packet parsed;
  bool valid = parsePacket((const uint8_t*)&built, parsed);

  EXPECT_TRUE(valid);
  EXPECT_EQ(parsed.seq, 42);
  EXPECT_EQ(parsed.type, CMD);
  EXPECT_EQ(parsed.length, 5);
  EXPECT_EQ(memcmp(parsed.payload, payload, 5), 0);
}

TEST(Packet, ParseDetectsCorruptPayload) {
  const uint8_t payload[] = {0xAA, 0xBB};
  Packet built = buildPacket(0, ACK, payload);

  // Corrupt one byte
  uint8_t buffer[32];
  memcpy(buffer, &built, 32);
  buffer[4] ^= 0x01;

  Packet parsed;
  EXPECT_FALSE(parsePacket(buffer, parsed));
}

TEST(Packet, ParseDetectsCorruptHeader) {
  const uint8_t payload[] = {0x01};
  Packet built = buildPacket(0, HEARTBEAT, payload);

  // Corrupt the type field
  uint8_t buffer[32];
  memcpy(buffer, &built, 32);
  buffer[1] ^= 0xFF;

  Packet parsed;
  EXPECT_FALSE(parsePacket(buffer, parsed));
}

TEST(Packet, ParseRejectsInvalidLength) {
  uint8_t buffer[32] = {};
  buffer[2] = MAX_PAYLOAD_SIZE + 1;  // length field too large

  Packet parsed;
  EXPECT_FALSE(parsePacket(buffer, parsed));
}

TEST(Packet, MaxPayload) {
  const uint8_t payload[MAX_PAYLOAD_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                                             0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
                                             0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B};
  Packet built = buildPacket(255, TELEMETRY, payload);

  Packet parsed;
  EXPECT_TRUE(parsePacket((const uint8_t*)&built, parsed));
  EXPECT_EQ(parsed.length, MAX_PAYLOAD_SIZE);
  EXPECT_EQ(memcmp(parsed.payload, payload, MAX_PAYLOAD_SIZE), 0);
}
