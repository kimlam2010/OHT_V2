#!/bin/bash

# OHT-50 FW Clean Restart Script
# Fixes port conflicts and restarts FW cleanly

echo "🔧 OHT-50 FW Clean Restart Script"
echo "=================================="

# Step 1: Kill all FW processes
echo "Step 1: Killing existing FW processes..."
sudo pkill -f oht50_main
sudo pkill -f "8080\|8081"

# Wait for processes to terminate
sleep 2

# Step 2: Force kill if still running
echo "Step 2: Force killing any remaining processes..."
sudo pkill -9 -f oht50_main

# Step 3: Check ports are free
echo "Step 3: Checking ports 8080 and 8081..."
if netstat -tlnp | grep -E "(8080|8081)" > /dev/null; then
    echo "❌ Ports still in use:"
    netstat -tlnp | grep -E "(8080|8081)"
    echo "Trying to find and kill processes using these ports..."
    
    # Find PIDs using ports 8080 and 8081
    PIDS_8080=$(lsof -ti:8080 2>/dev/null)
    PIDS_8081=$(lsof -ti:8081 2>/dev/null)
    
    if [ ! -z "$PIDS_8080" ]; then
        echo "Killing processes using port 8080: $PIDS_8080"
        sudo kill -9 $PIDS_8080
    fi
    
    if [ ! -z "$PIDS_8081" ]; then
        echo "Killing processes using port 8081: $PIDS_8081"
        sudo kill -9 $PIDS_8081
    fi
    
    sleep 1
else
    echo "✅ Ports 8080 and 8081 are free"
fi

# Step 4: Final check
echo "Step 4: Final port check..."
if netstat -tlnp | grep -E "(8080|8081)" > /dev/null; then
    echo "❌ Ports still in use after cleanup:"
    netstat -tlnp | grep -E "(8080|8081)"
    echo "Manual intervention required"
    exit 1
else
    echo "✅ All ports are free - ready to start FW"
fi

# Step 5: Start FW
echo "Step 5: Starting FW..."
cd "$(dirname "$0")/../build"
if [ -f "./oht50_main" ]; then
    echo "Starting OHT-50 FW..."
    ./oht50_main --verbose
else
    echo "❌ oht50_main not found in build directory"
    echo "Please build FW first: cd firmware_new/build && make"
    exit 1
fi
