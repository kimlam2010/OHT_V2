# 🚨 **FW SAFETY MONITOR IMPLEMENTATION ORDERS**

**Phiên bản:** v1.0  
**Ngày:** 2025-01-28  
**Từ:** PM  
**Đến:** FW Development Team  
**Mức độ:** 🔴 CRITICAL - SAFETY SYSTEM

---

## 🎯 **LỆNH TRIỂN KHAI - IMPLEMENTATION ORDERS**

### **📋 ORDER #1: PHASE 1 FOUNDATION - IMMEDIATE START**

**🚨 CRITICAL SAFETY NOTICE:**
- **KHÔNG được modify existing safety_monitor.c cho đến khi Phase 1 hoàn thành**
- **LUÔN backup code trước khi thay đổi**
- **TEST từng function riêng lẻ trước khi integrate**
- **BÁO CÁO ngay nếu có vấn đề với existing safety logic**

#### **📋 Task 1.1: Environment Setup (Ngày 1)**

**🎯 Lệnh cụ thể:**
```
FW Dev phải thực hiện theo đúng thứ tự:

1. CREATE backup của toàn bộ safety system:
   - cp -r firmware_new/src/app/core firmware_new/src/app/core.backup.$(date +%Y%m%d)
   - git branch safety-monitor-improvement-backup
   
2. CREATE new directories (KHÔNG touch existing files):
   - mkdir -p firmware_new/src/app/core/safety_integration
   - mkdir -p firmware_new/tests/integration/safety_monitor
   - mkdir -p firmware_new/config/safety
   
3. INSTALL dependencies:
   - sudo apt-get install libyaml-dev libcunit1-dev
   - Verify: pkg-config --exists yaml-0.1 && echo "YAML OK" || echo "YAML MISSING"
   
4. CREATE git feature branch:
   - git checkout -b feature/safety-monitor-rs485-integration
   - git push -u origin feature/safety-monitor-rs485-integration
```

**✅ Completion Criteria:**
- [ ] Backup directories created và verified
- [ ] New directories exist
- [ ] Dependencies installed và verified  
- [ ] Feature branch created và pushed
- [ ] NO existing files modified

---

#### **📋 Task 1.2: Data Structure Design (Ngày 2-3)**

**🎯 Lệnh cụ thể:**
```
FW Dev tạo NEW header file (KHÔNG modify existing):

File: firmware_new/src/app/core/safety_integration/safety_rs485_integration.h

YÊU CẦU:
1. Define safety_module_response_t structure
   - connection_online (bool)
   - safety_status (uint8_t: 0=SAFE, 1=WARNING, 2=CRITICAL, 3=ESTOP, 4=FAULT)
   - estop_active (bool)
   - critical_values struct (sensor distances, interlocks)
   - error_codes (uint8_t bitmap)
   - timestamps

2. Define module_safety_config_t structure
   - address, type, is_critical, timeout_ms, action_on_loss

3. Define safety_action_t enum
   - LOG_ONLY, WARNING, DEGRADED, ESTOP_DELAYED, ESTOP_IMMEDIATE

4. INCLUDE existing headers:
   - communication_manager.h
   - safety_module_handler.h  
   - module_manager.h

CHÚ Ý AN TOÀN:
- KHÔNG include vào existing safety_monitor.h
- KHÔNG modify existing enum/struct definitions
- SỬ DỤNG prefix "safety_rs485_" cho all new functions
```

**✅ Completion Criteria:**
- [ ] Header file created với complete data structures
- [ ] All includes working without conflicts
- [ ] No compilation errors
- [ ] No modifications to existing headers
- [ ] Code review passed

---

#### **📋 Task 1.3: Register Reading Function (Ngày 4-5)**

**🎯 Lệnh cụ thể:**
```
FW Dev implement STANDALONE register reading:

File: firmware_new/src/app/core/safety_integration/safety_rs485_integration.c

YÊU CẦU:
1. Implement safety_rs485_read_module_data() function:
   - Use EXISTING comm_manager_modbus_read_holding_registers()
   - Read registers: 0x0000 (status), 0x0001 (estop), 0x0010-0x0011 (sensors), 0x0020 (digital), 0x0005 (errors)
   - Handle communication timeouts gracefully
   - Measure và log response times
   - Return structured data trong safety_module_response_t

2. Implement safety_rs485_validate_response() function:
   - Check data ranges (distances 0-10000mm, status 0-4)
   - Validate timestamps
   - Check error codes

3. ADD comprehensive error handling:
   - Communication failures
   - Invalid data ranges
   - Timeout conditions

CHÚ Ý AN TOÀN:
- KHÔNG call từ existing safety_monitor code
- TEST standalone trước khi integrate
- LOG all register reads cho debugging
- HANDLE all error conditions gracefully
```

**✅ Completion Criteria:**
- [ ] Function compiles without errors
- [ ] Standalone testing successful
- [ ] Communication với Safety Module 0x03 working
- [ ] Response time < 50ms average
- [ ] Error handling comprehensive
- [ ] Logging functional

---

#### **📋 Task 1.4: Hardware Validation (Ngày 6-7)**

**🎯 Lệnh cụ thể:**
```
FW Dev tạo SEPARATE test program:

File: firmware_new/tests/integration/safety_monitor/test_rs485_communication.c

YÊU CẦU:
1. CREATE standalone test executable:
   - Initialize communication manager
   - Test safety_rs485_read_module_data() với real hardware
   - Print all register values
   - Measure response times
   - Test error scenarios (disconnect module)

2. VALIDATE register mapping:
   - Verify 0x0000 returns expected status values
   - Verify 0x0010-0x0011 return reasonable distance values
   - Verify 0x0020 digital inputs work
   - Document actual vs expected values

3. PERFORMANCE testing:
   - 100 consecutive reads
   - Measure min/max/average response times
   - Check for communication errors
   - Verify CRC và timeout handling

CHÚ Ý AN TOÀN:
- RUN test SEPARATE từ main firmware
- KHÔNG affect running safety systems
- BACKUP test results
- DOCUMENT any hardware issues discovered
```

**✅ Completion Criteria:**
- [ ] Test program compiles và runs
- [ ] Communication với Safety Module verified
- [ ] Register values documented
- [ ] Performance benchmarks recorded
- [ ] Error scenarios tested
- [ ] Hardware issues (if any) documented

---

### **📋 ORDER #2: PHASE 2 INTEGRATION - AFTER PHASE 1 COMPLETE**

**🚨 SAFETY INTEGRATION NOTICE:**
- **CHỈ bắt đầu sau khi Phase 1 100% complete**
- **BACKUP existing safety_monitor.c trước khi modify**
- **TEST integration trong isolated environment trước**
- **MAINTAIN existing E-Stop response time < 100ms**

#### **📋 Task 2.1: Critical Module Detection (Tuần 3)**

**🎯 Lệnh cụ thể:**
```
FW Dev implement module monitoring logic:

APPROACH:
1. CREATE new function safety_rs485_check_critical_modules():
   - Query registry_get() cho 5 modules (0x02-0x06)
   - Calculate offline time từ last_seen_timestamp
   - Compare với configured timeouts
   - Determine safety action based on module type

2. DEFINE module priority matrix:
   - Safety Module (0x03): timeout=0ms → immediate E-Stop
   - Power Module (0x02): timeout=5000ms → delayed E-Stop
   - Travel/Lifter (0x04/0x05): timeout=1000ms → warning mode
   - Dock Module (0x06): timeout=10000ms → log only

3. IMPLEMENT safety action handlers:
   - safety_rs485_trigger_immediate_estop()
   - safety_rs485_trigger_delayed_estop()
   - safety_rs485_set_warning_mode()
   - safety_rs485_log_module_loss()

CHÚ Ý AN TOÀN:
- KHÔNG modify existing safety_monitor_trigger_emergency_stop()
- CREATE new wrapper functions
- TEST module detection logic standalone
- VERIFY E-Stop triggers work correctly
```

#### **📋 Task 2.2: Integration với Safety Monitor (Tuần 4)**

**🎯 Lệnh cụ thể:**
```
FW Dev integrate với existing safety_monitor_update():

INTEGRATION APPROACH:
1. MODIFY safety_monitor_update() function:
   - ADD module checking AFTER existing LiDAR checks
   - Use static timestamps để control polling frequency
   - Call safety_rs485_check_critical_modules() every 100ms
   - Call safety_rs485_read_module_data() every 50ms for Safety Module

2. MAINTAIN existing functionality:
   - Keep all existing LiDAR safety logic
   - Keep existing E-Stop handling
   - Keep existing LED patterns
   - Keep existing state machine integration

3. ADD error propagation:
   - Module errors → existing safety event system
   - Communication failures → existing fault handling
   - Recovery events → existing recovery mechanisms

CHÚ Ý AN TOÀN:
- BACKUP safety_monitor.c trước khi modify
- TEST từng modification riêng lẻ
- VERIFY existing functionality unchanged
- MEASURE performance impact (must be < 5ms additional)
```

**✅ Completion Criteria:**
- [ ] Integration complete without breaking existing functionality
- [ ] Module health checking working
- [ ] E-Stop triggers functional
- [ ] Performance requirements met
- [ ] All existing tests still passing

---

### **📋 ORDER #3: PHASE 3 CONFIGURATION - AFTER PHASE 2 COMPLETE**

#### **📋 Task 3.1: Configuration System (Tuần 5-6)**

**🎯 Lệnh cụ thể:**
```
FW Dev implement configuration management:

CONFIGURATION APPROACH:
1. CREATE YAML configuration support:
   - Use libyaml library
   - Define configuration schema
   - Implement parser functions
   - Add validation logic

2. CREATE HTTP API endpoints:
   - GET /api/v1/safety/config
   - PUT /api/v1/safety/config  
   - POST /api/v1/safety/config/reset
   - Include proper error handling

3. IMPLEMENT runtime updates:
   - Hot-reload configuration without restart
   - Validate before applying
   - Backup current config before changes
   - Rollback on validation failure

CHÚ Ý AN TOÀN:
- NEVER apply invalid configurations
- ALWAYS validate before applying
- MAINTAIN factory reset capability
- LOG all configuration changes
```

---

### **📋 ORDER #4: TESTING & VALIDATION - CONTINUOUS**

**🚨 TESTING REQUIREMENTS:**

#### **📋 Continuous Testing Protocol:**
```
FW Dev MUST test after every change:

TESTING SEQUENCE:
1. UNIT TESTS:
   - Test new functions in isolation
   - Mock external dependencies
   - Verify error handling
   - Check boundary conditions

2. INTEGRATION TESTS:
   - Test với real RS485 modules
   - Verify E-Stop response times
   - Check module timeout detection
   - Validate configuration loading

3. REGRESSION TESTS:
   - Ensure existing safety logic unchanged
   - Verify LiDAR safety still working
   - Check E-Stop hardware still functional
   - Validate system state machine integration

4. PERFORMANCE TESTS:
   - Measure safety_monitor_update() cycle time
   - Check memory usage
   - Validate response times
   - Monitor CPU utilization
```

---

## 🚨 **SAFETY COMPLIANCE ORDERS**

### **📋 MANDATORY SAFETY CHECKS:**

**🔴 BEFORE ANY CODE CHANGE:**
- [ ] Read và understand existing safety logic
- [ ] Identify potential impact areas
- [ ] Plan rollback strategy
- [ ] Notify Safety Engineer of planned changes

**🔴 DURING IMPLEMENTATION:**
- [ ] Maintain E-Stop response time < 100ms
- [ ] Never disable existing safety features
- [ ] Test every change thoroughly
- [ ] Document all modifications

**🔴 AFTER IMPLEMENTATION:**
- [ ] Verify all safety tests passing
- [ ] Measure performance impact
- [ ] Update safety documentation
- [ ] Get Safety Engineer approval

---

## 📊 **REPORTING REQUIREMENTS**

### **📋 Daily Reports (to FW Lead):**
```
Subject: Safety Monitor Implementation - Day X Progress

COMPLETED TODAY:
- Task: [Task name]
- Hours spent: [X hours]
- Status: [Completed/In Progress/Blocked]
- Files modified: [List files]

TESTING RESULTS:
- Unit tests: [X/Y passing]
- Integration tests: [X/Y passing]  
- Performance: [measurements]
- Safety validation: [Pass/Fail]

ISSUES/BLOCKERS:
- [List any issues]
- [Impact assessment]
- [Requested support]

NEXT DAY PLAN:
- [Planned tasks]
- [Expected deliverables]
- [Resource needs]
```

### **📋 Weekly Reports (to PM):**
```
Subject: Safety Monitor Implementation - Week X Summary

WEEK SUMMARY:
- Phase: [Current phase]
- Overall progress: [X%]
- Tasks completed: [X/Y]
- Hours spent: [X/Y budgeted]

ACHIEVEMENTS:
- [Major accomplishments]
- [Milestones reached]
- [Technical breakthroughs]

CHALLENGES:
- [Technical challenges]
- [Resource constraints]
- [Integration issues]

NEXT WEEK:
- [Focus areas]
- [Critical tasks]
- [Support needed]

SAFETY STATUS:
- E-Stop functionality: [Status]
- Performance impact: [Measurements]
- Risk assessment: [Low/Medium/High]
```

---

## 🔧 **DEVELOPMENT GUIDELINES**

### **📋 Code Safety Rules:**

**🚨 ABSOLUTELY FORBIDDEN:**
- ❌ Modify existing safety_monitor_trigger_emergency_stop()
- ❌ Change existing E-Stop timing logic
- ❌ Remove existing safety checks
- ❌ Disable existing safety features
- ❌ Commit code that doesn't compile
- ❌ Commit code without tests

**✅ REQUIRED PRACTICES:**
- ✅ Create new functions với "safety_rs485_" prefix
- ✅ Add functionality, don't replace existing
- ✅ Test every function standalone first
- ✅ Measure performance impact of every change
- ✅ Document all assumptions và decisions
- ✅ Get code review before merging

### **📋 Testing Protocol:**

**🔄 For Every Function:**
1. **Write unit test FIRST**
2. **Implement function**
3. **Test standalone**
4. **Measure performance**
5. **Integration test**
6. **Regression test**
7. **Code review**
8. **Merge only after approval**

### **📋 Git Workflow:**

**🔄 Commit Strategy:**
```
- Feature branch: feature/safety-monitor-rs485-integration
- Commit prefix: [SAFETY] 
- Small commits: 1 function per commit
- Clear messages: [SAFETY] Add safety_rs485_read_module_data() function
- Daily pushes: Push progress daily
- PR only when phase complete
```

---

## 🚨 **EMERGENCY PROCEDURES**

### **📋 If Safety System Breaks:**

**🚨 IMMEDIATE ACTIONS:**
1. **STOP all development immediately**
2. **REVERT to last working backup**
3. **NOTIFY Safety Engineer immediately**
4. **DOCUMENT what happened**
5. **WAIT for Safety Engineer approval before continuing**

### **📋 If E-Stop Response Time > 100ms:**

**🚨 IMMEDIATE ACTIONS:**
1. **IDENTIFY root cause**
2. **REVERT changes that caused regression**
3. **NOTIFY FW Lead và Safety Engineer**
4. **RE-MEASURE performance**
5. **GET approval before proceeding**

### **📋 If Communication Issues:**

**🚨 TROUBLESHOOTING SEQUENCE:**
1. **VERIFY hardware connections**
2. **CHECK RS485 bus integrity**
3. **TEST với existing communication_manager functions**
4. **ISOLATE new code from existing code**
5. **ESCALATE to EMBED team if hardware issue**

---

## 📞 **SUPPORT CONTACTS**

### **📋 Immediate Support:**

| **Issue Type** | **Contact** | **Method** | **Response Time** |
|----------------|-------------|------------|-------------------|
| **Safety Logic Issues** | Safety Engineer | Immediate call | < 30 minutes |
| **Build/Compile Errors** | FW Lead | Slack/Email | < 2 hours |
| **Hardware Problems** | EMBED Team | Technical discussion | < 4 hours |
| **Integration Issues** | System Architect | Meeting | < 4 hours |
| **Performance Problems** | FW Lead | Code review | < 2 hours |

### **📋 Escalation Path:**
```
FW Developer → FW Lead → PM → Safety Engineer → CTO
```

---

## 📊 **PROGRESS TRACKING ORDERS**

### **📋 Daily Updates Required:**
- **9:00 AM:** Send daily report to FW Lead
- **5:00 PM:** Update task status trong tracking table
- **End of day:** Commit và push all changes
- **Before leaving:** Update progress percentages

### **📋 Weekly Reviews Required:**
- **Monday 10:00 AM:** Week planning meeting
- **Friday 4:00 PM:** Week summary report
- **Friday 5:00 PM:** Demo current progress
- **Weekend:** NO development on safety-critical code

---

## 🎯 **SUCCESS DEFINITION**

### **📋 Phase 1 Success:**
```
FW Dev MUST achieve ALL criteria:
- ✅ Safety Module 0x03 communication working
- ✅ Register reading < 50ms response time
- ✅ Data structures populated correctly
- ✅ Unit tests passing (100%)
- ✅ No existing functionality broken
- ✅ Performance impact < 1ms additional
- ✅ Code review approved
- ✅ Safety Engineer sign-off
```

### **📋 Overall Success:**
```
Project SUCCESS khi:
- ✅ All 4 requirements implemented
- ✅ All 25 tasks completed
- ✅ All tests passing (>95%)
- ✅ Performance targets met
- ✅ Safety compliance verified
- ✅ Production deployment ready
```

---

## 🚨 **FINAL ORDERS**

**🎯 FW Development Team:**

1. **READ toàn bộ SAFETY_MONITOR_IMPROVEMENT_PLAN.md trước khi bắt đầu**
2. **FOLLOW exact sequence trong implementation orders**
3. **NEVER skip testing steps**
4. **COMMUNICATE issues immediately**
5. **MAINTAIN safety-first mindset**

**📞 Questions/Issues:**
- **Technical:** Contact FW Lead
- **Safety:** Contact Safety Engineer immediately
- **Process:** Contact PM

**🎯 Mục tiêu:** Transform Safety Monitor thành production-ready system trong 8 tuần với ZERO safety compromises!

---

**📝 Acknowledgment Required:**

```
FW Developer Name: ________________
Date: ________________
Signature: ________________

"Tôi đã đọc, hiểu và đồng ý tuân thủ tất cả safety implementation orders trên. 
Tôi cam kết báo cáo ngay lập tức nếu có bất kỳ vấn đề nào ảnh hưởng đến safety system."
```

---

**🚨 This is a SAFETY-CRITICAL project. Follow orders exactly. No exceptions.**
