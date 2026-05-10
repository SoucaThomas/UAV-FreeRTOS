# UAV-FreeRTOS

A custom flight controller and ground station for a 1.2m wingspan MQ-9 Reaper RC plane, built from scratch on STM32 + FreeRTOS.

## Overview

This project implements a complete UAV system — bare-metal firmware, communication protocol, sensor fusion, and a ground station PC app — with no off-the-shelf flight controller software.

**Hardware:**
- STM32F411CE (Black Pill) — ARM Cortex-M4 with hardware FPU
- MPU6050/6500 IMU — accelerometer + gyroscope
- NRF24L01+PA+LNA — 2.4GHz radio link
- Hobbywing Skywalker 60A ESC + brushless motor
- Servos for control surfaces (aileron, elevator, rudder)

## Project Structure

```
UAV-FreeRTOS/
├── firmware/               Aircraft MCU firmware (FreeRTOS)
│   ├── src/
│   │   ├── main.cpp        Entry point
│   │   ├── core/           Flight state machine
│   │   └── tasks/          FreeRTOS task implementations
│   ├── include/            FreeRTOS config, task base class, LED driver
│   ├── external/           FreeRTOS kernel + CMSIS headers
│   ├── linker/             STM32 linker scripts
│   └── startup/            Cortex-M4 startup assembly
├── shared/                 Code shared between aircraft + ground station
│   ├── drivers/            Hardware drivers (SPI, I2C, UART, NRF, MPU, PWM)
│   ├── crc.h               CRC-16-CCITT
│   ├── protocol.h          Packet type definitions
│   └── packet.h            Packet serialization
├── ground-station/
│   ├── app/                PC ground station (React + TypeScript)
│   └── mcu/                Ground station MCU firmware (planned)
├── tests/                  Unit tests (Google Test)
└── .github/workflows/      CI/CD (auto-test + auto-format)
```

## Firmware

### Drivers

All drivers are header-only C++ classes in `shared/drivers/`:

| Driver | Interface | Pins | Description |
|--------|-----------|------|-------------|
| `spi.hpp` | SPI1 | PA5/PA6/PA7/PA4 | Master, configurable prescaler |
| `i2c.hpp` | I2C1 | PB6/PB7 | Master, 100kHz |
| `uart.hpp` | USART1 | PA9/PA10 | Debug serial, 115200 baud |
| `nrf24l01.hpp` | SPI | PA3 (CE), PA4 (CS) | 2.4GHz radio, 32-byte packets |
| `mpu6050.hpp` | I2C | 0x68 | 6-axis IMU, calibration, float output |
| `pwm.hpp` | TIM2/TIM3 | PA0-3, PB0-5 | 50Hz servo/ESC PWM |

### Sensor Fusion

Complementary filter at 100Hz combining gyroscope (short-term accuracy) and accelerometer (long-term stability) for pitch and roll estimation:

```
pitch = 0.98 * (pitch + gyro * dt) + 0.02 * accel_angle
```

### State Machine

9-state flight controller state machine with safety guards:

```
BOOT -> CALIBRATING -> DISARMED -> ARMED -> MANUAL <-> STABILIZE
                                     |         |           |
                                     |    FAILSAFE_SHORT --+
                                     |         |
                                     |    FAILSAFE_LONG
                                     |
                                  EMERGENCY (from any state)
```

**Arming requires:** gyro calibrated, radio link active, throttle at idle, battery above minimum voltage.

**Failsafe:** Two-stage radio loss handling — wings level + cruise throttle after 100ms, motor cut + glide after 10s.

### Communication Protocol

Fixed 32-byte packets over NRF24L01:

```
[SEQ 1B] [TYPE 1B] [LEN 1B] [PAYLOAD 0-27B] [CRC16 2B]
```

Packet types: `HEARTBEAT`, `ACK`, `NACK`, `CMD`, `TELEMETRY`

CRC-16-CCITT (polynomial 0x1021) for corruption detection.

## Ground Station

React + TypeScript desktop app for flight monitoring and control.

**Tech stack:** Vite, Tailwind CSS v4, shadcn/ui, MapLibre GL (mapcn), Tauri

**Features:**
- PS5 controller input via Gamepad API
- Real-time map with aircraft position and flight trail
- OpenAIP airspace overlay with zone classification
- Telemetry display (attitude, battery, distance, radio link)
- Attitude indicator and stick position visualization
- Flight simulation mode (controller-driven)
- Multiple map styles (dark, light, satellite, OSM)
- Configurable airspace overlay opacity

### Setup

```bash
cd ground-station/app
cp .env.example .env        # Add your OpenAIP API key
bun install
bun run dev                 # http://localhost:5173
```

Get a free API key at [openaip.net](https://www.openaip.net/) for airspace data.

## Building

### Prerequisites

- `arm-none-eabi-gcc` — ARM cross-compiler
- `dfu-util` — USB DFU flashing tool
- `bun` — JavaScript runtime (for ground station)
- Google Test — for unit tests

### Firmware

```bash
make firmware               # Build
make flash                  # Flash via DFU
make flash-stlink           # Flash via ST-Link
```

### Tests

```bash
make tests                  # Build and run all tests
```

53 unit tests covering CRC, packet serialization, and state machine transitions.

### Ground Station

```bash
cd ground-station/app
bun install
bun run dev                 # Development server
bun run build               # Production build
```

## Pin Map

| Pin | Function | Connected To |
|-----|----------|-------------|
| PA0 | TIM2 CH1 | ESC signal |
| PA1 | TIM2 CH2 | Servo (elevator) |
| PA3 | GPIO | NRF24L01 CE |
| PA4 | GPIO | NRF24L01 CSN |
| PA5 | SPI1 SCK | NRF24L01 SCK |
| PA6 | SPI1 MISO | NRF24L01 MISO |
| PA7 | SPI1 MOSI | NRF24L01 MOSI |
| PA9 | USART1 TX | USB-Serial adapter |
| PA10 | USART1 RX | USB-Serial adapter |
| PB0 | TIM3 CH3 | Servo (rudder) |
| PB4 | TIM3 CH1 | Servo (aileron L) |
| PB5 | TIM3 CH2 | Servo (aileron R) |
| PB6 | I2C1 SCL | MPU6050 SCL |
| PB7 | I2C1 SDA | MPU6050 SDA |
| PC13 | GPIO | Onboard LED |

## Roadmap

- [x] Communication protocol (CRC, packets, serialization)
- [x] SPI, I2C, UART drivers
- [x] NRF24L01 radio driver
- [x] MPU6050/6500 IMU driver + sensor fusion
- [x] PWM driver (servos + ESC)
- [x] Flight state machine
- [x] Ground station app with map + airspace
- [ ] PID controller (pitch, roll, yaw stabilization)
- [ ] Servo mixer (PID output to control surfaces)
- [ ] Radio TX/RX tasks (NRF24L01 packet exchange)
- [ ] Ground station MCU firmware (UART-radio bridge)
- [ ] Serial communication (ground station app to MCU)
- [ ] GPS integration + waypoint navigation
- [ ] Battery voltage monitoring (ADC)
- [ ] Frequency hopping
- [ ] Data logging

## License

This project is for educational and personal use.
