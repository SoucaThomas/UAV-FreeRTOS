# TODO

### Flight critical
- [ ] Servo mixer + PWM output task (connect PID to actual servos)
- [ ] ADC battery voltage monitoring (arming guard)
- [X] Hardware watchdog (IWDG — auto-reset on hang)
- [ ] PID tuning (use serial charts to dial in gains)

### Developer experience
- [X] FreeRTOS runtime stats (CPU % per task)
- [X] Stack overflow detection (`configCHECK_FOR_STACK_OVERFLOW = 2`)
- [ ] Heap usage monitoring
- [ ] Loop timing (measure actual vs expected task duration)
- [ ] Runtime log level switching
- [X] Boot reason tracking (clean boot vs watchdog vs hard fault)
- [X] HardFault handler (print registers on crash)

### Data logging
- [X] W25Q64 flash driver (SPI, PA4 CS)
- [X] Flight logger task (subscribe to bus, write to flash)
- [X] Log dump command (read flash back over UART)
- [X] Flash memory map (config/crash/blackbox sectors)

### Testing & safety
- [X] Built-in self test (BIST) — check all peripherals on boot
- [ ] Sensor simulation mode (fake data over UART for PID testing)
- [X] Parameter storage in flash (PID gains, servo cal, settings persist across reboots)
- [ ] Compile-time asserts for config validation
- [X] Skip sensor/control tasks when IMU not detected

## Radio (needs 2nd Blackpill + NRF)
- [X] Radio TX/RX tasks
- [ ] Command handler (arm/disarm, mode switch, stick input)
- [ ] Telemetry (send sensor/state data back to ground)
- [X] Heartbeat + signal loss detection
- [ ] Bidirectional serial protocol (ground station ↔ MCU)

## Ground station app
- [ ] Bidirectional serial commands (send PID gains, arm/disarm from app)
- [ ] Task CPU usage chart (from runtime stats)
- [ ] Parameter editor panel (PID gains, servo limits)
- [ ] Flight log viewer (load dumped flash data, plot it)

## Hardware
- [ ] Move NRF CSN to PB12 (free PA4 for flash)
- [ ] Solder motor connections properly
- [ ] Get servos + mount on airframe
- [ ] Second Blackpill + NRF for ground station MCU
- [ ] GPS module
- [ ] Build the airframe

## Build system
- [X] Nix flake for reproducible toolchain
- [X] Multi-target build (make uav / make ground)
- [X] ST-Link flash + debug support
- [X] VS Code Cortex-Debug integration
- [X] Post-build disassembly + hex generation

## Later
- [ ] Frequency hopping
- [ ] GPS waypoint navigation
- [ ] Altitude hold (needs barometer)
- [ ] Return to home
- [ ] OTA reflash over UART
- [ ] Camera pod (Pi Zero 2W)
