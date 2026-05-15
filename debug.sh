#!/usr/bin/env bash
#
# Debug script for STM32F411 Black Pill via ST-Link
#
# Usage:
#   ./debug.sh                    # defaults: firmware/build/blackpill.elf, stlink
#   ./debug.sh <elf> <debugger>   # debugger: stlink | jlink
#

set -euo pipefail

ELF="${1:-firmware/build/blackpill.elf}"
DEBUGGER="${2:-stlink}"
GDB="arm-none-eabi-gdb"

SERVER_PID=""

cleanup() {
    if [ -n "$SERVER_PID" ]; then
        echo "Stopping debug server (PID $SERVER_PID)..."
        kill "$SERVER_PID" 2>/dev/null || true
        wait "$SERVER_PID" 2>/dev/null || true
    fi
}
trap cleanup EXIT INT TERM

if [ ! -f "$ELF" ]; then
    echo "Error: ELF file not found: $ELF"
    echo "Run 'make firmware' first."
    exit 1
fi

case "$DEBUGGER" in
    stlink)
        echo "Starting OpenOCD with ST-Link..."
        openocd \
            -f interface/stlink.cfg \
            -f target/stm32f4x.cfg \
            -c "init" \
            -c "reset halt" &
        SERVER_PID=$!
        GDB_PORT=3333
        ;;
    jlink)
        echo "Starting J-Link GDB Server..."
        JLinkGDBServerCLExe \
            -device STM32F411CE \
            -if SWD \
            -speed 4000 \
            -port 2331 &
        SERVER_PID=$!
        GDB_PORT=2331
        ;;
    *)
        echo "Unknown debugger: $DEBUGGER (use 'stlink' or 'jlink')"
        exit 1
        ;;
esac

# Wait for server to start
sleep 1

echo "Connecting GDB to localhost:$GDB_PORT..."
$GDB "$ELF" \
    -ex "target extended-remote localhost:$GDB_PORT" \
    -ex "monitor reset halt" \
    -ex "load" \
    -ex "break main" \
    -ex "continue"
