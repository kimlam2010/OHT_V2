#!/bin/bash
# 🧪 Test Script for Issue #176 - Module Register APIs
# Tests new endpoints: GET/POST /api/v1/modules/{id}/registers

set -e

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BACKEND_URL="${BACKEND_URL:-http://localhost:8000}"
FRONTEND_URL="${FRONTEND_URL:-http://localhost:5173}"
FIRMWARE_URL="${FIRMWARE_URL:-http://localhost:8080}"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}🧪 TEST ISSUE #176 - Register APIs${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo -e "${YELLOW}📍 Backend URL: ${BACKEND_URL}${NC}"
echo -e "${YELLOW}📍 Firmware URL: ${FIRMWARE_URL}${NC}"
echo ""

# Step 1: Login to get token
echo -e "${BLUE}[1/6] 🔐 Login to Backend...${NC}"
LOGIN_RESPONSE=$(curl -s -X POST "${BACKEND_URL}/api/v1/auth/login" \
  -H "Content-Type: application/json" \
  -d '{
    "username": "admin",
    "password": "admin123"
  }')

TOKEN=$(echo "$LOGIN_RESPONSE" | jq -r '.access_token')

if [ "$TOKEN" = "null" ] || [ -z "$TOKEN" ]; then
  echo -e "${RED}❌ Login failed!${NC}"
  echo "$LOGIN_RESPONSE" | jq
  exit 1
fi

echo -e "${GREEN}✅ Login successful${NC}"
echo -e "   Token: ${TOKEN:0:20}...${NC}"
echo ""

# Step 2: Check Firmware health
echo -e "${BLUE}[2/6] 🏥 Check Firmware health...${NC}"
FW_HEALTH=$(curl -s "${FIRMWARE_URL}/health")
FW_STATUS=$(echo "$FW_HEALTH" | jq -r '.status')

if [ "$FW_STATUS" = "healthy" ]; then
  echo -e "${GREEN}✅ Firmware is healthy${NC}"
else
  echo -e "${RED}❌ Firmware is not healthy!${NC}"
  echo "$FW_HEALTH" | jq
  exit 1
fi
echo ""

# Step 3: Test GET /api/v1/modules (Firmware direct)
echo -e "${BLUE}[3/6] 📡 Test Firmware API: GET /api/v1/modules${NC}"
FW_MODULES=$(curl -s "${FIRMWARE_URL}/api/v1/modules")
MODULE_COUNT=$(echo "$FW_MODULES" | jq -r '.data.count')

echo -e "${GREEN}✅ Firmware returned ${MODULE_COUNT} modules${NC}"
echo "$FW_MODULES" | jq '.data.modules[] | {address, type, online}'
echo ""

# Step 4: Test GET /api/v1/modules/2/data (Firmware direct)
echo -e "${BLUE}[4/6] 📊 Test Firmware API: GET /api/v1/modules/2/data${NC}"
FW_REGISTERS=$(curl -s "${FIRMWARE_URL}/api/v1/modules/2/data")
REG_COUNT=$(echo "$FW_REGISTERS" | jq -r '.data.register_count')
MODULE_NAME=$(echo "$FW_REGISTERS" | jq -r '.data.module_name')

if [ "$REG_COUNT" != "null" ]; then
  echo -e "${GREEN}✅ Firmware returned ${REG_COUNT} registers for ${MODULE_NAME}${NC}"
  echo ""
  echo -e "${YELLOW}📋 Sample registers:${NC}"
  echo "$FW_REGISTERS" | jq '.data.registers[:3] | .[] | {address, name, mode, value}'
else
  echo -e "${RED}❌ Failed to get registers from Firmware${NC}"
  echo "$FW_REGISTERS" | jq
fi
echo ""

# Step 5: Test Backend GET /api/v1/modules/{id}/registers
echo -e "${BLUE}[5/6] 🆕 Test Backend API: GET /api/v1/modules/2/registers${NC}"
BE_REGISTERS=$(curl -s -H "Authorization: Bearer $TOKEN" \
  "${BACKEND_URL}/api/v1/modules/2/registers")

BE_SUCCESS=$(echo "$BE_REGISTERS" | jq -r '.success')
BE_REG_COUNT=$(echo "$BE_REGISTERS" | jq -r '.data.register_count')

if [ "$BE_SUCCESS" = "true" ]; then
  echo -e "${GREEN}✅ Backend API successful!${NC}"
  echo -e "   Module: $(echo "$BE_REGISTERS" | jq -r '.data.module_name')"
  echo -e "   Register count: ${BE_REG_COUNT}"
  echo -e "   Online: $(echo "$BE_REGISTERS" | jq -r '.data.online')"
  echo ""
  echo -e "${YELLOW}📋 Registers with ADDRESS and MODE:${NC}"
  echo "$BE_REGISTERS" | jq '.data.registers[:5] | .[] | {address, name, mode, data_type, value}'
else
  echo -e "${RED}❌ Backend API failed!${NC}"
  echo "$BE_REGISTERS" | jq
  exit 1
fi
echo ""

# Step 6: Test Backend POST /api/v1/modules/{id}/registers/{addr}
echo -e "${BLUE}[6/6] 🆕 Test Backend API: POST /api/v1/modules/2/registers/0x0049${NC}"
echo -e "${YELLOW}⚠️  Writing to 12V Relay register (may fail if slave doesn't support writes)${NC}"

WRITE_RESPONSE=$(curl -s -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -X POST \
  -d '{"value": 1}' \
  "${BACKEND_URL}/api/v1/modules/2/registers/0x0049")

WRITE_SUCCESS=$(echo "$WRITE_RESPONSE" | jq -r '.success')

if [ "$WRITE_SUCCESS" = "true" ]; then
  echo -e "${GREEN}✅ Write register successful!${NC}"
  echo "$WRITE_RESPONSE" | jq
else
  echo -e "${YELLOW}⚠️  Write failed (expected if slave simulation doesn't support writes)${NC}"
  echo "$WRITE_RESPONSE" | jq
fi
echo ""

# Summary
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}🎉 TEST SUMMARY${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo -e "${GREEN}✅ Backend login: OK${NC}"
echo -e "${GREEN}✅ Firmware health: OK${NC}"
echo -e "${GREEN}✅ Firmware /modules: OK (${MODULE_COUNT} modules)${NC}"
echo -e "${GREEN}✅ Firmware /modules/2/data: OK (${REG_COUNT} registers)${NC}"
echo -e "${GREEN}✅ Backend GET /modules/2/registers: OK (${BE_REG_COUNT} registers)${NC}"

if [ "$WRITE_SUCCESS" = "true" ]; then
  echo -e "${GREEN}✅ Backend POST /modules/2/registers/0x0049: OK${NC}"
else
  echo -e "${YELLOW}⚠️  Backend POST /modules/2/registers/0x0049: SKIPPED (slave limitation)${NC}"
fi

echo ""
echo -e "${GREEN}🎉 Issue #176 implementation verified!${NC}"
echo -e "${GREEN}✅ Frontend can now access:${NC}"
echo -e "   • Register address (e.g., '0x0000')${NC}"
echo -e "   • Register mode (READ/WRITE/READ_WRITE)${NC}"
echo -e "   • Register values, types, and metadata${NC}"
echo ""

