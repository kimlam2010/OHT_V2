#!/bin/bash
# Measure E-Stop latency using journalctl timestamps and a trigger marker
# Assumes firmware logs include lines:
#   "E-Stop TRIGGERED!" and downstream "Emergency handling activated" or state transition

set -e

BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

SERVICE="oht50.service"

echo -e "${BLUE}=== E-Stop Latency Measurement ===${NC}"
echo "This test reads journalctl for trigger and response timestamps."

if ! command -v journalctl >/dev/null 2>&1; then
  echo -e "${RED}journalctl not available${NC}"; exit 1
fi

start_ts=$(date +%s)
echo "Marking start..."
sudo systemctl restart ${SERVICE} || true
sleep 2

echo -e "${YELLOW}Perform a controlled E-Stop trigger now (within 20s)...${NC}"

deadline=$((start_ts + 20))
trigger_line=""
response_line=""

while [ "$(date +%s)" -le "$deadline" ]; do
  logs=$(sudo journalctl -u ${SERVICE} --since @${start_ts})
  if echo "$logs" | grep -q "E-Stop TRIGGERED!"; then
    trigger_line=$(echo "$logs" | grep "E-Stop TRIGGERED!" | tail -n1)
  fi
  if echo "$logs" | grep -q "Emergency handling activated"; then
    response_line=$(echo "$logs" | grep "Emergency handling activated" | tail -n1)
  fi
  if [ -n "$trigger_line" ] && [ -n "$response_line" ]; then
    break
  fi
  sleep 1
done

if [ -z "$trigger_line" ] || [ -z "$response_line" ]; then
  echo -e "${RED}Failed to capture trigger/response lines. Ensure firmware logs the expected markers.${NC}"
  exit 1
fi

# Extract timestamps from journalctl lines (format: MMM DD HH:MM:SS)
ts_trigger=$(echo "$trigger_line" | awk '{print $1" "$2" "$3}')
ts_response=$(echo "$response_line" | awk '{print $1" "$2" "$3}')

epoch_trigger=$(date -d "$ts_trigger" +%s%3N)
epoch_response=$(date -d "$ts_response" +%s%3N)

latency_ms=$((epoch_response - epoch_trigger))

echo -e "${GREEN}E-Stop latency: ${latency_ms} ms${NC}"

mkdir -p /opt/oht50/logs
echo "$(date -Iseconds), estop_latency_ms: ${latency_ms}" | sudo tee -a /opt/oht50/logs/estop_latency_results.log >/dev/null

exit 0


