#!/bin/bash
# Build script for LiftSense firmware
# Usage: ./build.sh [clean]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ZEPHYR_WORKSPACE="${HOME}/zephyrproject"

# Activate virtual environment
source "${ZEPHYR_WORKSPACE}/.venv/bin/activate"

# Change to zephyr workspace (required for west build)
cd "${ZEPHYR_WORKSPACE}"

# Check for clean build
if [ "$1" == "clean" ]; then
    echo "Clean build requested..."
    west build -b xiao_ble/nrf52840/sense "${SCRIPT_DIR}" -p
else
    west build -b xiao_ble/nrf52840/sense "${SCRIPT_DIR}"
fi

echo ""
echo "Build complete!"
echo "Output: ${ZEPHYR_WORKSPACE}/build/zephyr/zephyr.uf2"
echo ""
echo "To flash: Copy zephyr.uf2 to the XIAO bootloader drive"
