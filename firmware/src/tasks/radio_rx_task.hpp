#ifndef RADIO_RX_TASK_HPP
#define RADIO_RX_TASK_HPP

#include "core/bus.hpp"
#include "drivers/nrf24l01.hpp"
#include "drivers/spi.hpp"
#include "log.hpp"
#include "packet.h"
#include "protocol.h"
#include "stm32f411xe.h"
#include "task.hpp"

class RadioRxTask : public Task<RadioRxTask> {
 private:
  Spi spi;
  Nrf24l01 nrf;
  Logger log;

 public:
  RadioRxTask() : spi(SPI1, GPIOA, 4), nrf(&spi, GPIOA, 3), log("RadioRX") {}

  void run() {
    nrf.setSpi(&spi);

    nrf.init(true, 0);
    log.info("NRF init");

    uint32_t lastValidMs = xTaskGetTickCount();
    bool linkUp = false;

    while (true) {
      if (nrf.available()) {
        uint8_t buffer[32];
        nrf.receive(buffer, 32);

        Packet pkt;
        if (parsePacket(buffer, pkt)) {
          lastValidMs = xTaskGetTickCount();

          RadioMsg msg = {};
          memcpy(msg.data, buffer, 32);

          msg.valid = true;
          msg.timestamp = lastValidMs;
          bus.publish(msg);

          if (!linkUp) {
            log.info("link up");
            linkUp = true;
          }
          log.debug("RX seq=%", pkt.seq);
        }
      }

      uint32_t now = xTaskGetTickCount();
      if ((now - lastValidMs) > 500 && linkUp) {
        RadioMsg msg = {};
        msg.valid = false;
        msg.timestamp = now;
        bus.publish(msg);

        log.warn("link lost");
        linkUp = false;
      }

      delay(10);
    }
  }
};

#endif