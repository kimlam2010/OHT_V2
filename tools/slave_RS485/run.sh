#!/usr/bin/env bash
set -euo pipefail
cd "."
python3 -m venv .venv || true
source .venv/bin/activate
pip install -r requirements.txt

# 🔍 AUTO-DETECT CH340 PORT
echo "[INFO] 🔍 Đang tìm CH340 USB-to-Serial converter..."

if [ -z "${RS485_PORT:-}" ]; then
  # Tự động detect CH340
  DETECTED_PORT=$(python3 port_detector.py 2>/dev/null | grep "Recommended port:" | awk '{print $3}')
  
  if [ -n "$DETECTED_PORT" ]; then
    PORT="$DETECTED_PORT"
    echo "[INFO] ✅ Tự động detect: $PORT"
  else
    echo "[ERROR] ❌ Không tìm thấy CH340!"
    echo "[ERROR] 💡 Kiểm tra: lsusb | grep CH340"
    echo "[ERROR] 💡 Hoặc set thủ công: export RS485_PORT=/dev/ttyUSB0"
    exit 1
  fi
else
  PORT="$RS485_PORT"
  echo "[INFO] 📌 Dùng port đã set: $PORT"
fi

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
