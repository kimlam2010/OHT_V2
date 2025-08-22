#!/usr/bin/env bash
set -euo pipefail
HOST=${HOST:-http://localhost:8000}
TOKEN=${TOKEN:-}
AHDR=()
if [ -n "$TOKEN" ]; then AHDR=(-H "Authorization: Bearer $TOKEN"); fi

echo "HEALTH" && curl -s ${HOST}/health/ | jq . || true

echo "LOGIN" && curl -s -X POST ${HOST}/api/v1/auth/login \
  -H 'Content-Type: application/json' \
  -d '{"username":"admin","password":"admin"}' | jq . || true

echo "CONFIG GET" && curl -s ${AHDR[@]} ${HOST}/api/v1/config/ | jq . || true

echo "CONFIG HISTORY" && curl -s ${AHDR[@]} "${HOST}/api/v1/config/history?limit=5" | jq . || true

echo "TELEMETRY CURRENT" && curl -s ${HOST}/api/v1/telemetry/current | jq . || true

echo "TELEMETRY RATE 20" && curl -s -X POST ${HOST}/api/v1/telemetry/rate?hz=20 | jq . || true
