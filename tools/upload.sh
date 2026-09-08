#!/bin/zsh

set -euo pipefail

SCRIPT_DIR=${0:A:h}
PROJECT_DIR=${SCRIPT_DIR:h}
BUILD_DIR="${PROJECT_DIR}/.arduino/build"
FQBN="${FQBN:-esp32:esp32:esp32:PartitionScheme=huge_app}"
PORT="${PORT:-/dev/cu.usbserial-0001}"

"${SCRIPT_DIR}/compile.sh"

arduino-cli upload \
  --fqbn "${FQBN}" \
  --port "${PORT}" \
  --input-dir "${BUILD_DIR}" \
  "${PROJECT_DIR}"
