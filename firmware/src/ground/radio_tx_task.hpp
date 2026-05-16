#ifndef RADIO_TX_TASK_HPP
#define RADIO_TX_TASK_HPP

#include "crc.h"
#include "drivers/nrf24l01.hpp"
#include "drivers/spi.hpp"
#include "log.hpp"
#include "packet.h"
#include "protocol.h"
#include "stm32f411xe.h"
#include "task.hpp"

class RadioTxTask : public Task<RadioTxTask> {
 private:
  Spi spi;
  Nrf24l01 nrf;
  Logger log;

 public:
  RadioTxTask() : spi(SPI1, GPIOA, 4), nrf(&spi, GPIOA, 3), log("RadioTX") {}

  void run() {
    nrf.setSpi(&spi);

    nrf.init(false, 0);
    log.info("NRF init");

    uint8_t seq = 0;

    while (true) {
      uint8_t payload[] = {seq};

      Packet pkt = buildPacket(seq, HEARTBEAT, payload);
      nrf.send((uint8_t*)&pkt, 32);

      log.info("TX seq=%", seq);
      seq++;
      delay(250);
    }
  }
};

#endif