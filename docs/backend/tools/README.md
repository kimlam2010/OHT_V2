# Testing Tools - OHT-50 Backend

## 📋 Tổng quan
Thư mục này chứa các công cụ testing và validation cho OHT-50 Backend API.

## 📁 Cấu trúc

| File | Mô tả | Sử dụng cho |
|------|-------|-------------|
| [postman_collection.json](./postman_collection.json) | Postman collection với tất cả endpoints | API testing, Development |
| [INSOMNIA_WORKSPACE.json](./INSOMNIA_WORKSPACE.json) | Insomnia workspace với environments | API testing, Development |
| [curl_suite.sh](./curl_suite.sh) | Script test nhanh với curl | Quick testing, CI/CD |

## 🚀 Quick Start

### 1. Postman Collection
```bash
# Import vào Postman
1. Mở Postman
2. Import → File → Chọn postman_collection.json
3. Set environment variables:
   - base_url: http://localhost:8000
   - token: your-auth-token
```

### 2. Insomnia Workspace
```bash
# Import vào Insomnia
1. Mở Insomnia
2. Import/Export → Import Data → Chọn INSOMNIA_WORKSPACE.json
3. Switch environment: Development/Production
```

### 3. Curl Suite
```bash
# Chạy test nhanh
cd /home/orangepi/Desktop/OHT-50/docs/backend/tools
chmod +x curl_suite.sh
./curl_suite.sh
```

## 🧪 Testing Scenarios

### Basic Health Check
```bash
curl http://localhost:8000/health/
```

### Configuration API
```bash
# Get current config
curl -H "Authorization: Bearer $TOKEN" http://localhost:8000/api/v1/config/

# Update config
curl -X PUT -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d @config.json \
  http://localhost:8000/api/v1/config/
```

### Telemetry API
```bash
# Get current telemetry
curl http://localhost:8000/api/v1/telemetry/current

# Set telemetry rate
curl -X POST http://localhost:8000/api/v1/telemetry/rate?hz=20
```

### Center Integration
```bash
# Submit mission
curl -X POST -H "Content-Type: application/json" \
  -d @mission.json \
  http://localhost:8000/api/v1/center/mission
```

## 📊 Test Coverage

### ✅ Covered Endpoints
- Health endpoints (GET /health/)
- Configuration API (GET/PUT /api/v1/config/)
- Telemetry API (GET /api/v1/telemetry/*)
- Center Integration (POST /api/v1/center/*)

### 🔄 Test Scenarios
- **Happy Path**: Normal operations
- **Error Cases**: Invalid inputs, auth failures
- **Edge Cases**: Boundary values, timeouts
- **Security**: Authentication, authorization

## 🔗 Liên kết quan trọng

- **API Specs**: `../api-specs/` - Endpoint documentation
- **Error Handling**: `../ERRORS_CONVENTION.md` - Error codes
- **Security**: `../operational/SECURITY_GUIDE.md` - Auth testing
- **Deployment**: `../DEPLOYMENT.md` - Environment setup

## 📝 Maintenance

### Regular Updates
- [ ] Update collections khi có API changes
- [ ] Test với actual backend server
- [ ] Verify examples với OpenAPI schema
- [ ] Update environment variables

### Validation
- [ ] All endpoints work với current API
- [ ] Examples match actual responses
- [ ] Error cases properly handled
- [ ] Authentication flows work

## 🚨 Troubleshooting

### Common Issues
1. **Connection refused**: Check if backend server is running
2. **401 Unauthorized**: Verify token in environment
3. **404 Not Found**: Check endpoint URLs
4. **500 Internal Error**: Check backend logs

### Debug Mode
```bash
# Enable verbose output
curl -v http://localhost:8000/health/

# Check response headers
curl -I http://localhost:8000/api/v1/config/
```

## 📈 Performance Testing

### Load Testing
```bash
# Basic load test với curl
for i in {1..100}; do
  curl -s http://localhost:8000/health/ &
done
wait
```

### Stress Testing
```bash
# Stress test với Apache Bench
ab -n 1000 -c 10 http://localhost:8000/health/
```

## 🔒 Security Testing

### Authentication Tests
- [ ] Valid token access
- [ ] Invalid token rejection
- [ ] Missing token handling
- [ ] Token expiration

### Authorization Tests
- [ ] Role-based access
- [ ] Resource permissions
- [ ] API rate limiting
- [ ] Input validation
