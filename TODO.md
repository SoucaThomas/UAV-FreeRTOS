# TODO

### Flight critical
- [ ] Servo mixer + PWM output task (connect PID to actual servos)
- [ ] ADC battery voltage monitoring (arming guard)
- [ ] Hardware watchdog (IWDG — auto-reset on hang)
- [ ] PID tuning (use serial charts to dial in gains)

### Developer experience
- [ ] FreeRTOS runtime stats (CPU % per task)
- [ ] Stack overflow detection (`configCHECK_FOR_STACK_OVERFLOW = 2`)
- [ ] Heap usage monitoring
- [ ] Loop timing (measure actual vs expected task duration)
- [ ] Runtime log level switching
- [ ] Boot reason tracking (clean boot vs watchdog vs hard fault)
- [ ] HardFault handler (print registers on crash)

### Data logging
- [X] W25Q64 flash driver (SPI, PA4 CS)
- [X] Flight logger task (subscribe to bus, write to flash)
- [X] Log dump command (read flash back over UART)

### Testing & safety
- [ ] Built-in self test (BIST) — check all peripherals on boot
- [ ] Sensor simulation mode (fake data over UART for PID testing)
- [ ] Parameter storage in flash (PID gains, servo cal, settings persist across reboots)
- [ ] Compile-time asserts for config validation

## Radio (needs 2nd Blackpill + NRF)
- [ ] Radio TX/RX tasks
- [ ] Command handler (arm/disarm, mode switch, stick input)
- [ ] Telemetry (send sensor/state data back to ground)
- [ ] Heartbeat + signal loss detection
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

## Later
- [ ] Frequency hopping
- [ ] GPS waypoint navigation
- [ ] Altitude hold (needs barometer)
- [ ] Return to home
- [ ] OTA reflash over UART
