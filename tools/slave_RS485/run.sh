#!/usr/bin/env bash
set -euo pipefail
cd "."
python3 -m venv .venv || true
source .venv/bin/activate
pip install -r requirements.txt
exec env RS485_PORT="${RS485_PORT:-/dev/ttyUSB1}" RS485_BAUD="${RS485_BAUD:-115200}" python3 server.py
