#!/bin/zsh

set -euo pipefail

SCRIPT_DIR=${0:A:h}
PROJECT_DIR=${SCRIPT_DIR:h}
FQBN="${FQBN:-esp32:esp32:esp32}"
PORT="${PORT:-/dev/cu.usbserial-0001}"
MONITOR_BAUD="${MONITOR_BAUD:-115200}"

arduino-cli monitor \
  --fqbn "${FQBN}" \
  --port "${PORT}" \
  --config "baudrate=${MONITOR_BAUD}"
