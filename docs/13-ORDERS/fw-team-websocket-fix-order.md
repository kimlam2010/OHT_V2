# 🚨 **LỆNH KHẨN CẤP CHO FW TEAM - FIX WEBSOCKET SERVER**

**Ngày:** 28/01/2025  
**Từ:** PM Team  
**Đến:** FW Team  
**Priority:** 🔴 **P0 - CRITICAL**  
**Deadline:** 7 ngày (04/02/2025)  

---

## 📋 **LỆNH CHÍNH THỨC**

### **🎯 MISSION:**
**FIX WEBSOCKET SERVER HANG ISSUE - BLOCKING PRODUCTION RELEASE**

### **📊 STATUS:**
- **GitHub Issue:** [#130](https://github.com/kimlam2010/OHT_V2/issues/130) - 🚨 CRITICAL
- **Technical Document:** `docs/issues/WEBSOCKET_SERVER_CRITICAL_ISSUE.md`
- **Current Status:** ❌ **WebSocket Server DISABLED**
- **Impact:** 🔴 **BLOCKING** - System không thể production ready

---

## 🚨 **CRITICAL ISSUE SUMMARY**

### **Problem:**
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

## 🎯 **IMMEDIATE ACTIONS REQUIRED**

### **🔴 DAY 1-2: DEBUG & FIX (CRITICAL)**

#### **1. Debug Hang Issue:**
```bash
cd /home/orangepi/Desktop/OHT_V2/firmware_new/src/app
# Debug websocket_server.c hang issue
# Identify root cause của blocking
# Fix socket blocking problems
```

#### **2. Fix Socket Issues:**
```c
// Fix trong websocket_server.c:
// - Increase accept timeout từ 1s lên 5-10s
// - Implement proper non-blocking I/O
// - Fix connection management
// - Remove debug spam
```

#### **3. Test Basic Functionality:**
```bash
cd /home/orangepi/Desktop/OHT_V2/firmware_new/build
./oht50_main --debug  # Test WebSocket startup
```

### **🟡 DAY 3-5: IMPLEMENTATION**

#### **1. Real-time Features:**
- Implement telemetry streaming
- Add connection management
- Implement authentication support
- Add error handling
- Performance optimization

#### **2. Integration:**
- Backend WebSocket client testing
- Frontend WebSocket client testing
- End-to-end validation

### **🟢 DAY 6-7: VALIDATION**

#### **1. Testing:**
- Unit tests
- Integration tests
- Performance tests
- Production readiness validation

---

## 📁 **FILES TO FIX**

### **Primary Files:**
1. **`firmware_new/src/main.c`** (line 572-573)
   - Remove DISABLED status
   - Enable WebSocket server

2. **`firmware_new/src/app/websocket_server.c`**
   - Fix hang issue
   - Fix socket blocking
   - Fix timeout handling
   - Remove debug spam

3. **`firmware_new/src/app/managers/communication_manager.c`**
   - Fix simulated server
   - Implement real WebSocket server
   - Fix telemetry broadcasting

### **Secondary Files:**
- `firmware_new/src/app/api/api_endpoints.c`
- `firmware_new/include/websocket_server.h`
- `firmware_new/tests/unit/test_websocket_server.c`

---

## ✅ **ACCEPTANCE CRITERIA**

### **Must Have:**
- [ ] WebSocket server starts without hanging
- [ ] Real-time telemetry streaming works
- [ ] Frontend can connect và receive updates
- [ ] Backend can connect và send commands
- [ ] Connection management robust
- [ ] Error handling comprehensive
- [ ] Performance latency < 50ms
- [ ] No debug spam trong production

### **Should Have:**
- [ ] Authentication support
- [ ] Message queuing
- [ ] Health monitoring
- [ ] Connection pooling
- [ ] Retry mechanisms

---

## 🧪 **TESTING REQUIREMENTS**

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

## 📊 **SUCCESS METRICS**

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

---

## 📅 **TIMELINE & MILESTONES**

### **Day 1 (29/01/2025):**
- [ ] Debug hang issue
- [ ] Identify root cause
- [ ] Fix socket blocking
- [ ] Test basic startup

### **Day 2 (30/01/2025):**
- [ ] Fix timeout handling
- [ ] Remove debug spam
- [ ] Test WebSocket functionality
- [ ] Basic integration test

### **Day 3-4 (31/01-01/02/2025):**
- [ ] Implement real-time telemetry
- [ ] Add connection management
- [ ] Implement authentication
- [ ] Add error handling

### **Day 5 (02/02/2025):**
- [ ] Performance optimization
- [ ] Backend integration testing
- [ ] Frontend integration testing

### **Day 6-7 (03-04/02/2025):**
- [ ] End-to-end validation
- [ ] Performance benchmarking
- [ ] Production readiness validation
- [ ] Documentation update

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

## 📞 **COMMUNICATION**

### **Daily Updates:**
- **Time:** 9:00 AM daily
- **Format:** Progress report trong GitHub issue #130
- **Content:** Status, blockers, next steps

### **Escalation:**
- **Immediate:** Nếu có blocking issues
- **Daily:** Progress updates
- **Weekly:** Milestone reviews

### **Contact:**
- **PM Team:** pm-team@oht50.local
- **Slack:** #pm-team
- **GitHub:** Issue #130

---

## 🎯 **DELIVERABLES**

### **Code Deliverables:**
- [ ] Fixed WebSocket server (no hang)
- [ ] Real-time telemetry streaming
- [ ] Robust connection management
- [ ] Comprehensive error handling
- [ ] Performance optimized

### **Documentation Deliverables:**
- [ ] Updated API documentation
- [ ] WebSocket implementation guide
- [ ] Testing procedures
- [ ] Performance benchmarks

### **Testing Deliverables:**
- [ ] Unit test suite
- [ ] Integration test suite
- [ ] Performance test results
- [ ] Production readiness report

---

## 🏆 **SUCCESS CRITERIA**

### **Technical Success:**
- ✅ WebSocket server hoạt động ổn định
- ✅ Real-time communication working
- ✅ Performance đạt targets
- ✅ No critical bugs

### **Business Success:**
- ✅ Frontend real-time updates working
- ✅ Backend telemetry streaming working
- ✅ System production ready
- ✅ User satisfaction high

---

## 🚨 **CRITICAL REMINDER**

**WebSocket server fix là BLOCKING ISSUE cho production release!**

**FW Team cần:**
1. **STOP** tất cả tasks khác
2. **FOCUS** 100% vào WebSocket fix
3. **REPORT** daily progress
4. **ESCALATE** nếu có blockers
5. **DELIVER** trong 7 ngày

**🎯 MISSION CRITICAL - NO EXCUSES!**

---

**Changelog v1.0:**
- ✅ Created official order for FW team
- ✅ Defined critical timeline và milestones
- ✅ Set clear acceptance criteria
- ✅ Established escalation matrix
- ✅ Created testing requirements
- ✅ Defined success metrics

**📞 Contact:** PM Team - pm-team@oht50.local - #pm-team
