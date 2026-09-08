#!/bin/zsh

set -euo pipefail

SCRIPT_DIR=${0:A:h}
PROJECT_DIR=${SCRIPT_DIR:h}
BUILD_DIR="${PROJECT_DIR}/.arduino/build"
FQBN="${FQBN:-esp32:esp32:esp32:PartitionScheme=huge_app}"

arduino-cli compile \
  --fqbn "${FQBN}" \
  --build-path "${BUILD_DIR}" \
  "${PROJECT_DIR}"
