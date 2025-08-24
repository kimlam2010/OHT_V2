#!/bin/bash

echo "🧪 Testing OHT-50 API Endpoints"
echo "================================="

# Test 1: System Health
echo "1. Testing System Health..."
timeout 5 curl -s http://localhost:8080/api/v1/system/health
if [ $? -eq 0 ]; then
    echo "✅ System Health: OK"
else
    echo "❌ System Health: FAILED"
fi

echo ""

# Test 2: System Status
echo "2. Testing System Status..."
timeout 5 curl -s http://localhost:8080/api/v1/system/status
if [ $? -eq 0 ]; then
    echo "✅ System Status: OK"
else
    echo "❌ System Status: FAILED"
fi

echo ""

# Test 3: Modules List
echo "3. Testing Modules List..."
timeout 5 curl -s http://localhost:8080/api/v1/modules
if [ $? -eq 0 ]; then
    echo "✅ Modules List: OK"
else
    echo "❌ Modules List: FAILED"
fi

echo ""

# Test 4: Safety Status
echo "4. Testing Safety Status..."
timeout 5 curl -s http://localhost:8080/api/v1/safety/status
if [ $? -eq 0 ]; then
    echo "✅ Safety Status: OK"
else
    echo "❌ Safety Status: FAILED"
fi

echo ""

# Test 5: Module Command (POST)
echo "5. Testing Module Command..."
timeout 10 curl -s -X POST http://localhost:8080/api/v1/modules/1/command \
    -H "Content-Type: application/json" \
    -d '{"command": "get_status", "parameters": {}}'
if [ $? -eq 0 ]; then
    echo "✅ Module Command: OK"
else
    echo "❌ Module Command: FAILED"
fi

echo ""
echo "🎯 API Testing Complete!"
