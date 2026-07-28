#!/bin/bash

# Get the absolute path to the firmware directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
FIRMWARE_ELF="$DIR/../firmware/firmware.elf"

echo "Starting QEMU Cortex-M7 Emulator..."
echo "Waiting for ROS 2 ros2_control to connect on port 8888..."

qemu-system-arm -M mps2-an500 -kernel "$FIRMWARE_ELF" -nographic -serial tcp:127.0.0.1:8888,server,wait