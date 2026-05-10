# UAV-FreeRTOS

DIY flight controller for an MQ-9 Reaper RC plane (1.2m wingspan). Everything from scratch - bare metal STM32, FreeRTOS, custom radio protocol, and a ground station app. No Betaflight, no ArduPilot, just registers and pain.

## What is this

I'm building a fixed-wing RC plane and writing all the flight controller software myself. The plane runs on a STM32F411 (Black Pill board) with FreeRTOS, talks to the ground over NRF24L01 radio, and reads an MPU6050 gyro for stabilization.

The ground station is a React app that shows the plane on a map, displays telemetry, and takes PS5 controller input.

**Current hardware:**
- STM32F411CE Black Pill
- MPU6500 (came labeled as MPU6050, works the same)
- NRF24L01+PA+LNA (cheap AliExpress clone, channel register is stuck at 0 lol)
- Hobbywing Skywalker 60A V2 ESC
- Robbe Roxxy BL 3545-12 motor
- 4S LiPo

## Project layout

```
firmware/          STM32 firmware (FreeRTOS tasks, startup, linker scripts)
shared/            Drivers and protocol code (used by both aircraft and ground station)
shared/drivers/    SPI, I2C, UART, NRF24L01, MPU6050, PWM drivers
ground-station/    PC app (React + TypeScript + MapLibre)
tests/             Unit tests (Google Test)
```

## Drivers

Wrote all the peripheral drivers from scratch, register level. They're all in `shared/drivers/` as header-only C++ classes.

- **SPI** - talks to the NRF radio (PA5/PA6/PA7, software CS on PA4)
- **I2C** - talks to the gyro (PB6/PB7, 100kHz)
- **UART** - debug output (PA9/PA10, 115200 baud)
- **NRF24L01** - radio driver with register read/write, TX/RX, init sequence
- **MPU6050** - gyro + accel with calibration, outputs in g and deg/s
- **PWM** - timer-based 50Hz for servos and ESC (TIM2/TIM3)

## Sensor fusion

Complementary filter running at 100Hz. Blends gyro (fast, drifts) with accelerometer (slow, noisy) for pitch and roll:

```
pitch = 0.98 * (pitch + gyro * dt) + 0.02 * atan2(ax, az)
```

Works surprisingly well - steady to ~0.05 degrees when sitting still, responds instantly to tilting.

## State machine

The flight controller has 9 states:

```
BOOT -> CALIBRATING -> DISARMED -> ARMED -> MANUAL / STABILIZE
```

Plus `FAILSAFE_SHORT`, `FAILSAFE_LONG`, and `EMERGENCY` for when things go wrong. Can't arm without the gyro calibrated, radio linked, throttle at zero, and battery voltage OK. Auto-disarms after 60s if you arm but never take off.

The whole thing is unit tested with 38 tests covering every transition and edge case.

## Radio protocol

32-byte fixed packets over NRF24L01:

```
[SEQ 1B][TYPE 1B][LEN 1B][PAYLOAD 0-27B][CRC16 2B]
```

Types are HEARTBEAT, ACK, NACK, CMD, TELEMETRY. CRC-16-CCITT catches corrupted packets. The 32 byte size matches the NRF's native payload size so there's no fragmentation.

## Ground station

React + TypeScript app with shadcn/ui. Uses a PS5 controller for input (via browser Gamepad API).

What it does:
- Shows the plane on a dark map (MapLibre + Carto tiles)
- OpenAIP airspace overlay so you can see if you're about to fly somewhere illegal
- Telemetry panel with battery, distance, link status
- Attitude indicator showing pitch/roll
- Stick position display
- Simulation mode where you can fly a virtual plane around with the controller
- Multiple map styles you can switch between
- Toggleable panels for everything

To run it:
```bash
cd ground-station/app
cp .env.example .env    # put your OpenAIP key in here
bun install
bun run dev
```

You need a free API key from [openaip.net](https://www.openaip.net/) for the airspace overlay.

## Building the firmware

You need `arm-none-eabi-gcc` and `dfu-util`.

```bash
make firmware     # build
make flash        # flash over USB (DFU mode)
```

## Running tests

```bash
make tests
```

53 tests - CRC, packet serialization, protocol, and state machine.

## Pin map

| Pin | What |
|-----|------|
| PA0 | ESC (TIM2 CH1) |
| PA1 | Elevator servo (TIM2 CH2) |
| PA3 | NRF CE |
| PA4 | NRF CSN |
| PA5/6/7 | SPI (NRF) |
| PA9/10 | UART (debug) |
| PB0 | Rudder servo (TIM3 CH3) |
| PB4/5 | Aileron servos (TIM3 CH1/2) |
| PB6/7 | I2C (gyro) |
| PC13 | LED |

## What's left to do

- [x] Protocol + drivers
- [x] NRF24L01 radio
- [x] MPU6050 gyro + sensor fusion
- [x] PWM for servos/ESC
- [x] State machine
- [x] Ground station app
- [ ] PID controller
- [ ] Servo mixer
- [ ] Radio link between two boards
- [ ] Ground station MCU (UART-radio bridge)
- [ ] Connect the app to actual hardware over serial
- [ ] GPS
- [ ] Battery monitoring
- [ ] Actually fly the thing

## EU stuff

This falls under EASA Open Category A3 in Romania. Need to register with AACR, do the free online theory test, and fly 150m from buildings/people. The NRF24L01 at 0dBm is well under the EU 100mW limit for 2.4GHz ISM band.
