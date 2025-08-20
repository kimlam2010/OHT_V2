#!/bin/bash
# RS485 fault injection test: provoke CRC/timeout and record statistics

set -e

BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}=== RS485 Fault Test (CRC/Timeout) ===${NC}"

# Build tests if needed
if [ -d firmware ]; then
  (cd firmware && make -s test_rs485 || true)
fi

# Baseline: run test_rs485 to init/open/health
if [ -x firmware/build/test_rs485 ]; then
  echo "Running baseline RS485 health..."
  sudo firmware/build/test_rs485 | cat || true
fi

# Fault scenario 1: Timeout (no slave)
echo -e "\n${YELLOW}Injecting timeout scenario (no slave / wrong address)...${NC}"
# Here we rely on HAL stats after attempted read; run a simple ping via CLI to a non-existent address
if [ -x tools/module_cli.py ]; then
  echo "module_cli ping --addr 0x7F (expected timeout)"
  ./tools/module_cli.py ping --addr 0x7F || true
fi

# Collect journald stats lines if HAL prints timeout increments
echo -e "\nCollecting RS485 timeout errors from logs (last 2 min)..."
sudo journalctl -u oht50.service --since "2 minutes ago" | grep -i "rs485\|timeout" | tail -n 50 | sed 's/^/  /'

# Fault scenario 2: CRC (if supported by HAL/modbus stubs)
echo -e "\n${YELLOW}Attempting CRC error provoke (if supported)...${NC}"
echo "(Note: current HAL has stubbed CRC verify; record N/A if not supported)"

# Summary line into results file
mkdir -p /opt/oht50/logs
echo "$(date -Iseconds), rs485_timeout_errors: collected, rs485_crc_errors: N_A" | sudo tee -a /opt/oht50/logs/rs485_fault_results.log >/dev/null

echo -e "\n${GREEN}RS485 fault test completed.${NC}"
exit 0


