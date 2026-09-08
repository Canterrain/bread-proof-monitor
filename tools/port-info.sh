#!/bin/zsh

set -euo pipefail

PORT="${PORT:-/dev/cu.usbserial-0001}"

arduino-cli monitor --port "${PORT}" --describe
