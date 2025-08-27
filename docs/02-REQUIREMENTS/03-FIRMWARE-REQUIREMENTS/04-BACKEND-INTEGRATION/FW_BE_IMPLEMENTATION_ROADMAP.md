# 🛣️ FW_BE_IMPLEMENTATION_ROADMAP - Lộ trình thực hiện FW-BE Integration

**Mục đích:** Lộ trình chi tiết để khắc phục khoảng cách giữa BE hiện tại và FW_BE_INTEGRATION_GUIDE

---

## 📋 **Thông tin tài liệu:**
- **Phiên bản:** v1.0
- **Ngày tạo:** 2025-01-27
- **Người tạo:** FW Team
- **Trạng thái:** ✅ Hoàn thành
- **Đối tượng đọc:** BE Team, FW Team, System Integration Team

---

## 🎯 **Tổng quan lộ trình:**

### **Mục tiêu:**
- Đồng bộ hóa BE với FW_BE_INTEGRATION_GUIDE
- Đảm bảo API compatibility 100%
- Tối ưu hóa performance và reliability
- Hoàn thành trong 6 tuần

### **Phạm vi:**
- **API Endpoints:** Standardize URL structure
- **Data Models:** Update enums và structures
- **WebSocket:** Implement message types
- **Error Handling:** Standardize error codes
- **Configuration:** Add missing constants

---

## 📅 **Timeline chi tiết:**

### **Week 1: Foundation & URL Standardization**

#### **Day 1-2: URL Structure Changes**
```python
# Task 1.1: Create new API structure
- Create /api/v1/ prefix
- Implement backward compatibility
- Update routing configuration

# Task 1.2: Migrate existing endpoints
- /fw_integration/status → /api/v1/system/status
- /fw_integration/modules → /api/v1/modules
- /fw_integration/command → /api/v1/modules/{id}/command
```

**Deliverables:**
- [ ] New API structure implemented
- [ ] Backward compatibility maintained
- [ ] Routing tests passed

#### **Day 3-4: Core API Implementation**
```python
# Task 1.3: Implement missing core APIs
- GET /api/v1/system/status
- GET /api/v1/system/health
- GET /api/v1/modules
- GET /api/v1/safety/status
- GET /api/v1/config
- GET /api/v1/diagnostics
```

**Deliverables:**
- [ ] All core APIs implemented
- [ ] API documentation updated
- [ ] Unit tests written

#### **Day 5: Data Model Updates**
```python
# Task 1.4: Update data models
- Add SystemState enum
- Add ModuleType enum
- Add ModuleStatus enum
- Add SafetyLevel enum
```

**Deliverables:**
- [ ] All enums implemented
- [ ] Model validation working
- [ ] Type hints updated

---

### **Week 2: Core Features & Error Handling**

#### **Day 1-2: Missing Endpoints Implementation**
```python
# Task 2.1: Implement module management
- POST /api/v1/modules/{id}/command
- GET /api/v1/modules/{id}/status
- PUT /api/v1/modules/{id}/config

# Task 2.2: Implement configuration management
- GET /api/v1/config
- PUT /api/v1/config
- POST /api/v1/config/validate
```

**Deliverables:**
- [ ] Module management APIs working
- [ ] Configuration APIs working
- [ ] Integration tests passed

#### **Day 3-4: Error Handling Standardization**
```python
# Task 2.3: Standardize error codes
- MODULE_NOT_FOUND
- MODULE_OFFLINE
- INVALID_COMMAND
- TIMEOUT
- SAFETY_VIOLATION
- SYSTEM_FAULT

# Task 2.4: Update error response format
- Standardize error response structure
- Add error code mapping
- Implement error logging
```

**Deliverables:**
- [ ] Error codes standardized
- [ ] Error response format consistent
- [ ] Error logging implemented

#### **Day 5: Basic Integration Testing**
```python
# Task 2.5: Integration testing
- Test API endpoints with mock FW
- Validate data formats
- Test error scenarios
```

**Deliverables:**
- [ ] Integration tests written
- [ ] Mock FW client working
- [ ] Test coverage > 80%

---

### **Week 3: Advanced Features & WebSocket**

#### **Day 1-2: WebSocket Message Standardization**
```python
# Task 3.1: Implement WebSocket message types
- system_status updates
- module_status updates
- safety_events
- module_data updates

# Task 3.2: Standardize message format
- Define message structure
- Implement message validation
- Add message routing
```

**Deliverables:**
- [ ] WebSocket message types implemented
- [ ] Message format standardized
- [ ] WebSocket tests working

#### **Day 3-4: Configuration Constants**
```python
# Task 3.3: Add FW constants
- API_HTTP_PORT = 8080
- API_WEBSOCKET_PORT = 8081
- RS485_DEVICE_PATH = "/dev/ttyOHT485"
- Module addresses (0x02-0x06)
- Timing constants

# Task 3.4: Configuration management
- Environment-based configuration
- Configuration validation
- Configuration hot-reload
```

**Deliverables:**
- [ ] All constants defined
- [ ] Configuration management working
- [ ] Environment setup documented

#### **Day 5: Enhanced Error Codes**
```python
# Task 3.5: Advanced error handling
- Error code categories
- Error severity levels
- Error recovery strategies
- Error reporting system
```

**Deliverables:**
- [ ] Advanced error handling implemented
- [ ] Error recovery working
- [ ] Error reporting system ready

---

### **Week 4: Integration & Performance**

#### **Day 1-2: End-to-End Testing**
```python
# Task 4.1: Comprehensive testing
- System status flow
- Module communication flow
- Safety system integration
- Real-time data flow
- Error handling scenarios

# Task 4.2: Performance testing
- Load testing
- Stress testing
- Memory leak testing
- Response time validation
```

**Deliverables:**
- [ ] End-to-end tests passing
- [ ] Performance benchmarks met
- [ ] No memory leaks detected

#### **Day 3-4: Performance Optimization**
```python
# Task 4.3: Performance optimization
- Response time optimization
- Memory usage optimization
- Database query optimization
- Caching implementation

# Task 4.4: Monitoring setup
- Performance metrics
- Health checks
- Alerting system
- Logging optimization
```

**Deliverables:**
- [ ] Performance targets met
- [ ] Monitoring system working
- [ ] Alerting configured

#### **Day 5: Documentation Updates**
```python
# Task 4.5: Documentation
- API documentation update
- Integration guide update
- Deployment guide update
- Troubleshooting guide
```

**Deliverables:**
- [ ] All documentation updated
- [ ] Integration guide complete
- [ ] Deployment guide ready

---

### **Week 5: Validation & Bug Fixes**

#### **Day 1-2: Integration Testing with FW**
```python
# Task 5.1: Real FW integration
- Connect to actual FW
- Test all API endpoints
- Validate data exchange
- Test error scenarios

# Task 5.2: Real-time communication
- WebSocket connection test
- Message flow validation
- Latency measurement
- Reliability testing
```

**Deliverables:**
- [ ] Real FW integration working
- [ ] All APIs validated
- [ ] Real-time communication stable

#### **Day 3-4: Bug Fixes & Optimization**
```python
# Task 5.3: Bug fixes
- Fix identified issues
- Optimize performance
- Improve error handling
- Enhance reliability

# Task 5.4: Final optimization
- Code review
- Performance tuning
- Security review
- Documentation review
```

**Deliverables:**
- [ ] All bugs fixed
- [ ] Performance optimized
- [ ] Security validated

#### **Day 5: Final Validation**
```python
# Task 5.5: Final validation
- Complete system test
- Performance validation
- Security validation
- Documentation validation
```

**Deliverables:**
- [ ] System validation passed
- [ ] Performance targets met
- [ ] Security requirements met

---

### **Week 6: Deployment & Monitoring**

#### **Day 1-2: Production Deployment**
```python
# Task 6.1: Production deployment
- Deploy to staging
- Run integration tests
- Deploy to production
- Monitor deployment

# Task 6.2: Rollback preparation
- Rollback procedures
- Backup strategies
- Emergency procedures
- Communication plan
```

**Deliverables:**
- [ ] Production deployment successful
- [ ] Rollback procedures ready
- [ ] Emergency procedures documented

#### **Day 3-4: Monitoring Setup**
```python
# Task 6.3: Monitoring implementation
- Real-time monitoring
- Performance tracking
- Error tracking
- Alert configuration

# Task 6.4: Logging setup
- Centralized logging
- Log analysis
- Log retention
- Log security
```

**Deliverables:**
- [ ] Monitoring system active
- [ ] Alerts configured
- [ ] Logging system working

#### **Day 5: Documentation Finalization**
```python
# Task 6.5: Final documentation
- Update all documentation
- Create user guides
- Create troubleshooting guides
- Create maintenance guides
```

**Deliverables:**
- [ ] All documentation complete
- [ ] User guides ready
- [ ] Maintenance procedures documented

---

## 🔧 **Technical Implementation Details:**

### **1. File Structure Changes:**

#### **New Files:**
```
backend/app/api/v1/
├── system.py          # System status APIs
├── modules.py         # Module management APIs
├── diagnostics.py     # Diagnostics APIs
└── __init__.py        # Updated router registration

backend/app/models/
├── system.py          # System state models
├── modules.py         # Module models
└── fw.py              # FW-specific models

backend/app/services/
├── system_service.py  # System management service
├── module_service.py  # Module management service
└── fw_service.py      # Updated FW integration service
```

#### **Updated Files:**
```
backend/app/api/v1/
├── fw_integration.py  # Standardized URLs
├── safety.py          # Standardized safety APIs
└── config.py          # Updated configuration APIs

backend/app/models/
├── safety.py          # Updated safety models
└── fw_integration.py  # Updated FW models

backend/lib/
├── fw_client.py       # Updated client library
└── fw_protocol.py     # Updated protocol definitions
```

### **2. API Endpoint Mapping:**

#### **Old → New Mapping:**
```python
# Old endpoints (to be deprecated)
/fw_integration/status     → /api/v1/system/status
/fw_integration/modules    → /api/v1/modules
/fw_integration/command    → /api/v1/modules/{id}/command
/fw_integration/connect    → /api/v1/system/connect
/fw_integration/disconnect → /api/v1/system/disconnect

# New endpoints
GET  /api/v1/system/status
GET  /api/v1/system/health
GET  /api/v1/modules
GET  /api/v1/modules/{id}
POST /api/v1/modules/{id}/command
GET  /api/v1/safety/status
POST /api/v1/safety/estop
GET  /api/v1/config
PUT  /api/v1/config
GET  /api/v1/diagnostics
```

### **3. Data Model Updates:**

#### **New Enums:**
```python
from enum import Enum

class SystemState(Enum):
    INIT = "INIT"
    IDLE = "IDLE"
    MOVE = "MOVE"
    DOCK = "DOCK"
    FAULT = "FAULT"
    ESTOP = "ESTOP"
    SHUTDOWN = "SHUTDOWN"

class ModuleType(Enum):
    POWER = "POWER"
    MOTOR = "MOTOR"
    IO = "IO"
    DOCK = "DOCK"
    SENSOR = "SENSOR"

class ModuleStatus(Enum):
    ONLINE = "ONLINE"
    OFFLINE = "OFFLINE"
    ERROR = "ERROR"

class SafetyLevel(Enum):
    NORMAL = "NORMAL"
    WARNING = "WARNING"
    CRITICAL = "CRITICAL"
    EMERGENCY = "EMERGENCY"
```

#### **Updated Models:**
```python
from pydantic import BaseModel
from typing import Optional, List, Dict, Any
from datetime import datetime

class SystemStatus(BaseModel):
    system_name: str
    version: str
    status: SystemState
    uptime_ms: int
    active_modules: int
    estop_active: bool
    safety_ok: bool
    timestamp: datetime

class ModuleInfo(BaseModel):
    module_id: int
    module_type: ModuleType
    status: ModuleStatus
    version: str
    last_seen: datetime
    capabilities: List[str]
    data: Optional[Dict[str, Any]] = None

class SafetyStatus(BaseModel):
    estop_active: bool
    safety_ok: bool
    safety_level: SafetyLevel
    safety_message: str
    last_safety_check: datetime
    fault_code: Optional[int] = None
    fault_message: Optional[str] = None
```

### **4. WebSocket Message Types:**

#### **Message Structure:**
```python
class WebSocketMessage(BaseModel):
    type: str
    data: Dict[str, Any]
    timestamp: datetime
    correlation_id: Optional[str] = None

# Message types
SYSTEM_STATUS_UPDATE = "system_status"
MODULE_STATUS_UPDATE = "module_status"
SAFETY_EVENT = "safety_event"
MODULE_DATA_UPDATE = "module_data"
ERROR_EVENT = "error_event"
```

#### **Message Examples:**
```python
# System status update
{
    "type": "system_status",
    "data": {
        "state": "IDLE",
        "safety_ok": true,
        "estop_active": false,
        "timestamp": 1640995200000
    },
    "timestamp": "2025-01-27T10:00:00Z"
}

# Module status update
{
    "type": "module_status",
    "data": {
        "module_id": 2,
        "status": "ONLINE",
        "last_seen": 1640995200000
    },
    "timestamp": "2025-01-27T10:00:00Z"
}
```

### **5. Error Handling:**

#### **Error Codes:**
```python
class FWErrorCode(Enum):
    MODULE_NOT_FOUND = "MODULE_NOT_FOUND"
    MODULE_OFFLINE = "MODULE_OFFLINE"
    INVALID_COMMAND = "INVALID_COMMAND"
    TIMEOUT = "TIMEOUT"
    SAFETY_VIOLATION = "SAFETY_VIOLATION"
    SYSTEM_FAULT = "SYSTEM_FAULT"
    CONNECTION_ERROR = "CONNECTION_ERROR"
    AUTHENTICATION_ERROR = "AUTHENTICATION_ERROR"
    VALIDATION_ERROR = "VALIDATION_ERROR"
```

#### **Error Response Format:**
```python
class ErrorResponse(BaseModel):
    success: bool = False
    error_code: FWErrorCode
    message: str
    details: Optional[str] = None
    timestamp: datetime
    correlation_id: Optional[str] = None
```

---

## 📊 **Success Metrics & KPIs:**

### **1. API Compatibility:**
- [ ] **100% endpoints match FW Guide**
- [ ] **100% data structures compatible**
- [ ] **100% error codes standardized**
- [ ] **Backward compatibility maintained**

### **2. Performance Metrics:**
- [ ] **Response time < 100ms** (95th percentile)
- [ ] **Throughput > 1000 requests/second**
- [ ] **WebSocket latency < 50ms**
- [ ] **Memory usage < 256MB**
- [ ] **CPU usage < 30%**

### **3. Reliability Metrics:**
- [ ] **99.9% uptime**
- [ ] **< 0.1% error rate**
- [ ] **Zero data loss**
- [ ] **< 1 second recovery time**

### **4. Integration Metrics:**
- [ ] **End-to-end testing pass rate > 95%**
- [ ] **FW communication success rate > 99%**
- [ ] **Real-time data flow latency < 100ms**
- [ ] **WebSocket connection stability > 99%**

### **5. Quality Metrics:**
- [ ] **Code coverage > 90%**
- [ ] **Documentation coverage 100%**
- [ ] **Security scan pass**
- [ ] **Performance benchmarks met**

---

## 🚨 **Risk Management:**

### **High Risk Items:**
1. **URL structure changes** - Có thể break existing clients
2. **Data model changes** - Cần migration strategy
3. **FW integration** - Phụ thuộc vào FW stability

### **Mitigation Strategies:**
1. **Backward compatibility** - Maintain old endpoints temporarily
2. **Gradual rollout** - Deploy features incrementally
3. **Comprehensive testing** - Extensive testing before production
4. **Monitoring** - Real-time monitoring during deployment
5. **Rollback plan** - Quick rollback procedures ready

### **Contingency Plans:**
1. **If FW integration fails** - Use mock FW for testing
2. **If performance degrades** - Optimize critical paths first
3. **If backward compatibility breaks** - Quick hotfix deployment
4. **If security issues found** - Immediate security patch

---

## 📚 **References:**

### **Related Documents:**
- [FW_BE_INTEGRATION_GUIDE](FW_BE_INTEGRATION_GUIDE.md)
- [FW_BE_GAP_ANALYSIS](FW_BE_GAP_ANALYSIS.md)
- [BE Architecture](../04-BACKEND-REQUIREMENTS/REQ_BE_Architecture.md)

### **Source Code:**
- [BE API](backend/app/api/v1/)
- [BE Models](backend/app/models/)
- [FW Client](backend/lib/fw_client.py)

---

**Changelog:**
- ✅ v1.0 (2025-01-27): Initial implementation roadmap
- ✅ Detailed timeline với deliverables
- ✅ Technical implementation details
- ✅ Success metrics và risk management

**🚨 Lưu ý:** Lộ trình này có thể điều chỉnh dựa trên tiến độ thực tế và feedback từ team.
