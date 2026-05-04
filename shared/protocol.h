#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define MAX_PAYLOAD_SIZE 27

typedef enum {
  HEARTBEAT = 0x01,
  ACK = 0x02,
  NACK = 0x03,
  CMD = 0x10,
  TELEMETRY = 0x20
} PacketTypes;

/*
    Byte layout (max 32 bytes total):
    [SEQ 1B] [TYPE 1B] [LEN 1B] [PAYLOAD 0-27B] [CRC16 2B]
*/
typedef struct __attribute__((packed)) {
  // Header
  uint8_t seq;
  uint8_t type;
  uint8_t length;

  // Payload
  uint8_t payload[MAX_PAYLOAD_SIZE];

  // Checksum
  uint16_t crc;
} Packet;

#endif