#!/bin/bash
# WebSocket Load Testing Script (Issue #153)
# Test WebSocket server with 10+ concurrent clients

echo "🔧 WebSocket Load Testing Script - Issue #153"
echo "=============================================="

# Configuration
WS_SERVER_PORT=8081
MAX_CLIENTS=15
TEST_DURATION=60  # seconds
MESSAGE_INTERVAL=1  # seconds

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test results
TOTAL_CONNECTIONS=0
SUCCESSFUL_CONNECTIONS=0
FAILED_CONNECTIONS=0
TOTAL_MESSAGES=0
AVERAGE_LATENCY=0

echo -e "${BLUE}📊 Test Configuration:${NC}"
echo "  - Server Port: $WS_SERVER_PORT"
echo "  - Max Clients: $MAX_CLIENTS"
echo "  - Test Duration: $TEST_DURATION seconds"
echo "  - Message Interval: $MESSAGE_INTERVAL seconds"
echo ""

# Check if WebSocket server is running
echo -e "${YELLOW}🔍 Checking WebSocket server status...${NC}"
if ! netstat -tuln | grep -q ":$WS_SERVER_PORT "; then
    echo -e "${RED}❌ WebSocket server not running on port $WS_SERVER_PORT${NC}"
    echo "Please start the firmware first: ./oht50_main"
    exit 1
fi
echo -e "${GREEN}✅ WebSocket server is running${NC}"
echo ""

# Create test client function
create_test_client() {
    local client_id=$1
    local duration=$2
    
    echo "Starting client $client_id..."
    
    # Use wscat or websocat if available, otherwise use netcat
    if command -v wscat &> /dev/null; then
        timeout $duration wscat -c ws://localhost:$WS_SERVER_PORT \
            --ping-interval 5 \
            --ping-timeout 3 \
            --max-messages 100 \
            --message-interval $MESSAGE_INTERVAL \
            --client-id $client_id
    elif command -v websocat &> /dev/null; then
        timeout $duration websocat ws://localhost:$WS_SERVER_PORT \
            --ping-interval 5s \
            --ping-timeout 3s \
            --text \
            --oneshot
    else
        # Fallback to netcat (basic test)
        echo "Client $client_id connected" | nc localhost $WS_SERVER_PORT &
        sleep $duration
        kill %1 2>/dev/null
    fi
    
    local exit_code=$?
    if [ $exit_code -eq 0 ]; then
        echo "Client $client_id completed successfully"
        return 0
    else
        echo "Client $client_id failed (exit code: $exit_code)"
        return 1
    fi
}

# Start load test
echo -e "${BLUE}🚀 Starting load test with $MAX_CLIENTS concurrent clients...${NC}"
echo ""

start_time=$(date +%s)
pids=()

# Start all clients concurrently
for ((i=1; i<=MAX_CLIENTS; i++)); do
    create_test_client $i $TEST_DURATION &
    pids+=($!)
    TOTAL_CONNECTIONS=$((TOTAL_CONNECTIONS + 1))
    
    # Small delay to stagger connections
    sleep 0.1
done

echo -e "${YELLOW}⏱️  Running test for $TEST_DURATION seconds...${NC}"

# Wait for all clients to complete
for pid in "${pids[@]}"; do
    if wait $pid; then
        SUCCESSFUL_CONNECTIONS=$((SUCCESSFUL_CONNECTIONS + 1))
    else
        FAILED_CONNECTIONS=$((FAILED_CONNECTIONS + 1))
    fi
done

end_time=$(date +%s)
test_duration=$((end_time - start_time))

echo ""
echo -e "${BLUE}📊 Load Test Results:${NC}"
echo "=============================================="
echo "Test Duration: $test_duration seconds"
echo "Total Connections: $TOTAL_CONNECTIONS"
echo "Successful Connections: $SUCCESSFUL_CONNECTIONS"
echo "Failed Connections: $FAILED_CONNECTIONS"
echo "Success Rate: $(( (SUCCESSFUL_CONNECTIONS * 100) / TOTAL_CONNECTIONS ))%"
echo ""

# Check server performance metrics (if available)
echo -e "${BLUE}🔍 Server Performance Check:${NC}"
if netstat -tuln | grep -q ":$WS_SERVER_PORT "; then
    echo -e "${GREEN}✅ Server still running after load test${NC}"
    
    # Count active connections
    active_connections=$(netstat -an | grep ":$WS_SERVER_PORT " | grep ESTABLISHED | wc -l)
    echo "Active Connections: $active_connections"
    
    # Check memory usage
    if [ -f /proc/meminfo ]; then
        memory_usage=$(grep MemAvailable /proc/meminfo | awk '{print $2}')
        echo "Available Memory: $memory_usage KB"
    fi
else
    echo -e "${RED}❌ Server crashed during load test${NC}"
fi

echo ""

# Performance evaluation
echo -e "${BLUE}📈 Performance Evaluation:${NC}"
echo "=============================================="

if [ $SUCCESSFUL_CONNECTIONS -eq $TOTAL_CONNECTIONS ]; then
    echo -e "${GREEN}✅ EXCELLENT: All connections successful${NC}"
elif [ $SUCCESSFUL_CONNECTIONS -ge $((TOTAL_CONNECTIONS * 90 / 100)) ]; then
    echo -e "${YELLOW}⚠️  GOOD: 90%+ connections successful${NC}"
elif [ $SUCCESSFUL_CONNECTIONS -ge $((TOTAL_CONNECTIONS * 70 / 100)) ]; then
    echo -e "${YELLOW}⚠️  FAIR: 70%+ connections successful${NC}"
else
    echo -e "${RED}❌ POOR: Less than 70% connections successful${NC}"
fi

if [ $FAILED_CONNECTIONS -eq 0 ]; then
    echo -e "${GREEN}✅ No connection failures${NC}"
else
    echo -e "${YELLOW}⚠️  $FAILED_CONNECTIONS connection failures${NC}"
fi

echo ""
echo -e "${BLUE}🎯 Issue #153 Compliance Check:${NC}"
echo "=============================================="

# Check against Issue #153 requirements
if [ $SUCCESSFUL_CONNECTIONS -ge 10 ]; then
    echo -e "${GREEN}✅ Requirement: 10+ concurrent clients - PASSED${NC}"
else
    echo -e "${RED}❌ Requirement: 10+ concurrent clients - FAILED${NC}"
fi

if [ $FAILED_CONNECTIONS -eq 0 ]; then
    echo -e "${GREEN}✅ Requirement: < 1% connection failure rate - PASSED${NC}"
else
    failure_rate=$(( (FAILED_CONNECTIONS * 100) / TOTAL_CONNECTIONS ))
    if [ $failure_rate -lt 1 ]; then
        echo -e "${GREEN}✅ Requirement: < 1% connection failure rate - PASSED ($failure_rate%)${NC}"
    else
        echo -e "${RED}❌ Requirement: < 1% connection failure rate - FAILED ($failure_rate%)${NC}"
    fi
fi

echo ""
echo -e "${BLUE}🏁 Load Test Complete!${NC}"
echo "=============================================="
