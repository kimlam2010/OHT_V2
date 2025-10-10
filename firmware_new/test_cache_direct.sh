#!/bin/bash
# Test cache API trực tiếp

echo "🧪 Testing Register Cache API"
echo "================================"

# Test 1: Manual cache store
echo ""
echo "📝 Test 1: Manual cache store và retrieve"

curl -s -X POST \
  -H "Authorization: Bearer oht50_admin_token_2025" \
  -H "Content-Type: application/json" \
  -d '{"value": 24400}' \
  http://localhost:8080/api/v1/modules/2/registers/0x0000 | jq

echo ""
echo "🔍 Test 2: Check if value was cached"
curl -s http://localhost:8080/api/v1/modules/2/data | jq '.data.registers[] | select(.address == "0x0000")'

echo ""
echo "✅ Test complete"

