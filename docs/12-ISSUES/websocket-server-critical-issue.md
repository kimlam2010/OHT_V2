# 🚨 **WEBSOCKET SERVER CRITICAL ISSUE - OHT-50**

**Issue ID:** WS-001  
**Severity:** 🔴 **CRITICAL**  
**Priority:** **P0 - BLOCKING**  
**Date:** 28/01/2025  
**Reporter:** PM Team  
**Assignee:** FW Team  

---

## 📋 **ISSUE SUMMARY**

### **Problem:**
WebSocket server trong firmware OHT-50 bị **DISABLED hoàn toàn** do hang issue, gây ảnh hưởng nghiêm trọng đến real-time communication của hệ thống.

### **Current Status:**
```c
// main.c line 572-573
// DISABLED: WebSocket Server (causing hang issue)
printf("[OHT-50] ⚠️  WebSocket Server DISABLED (hang issue - use port 8080 for all APIs)\n");
```

### **Impact:**
- ❌ **Frontend không thể nhận real-time updates**
- ❌ **Backend không thể stream telemetry data**
- ❌ **Safety alerts không real-time**
- ❌ **LiDAR data không streaming**
- ❌ **Module monitoring không live**
- ❌ **System không đạt production readiness**

---

## 🔍 **TECHNICAL ANALYSIS**

### **Root Cause Analysis:**
1. **Socket blocking issues** trong websocket_server.c
2. **Timeout handling problems** - accept timeout quá ngắn (1 second)
3. **Non-blocking I/O workarounds** không hiệu quả
4. **Debug output spam** gây performance issues
5. **Connection management** không robust

### **Code Issues Found:**
```c
// websocket_server.c - Line 174-176
// Set accept timeout to prevent blocking (Issue #113 Fix)
struct timeval accept_timeout;
accept_timeout.tv_sec = 1;   // ❌ TOO SHORT

// Line 1728-1730
// Set socket to non-blocking mode to prevent hang
int flags = fcntl(socket_fd, F_GETFL, 0);
fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);  // ❌ WORKAROUND
```

### **Architecture Impact:**
```yaml
Current Architecture (BROKEN):
Backend → HTTP (Port 8080) → Firmware
Frontend → HTTP (Port 8080) → Firmware
❌ No real-time communication

Required Architecture:
Backend → HTTP (Port 8080) → Firmware
Backend → WebSocket (Port 8081) → Firmware ← Real-time
Frontend → WebSocket (Port 8081) → Firmware ← Real-time
```

---

## 🎯 **REQUIREMENTS**

### **Functional Requirements:**
- ✅ **WebSocket server hoạt động ổn định** trên port 8081
- ✅ **Real-time telemetry streaming** cho Frontend
- ✅ **Bidirectional communication** Backend ↔ Firmware
- ✅ **Connection management** robust và reliable
- ✅ **Error handling** comprehensive
- ✅ **Performance** latency < 50ms

### **Technical Requirements:**
- ✅ **Non-blocking I/O** implementation
- ✅ **Proper timeout handling** (5-10 seconds)
- ✅ **Connection pooling** và management
- ✅ **Authentication support** (JWT tokens)
- ✅ **Message queuing** cho reliability
- ✅ **Health monitoring** và diagnostics

### **Message Types Required:**
```yaml
telemetry:    # Real-time telemetry data
status:       # Status updates  
alerts:       # Alert notifications
commands:     # Command responses
lidar:        # LiDAR scan data
modules:      # Module status updates
```

---

## 🛠️ **SOLUTION APPROACH**

### **Phase 1: Debug & Fix (2 days)**
1. **Debug hang issue** trong websocket_server.c
2. **Fix socket blocking** problems
3. **Implement proper timeout** handling
4. **Remove debug spam** từ production code
5. **Test basic WebSocket** functionality

### **Phase 2: Implementation (3 days)**
1. **Implement real-time telemetry** streaming
2. **Add connection management** robust
3. **Implement authentication** support
4. **Add error handling** comprehensive
5. **Performance optimization**

### **Phase 3: Integration (2 days)**
1. **Backend integration** testing
2. **Frontend integration** testing
3. **End-to-end validation**
4. **Performance benchmarking**
5. **Production readiness** validation

---

## 📊 **ACCEPTANCE CRITERIA**

### **✅ Must Have:**
- [ ] WebSocket server starts without hanging
- [ ] Real-time telemetry streaming works
- [ ] Frontend can connect và receive updates
- [ ] Backend can connect và send commands
- [ ] Connection management robust
- [ ] Error handling comprehensive
- [ ] Performance latency < 50ms
- [ ] No debug spam trong production

### **✅ Should Have:**
- [ ] Authentication support
- [ ] Message queuing
- [ ] Health monitoring
- [ ] Connection pooling
- [ ] Retry mechanisms
- [ ] Graceful degradation

### **✅ Could Have:**
- [ ] WebSocket compression
- [ ] Message encryption
- [ ] Advanced monitoring
- [ ] Load balancing
- [ ] Clustering support

---

## 🧪 **TESTING STRATEGY**

### **Unit Testing:**
```bash
# Test WebSocket server startup
./test_websocket_startup

# Test connection handling
./test_websocket_connections

# Test message handling
./test_websocket_messages

# Test error scenarios
./test_websocket_errors
```

### **Integration Testing:**
```bash
# Test với Backend
./test_backend_websocket_integration

# Test với Frontend
./test_frontend_websocket_integration

# Test end-to-end
./test_websocket_e2e
```

### **Performance Testing:**
```bash
# Test latency
./test_websocket_latency

# Test throughput
./test_websocket_throughput

# Test concurrent connections
./test_websocket_concurrent
```

---

## 📅 **TIMELINE**

### **Day 1-2: Debug & Fix**
- Debug hang issue
- Fix socket blocking
- Implement proper timeouts
- Remove debug spam
- Basic functionality test

### **Day 3-5: Implementation**
- Real-time telemetry streaming
- Connection management
- Authentication support
- Error handling
- Performance optimization

### **Day 6-7: Integration**
- Backend integration testing
- Frontend integration testing
- End-to-end validation
- Performance benchmarking
- Production readiness

---

## 🚨 **ESCALATION MATRIX**

### **Level 1: FW Team Lead**
- **Responsibility:** Debug và fix hang issue
- **Timeline:** 2 ngày
- **Escalation:** Nếu không fix được trong 2 ngày

### **Level 2: CTO**
- **Responsibility:** Architecture review và technical guidance
- **Timeline:** 1 ngày
- **Escalation:** Nếu cần thay đổi architecture

### **Level 3: Executive**
- **Responsibility:** Resource allocation và priority
- **Timeline:** Immediate
- **Escalation:** Nếu blocking production release

---

## 📞 **CONTACT INFORMATION**

### **FW Team:**
- **Lead:** FW Team Lead
- **Email:** fw-team@oht50.local
- **Slack:** #fw-team
- **Status:** 🔴 **ACTIVE** - Working on fix

### **Backend Team:**
- **Lead:** Backend Team Lead
- **Email:** backend-team@oht50.local
- **Slack:** #backend-team
- **Status:** 🟡 **WAITING** - Ready for integration

### **Frontend Team:**
- **Lead:** Frontend Team Lead
- **Email:** frontend-team@oht50.local
- **Slack:** #frontend-team
- **Status:** 🟡 **WAITING** - Ready for integration

---

## 📚 **REFERENCES**

### **Related Documents:**
- [Backend-Firmware Integration Architecture](../03-ARCHITECTURE/BACKEND_FIRMWARE_INTEGRATION_ARCHITECTURE.md)
- [API Documentation](../05-IMPLEMENTATION/FIRMWARE/API_DOCUMENTATION.md)
- [WebSocket Server Implementation](../05-IMPLEMENTATION/FIRMWARE/websocket_server.c)

### **Related Issues:**
- Issue #113: WebSocket accept timeout fix
- Issue #90: RS485 Module Telemetry Broadcasting
- Issue #FW-01: WebSocket & Integration Implementation

### **External References:**
- [WebSocket RFC 6455](https://tools.ietf.org/html/rfc6455)
- [libwebsockets Documentation](https://libwebsockets.org/)
- [WebSocket Best Practices](https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API/Writing_WebSocket_client_applications)

---

## 🎯 **SUCCESS METRICS**

### **Technical Metrics:**
- **WebSocket server startup time:** < 2 seconds
- **Connection establishment:** < 1 second
- **Message latency:** < 50ms
- **Throughput:** > 100 messages/second
- **Concurrent connections:** > 10 clients
- **Uptime:** > 99.9%

### **Business Metrics:**
- **Frontend real-time updates:** Working
- **Backend telemetry streaming:** Working
- **Safety alerts real-time:** Working
- **System production ready:** Yes
- **User satisfaction:** High

---

## 🔄 **CHANGELOG**

### **v1.0 (2025-01-28):**
- ✅ Created critical issue document
- ✅ Identified root causes
- ✅ Defined solution approach
- ✅ Set acceptance criteria
- ✅ Created testing strategy
- ✅ Established timeline
- ✅ Defined escalation matrix

---

**🚨 CRITICAL:** WebSocket server fix là **BLOCKING ISSUE** cho production release. FW team cần ưu tiên cao nhất và fix trong 7 ngày tới.

**📞 Contact:** PM Team - pm-team@oht50.local - #pm-team
