#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <string.h>

#include "crc.h"
#include "protocol.h"

template <uint8_t N>
Packet buildPacket(uint8_t seq, PacketTypes packetType, const uint8_t (&payload)[N]) {
  static_assert(N <= MAX_PAYLOAD_SIZE, "Payload exceeds max size");
  Packet packet;

  // Header
  packet.seq = seq;
  packet.type = packetType;
  packet.length = N;

  // Payload
  memcpy(packet.payload, payload, N);
  memset(packet.payload + N, 0, MAX_PAYLOAD_SIZE - N);

  // CRC over header + payload
  packet.crc = crc16((const uint8_t*)&packet, 3 + N);

  return packet;
}

inline bool parsePacket(const uint8_t* buffer, Packet& pkt) {
  // Copy packet to buffer
  memcpy(&pkt, buffer, sizeof(Packet));

  // Check max size
  if (pkt.length > MAX_PAYLOAD_SIZE) {
    return false;
  }

  // Check CRC over header + payload
  uint16_t crc = crc16(buffer, 3 + pkt.length);

  return pkt.crc == crc;
}

#endif