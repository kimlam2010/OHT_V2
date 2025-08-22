#!/bin/bash
# OHT-50 Startup Verification Script
# Verifies 30/60/90/120s milestones and patterns in journalctl

set -e

BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

print_ok() { echo -e "${GREEN}[OK]${NC} $1"; }
print_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
print_err() { echo -e "${RED}[ERR]${NC} $1"; }

SERVICE="oht50.service"
BACKEND_URL="http://localhost:8000/health"

echo -e "${BLUE}=== OHT-50 Startup Test (30/60/90/120s) ===${NC}"

# 0) Prechecks
if ! command -v journalctl >/dev/null 2>&1; then
  print_err "journalctl not found"
  exit 1
fi

start_ts=$(date +%s)

echo "Restarting ${SERVICE}..."
sudo systemctl restart ${SERVICE} || true
sleep 2

# Helper to check a log pattern within a timeout window
check_log_within() {
  local pattern="$1"
  local timeout_s="$2"
  local label="$3"
  local now
  local since

  while true; do
    now=$(date +%s)
    elapsed=$((now - start_ts))
    if [ "$elapsed" -gt "$timeout_s" ]; then
      print_err "Timeout ${timeout_s}s: missing '${label}'"
      return 1
    fi
    if sudo journalctl -u ${SERVICE} --since @${start_ts} | grep -q "$pattern"; then
      print_ok "${label} (${elapsed}s)"
      return 0
    fi
    sleep 2
  done
}

# 30s: LED & E-Stop init
check_log_within "LED system initialized" 30 "LED system initialized"
check_log_within "E-Stop safety system initialized" 30 "E-Stop safety system initialized"

# 60s: RS485 device + scan
check_log_within "RS485 device opened successfully" 60 "RS485 device opened successfully"
check_log_within "Starting scan range" 60 "RS485 scan started"

# 90s: Backend reachable
deadline_90=$((start_ts + 90))
while true; do
  if curl -s "$BACKEND_URL" | grep -q '"status"'; then
    print_ok "Backend API reachable"
    break
  fi
  if [ "$(date +%s)" -gt "$deadline_90" ]; then
    print_err "Timeout 90s: Backend API not reachable"
    exit 1
  fi
  sleep 2
done

# 120s: System IDLE
check_log_within "System state = IDLE" 120 "System state = IDLE"

echo -e "${GREEN}All milestones passed within 120s.${NC}"
exit 0

#!/bin/bash
# OHT-50 Startup Sequence Test Script
# Version: 1.0.0
# Date: 2025-01-28

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
OHT_HOME="/opt/oht50"
TIMEOUT=120  # seconds
CHECK_INTERVAL=5  # seconds

echo -e "${BLUE}=== OHT-50 Startup Sequence Test ===${NC}"

# Function to print status
print_status() {
    echo -e "${GREEN}[OK]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check service status
check_service_status() {
    if systemctl is-active --quiet oht50.service; then
        return 0
    else
        return 1
    fi
}

# Function to check if system reached IDLE state
check_idle_state() {
    # Check if service is running and has been running for a while
    if ! check_service_status; then
        return 1
    fi
    
    # Check logs for IDLE state (simplified check)
    if journalctl -u oht50.service --since "1 minute ago" | grep -q "IDLE\|idle"; then
        return 0
    fi
    
    return 1
}

# Function to check LED patterns
check_led_patterns() {
    # Check if LED system is initialized
    if journalctl -u oht50.service --since "30 seconds ago" | grep -q "LED system initialized"; then
        print_status "LED system initialized"
        return 0
    else
        print_warning "LED system initialization not found in logs"
        return 1
    fi
}

# Function to check E-Stop system
check_estop_system() {
    # Check if E-Stop system is initialized
    if journalctl -u oht50.service --since "30 seconds ago" | grep -q "E-Stop safety system initialized"; then
        print_status "E-Stop system initialized"
        return 0
    else
        print_warning "E-Stop system initialization not found in logs"
        return 1
    fi
}

# Function to check RS485 communication
check_rs485_communication() {
    # Check if RS485 is initialized
    if journalctl -u oht50.service --since "30 seconds ago" | grep -q "RS485 device opened successfully"; then
        print_status "RS485 communication initialized"
        return 0
    else
        print_warning "RS485 initialization not found in logs"
        return 1
    fi
}

# Function to check module scanning
check_module_scanning() {
    # Check if module scanning started
    if journalctl -u oht50.service --since "1 minute ago" | grep -q "Starting scan range"; then
        print_status "Module scanning started"
        return 0
    else
        print_warning "Module scanning not found in logs"
        return 1
    fi
}

# Function to check backend API
check_backend_api() {
    # Check if backend is accessible
    if curl -s http://localhost:8000/health >/dev/null 2>&1; then
        print_status "Backend API accessible"
        return 0
    else
        print_warning "Backend API not accessible"
        return 1
    fi
}

# Function to check CLI tools
check_cli_tools() {
    if command -v oht50-cli >/dev/null 2>&1; then
        print_status "CLI tools available"
        return 0
    else
        print_warning "CLI tools not available"
        return 1
    fi
}

# Main test function
run_startup_test() {
    echo "Starting OHT-50 startup sequence test..."
    echo "Timeout: ${TIMEOUT} seconds"
    echo "Check interval: ${CHECK_INTERVAL} seconds"
    echo ""
    
    # Start timing
    start_time=$(date +%s)
    elapsed=0
    
    # Test timeline markers
    timeline_30s=false
    timeline_60s=false
    timeline_90s=false
    timeline_120s=false
    
    while [ $elapsed -lt $TIMEOUT ]; do
        current_time=$(date +%s)
        elapsed=$((current_time - start_time))
        
        echo -n "Time: ${elapsed}s - "
        
        # Check timeline markers
        if [ $elapsed -ge 30 ] && [ "$timeline_30s" = false ]; then
            echo -e "${BLUE}30s milestone${NC}"
            timeline_30s=true
            check_led_patterns
            check_estop_system
        elif [ $elapsed -ge 60 ] && [ "$timeline_60s" = false ]; then
            echo -e "${BLUE}60s milestone${NC}"
            timeline_60s=true
            check_rs485_communication
            check_module_scanning
        elif [ $elapsed -ge 90 ] && [ "$timeline_90s" = false ]; then
            echo -e "${BLUE}90s milestone${NC}"
            timeline_90s=true
            check_backend_api
            check_cli_tools
        elif [ $elapsed -ge 120 ] && [ "$timeline_120s" = false ]; then
            echo -e "${BLUE}120s milestone${NC}"
            timeline_120s=true
            if check_idle_state; then
                print_status "System reached IDLE state within 120s"
                break
            else
                print_error "System did not reach IDLE state within 120s"
                return 1
            fi
        else
            # Check if system reached IDLE state
            if check_idle_state; then
                print_status "System reached IDLE state at ${elapsed}s"
                break
            else
                echo "Waiting for IDLE state..."
            fi
        fi
        
        sleep $CHECK_INTERVAL
    done
    
    # Final checks
    echo ""
    echo -e "${BLUE}=== Final System Check ===${NC}"
    
    # Check service status
    if check_service_status; then
        print_status "OHT-50 service is running"
    else
        print_error "OHT-50 service is not running"
        return 1
    fi
    
    # Check all components
    led_ok=$(check_led_patterns && echo "true" || echo "false")
    estop_ok=$(check_estop_system && echo "true" || echo "false")
    rs485_ok=$(check_rs485_communication && echo "true" || echo "false")
    scan_ok=$(check_module_scanning && echo "true" || echo "false")
    api_ok=$(check_backend_api && echo "true" || echo "false")
    cli_ok=$(check_cli_tools && echo "true" || echo "false")
    
    # Summary
    echo ""
    echo -e "${BLUE}=== Startup Test Summary ===${NC}"
    echo "Service Status: $([ "$(check_service_status && echo "true" || echo "false")" = "true" ] && echo "✅" || echo "❌")"
    echo "LED System: $([ "$led_ok" = "true" ] && echo "✅" || echo "❌")"
    echo "E-Stop System: $([ "$estop_ok" = "true" ] && echo "✅" || echo "❌")"
    echo "RS485 Communication: $([ "$rs485_ok" = "true" ] && echo "✅" || echo "❌")"
    echo "Module Scanning: $([ "$scan_ok" = "true" ] && echo "✅" || echo "❌")"
    echo "Backend API: $([ "$api_ok" = "true" ] && echo "✅" || echo "❌")"
    echo "CLI Tools: $([ "$cli_ok" = "true" ] && echo "✅" || echo "❌")"
    echo "IDLE State: $([ "$(check_idle_state && echo "true" || echo "false")" = "true" ] && echo "✅" || echo "❌")"
    
    # Check if all critical components passed
    if [ "$led_ok" = "true" ] && [ "$estop_ok" = "true" ] && [ "$rs485_ok" = "true" ] && check_idle_state; then
        echo ""
        print_status "Startup sequence test PASSED"
        return 0
    else
        echo ""
        print_error "Startup sequence test FAILED"
        return 1
    fi
}

# Show recent logs
show_recent_logs() {
    echo ""
    echo -e "${BLUE}=== Recent Service Logs ===${NC}"
    journalctl -u oht50.service --since "5 minutes ago" --no-pager -l | tail -20
}

# Main execution
main() {
    # Check if service exists
    if ! systemctl list-unit-files | grep -q "oht50.service"; then
        print_error "oht50.service not found. Please install first."
        exit 1
    fi
    
    # Check if service is enabled
    if ! systemctl is-enabled --quiet oht50.service; then
        print_warning "oht50.service is not enabled. Enabling now..."
        systemctl enable oht50.service
    fi
    
    # Start service if not running
    if ! check_service_status; then
        print_warning "oht50.service is not running. Starting now..."
        systemctl start oht50.service
        sleep 2
    fi
    
    # Run startup test
    if run_startup_test; then
        echo ""
        print_status "OHT-50 startup sequence test completed successfully!"
        show_recent_logs
        exit 0
    else
        echo ""
        print_error "OHT-50 startup sequence test failed!"
        show_recent_logs
        exit 1
    fi
}

# Run main function
main
