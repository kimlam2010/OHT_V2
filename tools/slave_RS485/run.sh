#!/usr/bin/env bash
set -euo pipefail
cd "."
python3 -m venv .venv || true
source .venv/bin/activate
pip install -r requirements.txt

# Free the serial port if locked
PORT="${RS485_PORT:-/dev/ttyUSB1}"
echo "[INFO] Preparing serial port $PORT"

# Kill previous simulator instances
pkill -f "server.py" || true

# Kill any process using the port
if command -v fuser >/dev/null 2>&1; then
  fuser -k "$PORT" || true
fi
if command -v lsof >/dev/null 2>&1; then
  PIDS=$(lsof -t "$PORT" || true)
  if [ -n "${PIDS}" ]; then
    kill -9 ${PIDS} || true
  fi
fi

# Stop ModemManager if it is grabbing USB serial (uses provided sudo password)
if systemctl is-active --quiet ModemManager.service; then
  echo orangepi | sudo -S systemctl stop ModemManager.service || true
fi

sleep 1

exec env RS485_PORT="$PORT" RS485_BAUD="${RS485_BAUD:-115200}" python3 server.py
