# 🚨 **FW TEAM CRITICAL IMPLEMENTATION PROMPT - OHT-50**

**Ngày:** 2025-09-19  
**Priority:** 🔴 **CRITICAL - SYSTEM BLOCKING**  
**Assigned:** Firmware Team  
**PM Status:** Integration failure detected - immediate action required  

---

## 🚨 **CRITICAL SITUATION**

**PM Integration Testing đã phát hiện COMPLETE INTEGRATION FAILURE:**
- ❌ Backend không thể kết nối với Firmware
- ❌ Không có real hardware communication
- ❌ 100% mock data usage
- ❌ System không thể operate với hardware

## 📋 **GITHUB ISSUES CẦN FIX NGAY**

### **🔴 Issue #107: CRITICAL - Firmware HTTP API Service Not Running**
- **Link:** https://github.com/kimlam2010/OHT_V2/issues/107
- **Problem:** Firmware service không chạy trên port 8081
- **Impact:** Backend không thể connect, complete integration failure
- **Action:** START FIRMWARE SERVICE IMMEDIATELY

### **🔴 Issue #109: CRITICAL - Missing Backend-Firmware Integration Architecture**
- **Link:** https://github.com/kimlam2010/OHT_V2/issues/109
- **Problem:** HTTP API ↔ RS485 bridge không được implement
- **Impact:** Architecture violation, 0% hardware communication
- **Action:** IMPLEMENT HTTP API SERVER

---

## 🎯 **IMMEDIATE TASKS (MUST DO TODAY)**

### **TASK 1: 🚨 START FIRMWARE HTTP API SERVICE**

#### **Current Status:**
```bash
# Test Results:
curl http://localhost:8081/health
# Result: Connection refused - NO SERVICE RUNNING

ps aux | grep oht50_main
# Result: No process found

netstat -tulpn | grep 8081
# Result: Port not in use
```

#### **Required Action:**
```bash
cd /home/orangepi/Desktop/OHT_V2/firmware_new

# Option 1: Start with HTTP API (if implemented)
./build/oht50_main --api-port 8081 --enable-http-api

# Option 2: Start basic service first
./build/oht50_main &

# Verify it's running
ps aux | grep oht50_main
netstat -tulpn | grep 8081
```

#### **Expected Result:**
```bash
# Should see process running
orangepi   XXXX  X.X  X.X  XXXXX  XXXX ?  Sl   XX:XX   X:XX ./build/oht50_main

# Should see port listening
tcp LISTEN 0.0.0.0:8081 0.0.0.0:* users:(("oht50_main",pid=XXXX,fd=X))
```

### **TASK 2: 🔧 IMPLEMENT HTTP API ENDPOINTS**

#### **Required Endpoints (MUST IMPLEMENT):**
```c
// HTTP API Server Implementation Needed
GET  /health                    // Service health check
GET  /api/v1/status            // Firmware status
GET  /api/v1/robot/status      // Robot status from RS485
POST /api/v1/robot/command     // Robot commands to RS485
GET  /api/v1/rs485/modules     // RS485 module list
GET  /api/v1/telemetry/current // Real-time telemetry data
```

#### **Implementation Framework:**
```c
// firmware_new/src/app/http_api_server.c (CREATE THIS FILE)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
#include "rs485_communication.h"
#include "telemetry_manager.h"

// HTTP API Server Structure
typedef struct {
    struct MHD_Daemon *daemon;
    int port;
    int running;
} http_api_server_t;

// Health Check Endpoint
static int handle_health(struct MHD_Connection *connection) {
    const char *response = "{\\"success\\":true,\\"status\\":\\"healthy\\",\\"timestamp\\":\\"2025-09-19T17:30:00Z\\"}";
    
    struct MHD_Response *mhd_response = MHD_create_response_from_buffer(
        strlen(response), (void*)response, MHD_RESPMEM_PERSISTENT
    );
    
    MHD_add_response_header(mhd_response, "Content-Type", "application/json");
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, mhd_response);
    MHD_destroy_response(mhd_response);
    
    return ret;
}

// RS485 Modules Endpoint  
static int handle_rs485_modules(struct MHD_Connection *connection) {
    // GET REAL DATA FROM RS485 - NOT MOCK!
    rs485_module_list_t *modules = rs485_get_module_list();
    
    char response[4096];
    snprintf(response, sizeof(response), 
        "{\\"success\\":true,\\"data\\":[");
        
    for (int i = 0; i < modules->count; i++) {
        rs485_module_t *module = &modules->modules[i];
        // Get REAL status from RS485 hardware
        char *status = rs485_get_module_status(module->address);
        
        snprintf(response + strlen(response), sizeof(response) - strlen(response),
            "{\\"address\\":%d,\\"name\\":\\"%s\\",\\"status\\":\\"%s\\",\\"last_seen\\":\\"%s\\"}",
            module->address, module->name, status, get_current_timestamp()
        );
        
        if (i < modules->count - 1) strcat(response, ",");
    }
    
    strcat(response, "],\\"message\\":\\"Retrieved RS485 modules\\"}");
    
    struct MHD_Response *mhd_response = MHD_create_response_from_buffer(
        strlen(response), (void*)response, MHD_RESPMEM_PERSISTENT
    );
    
    MHD_add_response_header(mhd_response, "Content-Type", "application/json");
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, mhd_response);
    MHD_destroy_response(mhd_response);
    
    return ret;
}

// Main HTTP Request Handler
static int handle_request(void *cls, struct MHD_Connection *connection,
                         const char *url, const char *method,
                         const char *version, const char *upload_data,
                         size_t *upload_data_size, void **con_cls) {
    
    printf("HTTP Request: %s %s\\n", method, url);
    
    if (strcmp(method, "GET") == 0) {
        if (strcmp(url, "/health") == 0) {
            return handle_health(connection);
        }
        else if (strcmp(url, "/api/v1/rs485/modules") == 0) {
            return handle_rs485_modules(connection);
        }
        else if (strcmp(url, "/api/v1/status") == 0) {
            return handle_firmware_status(connection);
        }
        else if (strcmp(url, "/api/v1/robot/status") == 0) {
            return handle_robot_status(connection);
        }
        else if (strcmp(url, "/api/v1/telemetry/current") == 0) {
            return handle_telemetry_current(connection);
        }
    }
    else if (strcmp(method, "POST") == 0) {
        if (strcmp(url, "/api/v1/robot/command") == 0) {
            return handle_robot_command(connection, upload_data, upload_data_size);
        }
    }
    
    // 404 Not Found
    const char *not_found = "{\\"error\\":\\"Not Found\\"}";
    struct MHD_Response *response = MHD_create_response_from_buffer(
        strlen(not_found), (void*)not_found, MHD_RESPMEM_PERSISTENT
    );
    
    int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_destroy_response(response);
    return ret;
}

// Start HTTP API Server
int start_http_api_server(int port) {
    struct MHD_Daemon *daemon = MHD_start_daemon(
        MHD_USE_THREAD_PER_CONNECTION,
        port,
        NULL, NULL,
        &handle_request, NULL,
        MHD_OPTION_END
    );
    
    if (daemon == NULL) {
        printf("❌ Failed to start HTTP API server on port %d\\n", port);
        return -1;
    }
    
    printf("✅ HTTP API server started on port %d\\n", port);
    return 0;
}
```

### **TASK 3: 🔗 INTEGRATE WITH MAIN FIRMWARE**

#### **Update main.c:**
```c
// firmware_new/src/main.c (UPDATE THIS)
#include "http_api_server.h"

int main(int argc, char *argv[]) {
    printf("🚀 Starting OHT-50 Firmware with HTTP API\\n");
    
    // Initialize RS485 communication
    if (rs485_init("/dev/ttyOHT485", 115200) != 0) {
        printf("❌ Failed to initialize RS485\\n");
        return -1;
    }
    
    // Initialize telemetry system
    telemetry_manager_init();
    
    // Start HTTP API server on port 8081
    if (start_http_api_server(8081) != 0) {
        printf("❌ Failed to start HTTP API server\\n");
        return -1;
    }
    
    printf("✅ Firmware HTTP API ready - Backend can now connect!\\n");
    
    // Main firmware loop
    while (1) {
        // Update RS485 modules
        rs485_update_modules();
        
        // Update telemetry
        telemetry_manager_update();
        
        // Process safety systems
        safety_monitor_update();
        
        // Sleep for control loop
        usleep(10000); // 10ms loop
    }
    
    return 0;
}
```

### **TASK 4: 🔌 BUILD AND TEST**

#### **Build Commands:**
```bash
cd /home/orangepi/Desktop/OHT_V2/firmware_new

# Clean build
make clean

# Build with HTTP API support
make ENABLE_HTTP_API=1

# Check build result
ls -la build/oht50_main
```

#### **Test Commands:**
```bash
# Start firmware
./build/oht50_main &

# Test health endpoint
curl http://localhost:8081/health

# Expected response:
{"success":true,"status":"healthy","timestamp":"2025-09-19T17:30:00Z"}

# Test RS485 modules endpoint
curl http://localhost:8081/api/v1/rs485/modules

# Expected response: REAL MODULE DATA, NOT MOCK!
```

---

## 🎯 **SUCCESS CRITERIA**

### **✅ MUST ACHIEVE TODAY:**
- [ ] Firmware process running on port 8081
- [ ] HTTP API endpoints responding
- [ ] Backend can connect: `curl http://localhost:8081/health` = success
- [ ] RS485 modules return REAL data (not "lost" status)
- [ ] Backend integration test passes

### **🔍 VERIFICATION COMMANDS:**
```bash
# 1. Check firmware process
ps aux | grep oht50_main
# Should show: ./build/oht50_main running

# 2. Check port listening
netstat -tulpn | grep 8081
# Should show: tcp LISTEN 0.0.0.0:8081

# 3. Test health endpoint
curl http://localhost:8081/health
# Should return: {"success":true,"status":"healthy",...}

# 4. Test backend integration
curl http://localhost:8000/api/v1/rs485/modules
# Should return: REAL module data with "healthy" status
```

---

## 🚨 **CRITICAL NOTES**

### **⚠️ IMPORTANT:**
1. **NO MOCK DATA:** All endpoints must return REAL hardware data
2. **RS485 INTEGRATION:** Must communicate with actual RS485 modules
3. **PORT 8081:** Backend expects HTTP API on this exact port
4. **REAL-TIME DATA:** Timestamps must be current, not static

### **❌ COMMON MISTAKES TO AVOID:**
- Don't return mock/fake data
- Don't use static timestamps
- Don't ignore RS485 communication errors
- Don't start without HTTP API server

### **✅ SUCCESS INDICATORS:**
- Backend health score >0% (currently 0%)
- RS485 modules show "healthy" not "lost"
- Real-time telemetry with current timestamps
- Backend-Firmware communication working

---

## 📚 **RESOURCES**

### **📁 Key Files to Modify:**
- `firmware_new/src/main.c` - Add HTTP API server
- `firmware_new/src/app/http_api_server.c` - Create HTTP API (NEW FILE)
- `firmware_new/CMakeLists.txt` - Add HTTP API dependencies
- `firmware_new/build/` - Build output directory

### **📖 Dependencies Needed:**
```bash
# Install libmicrohttpd for HTTP server
sudo apt-get install libmicrohttpd-dev

# Update CMakeLists.txt
target_link_libraries(oht50_main microhttpd)
```

### **🔗 Related Issues:**
- Issue #107: Firmware HTTP API Service Not Running
- Issue #108: Backend Using Mock Data Instead of Real Firmware  
- Issue #109: Missing Backend-Firmware Integration Architecture
- Issue #110: RS485 System Health 0% - All Modules Lost Status

---

## ⏰ **TIMELINE**

### **🚨 URGENT (Next 4 Hours):**
- [ ] Start firmware service (any way possible)
- [ ] Verify port 8081 is accessible
- [ ] Test basic connectivity with Backend

### **📅 TODAY (Next 8 Hours):**
- [ ] Implement basic HTTP API endpoints
- [ ] Connect RS485 data to HTTP responses
- [ ] Test Backend-Firmware integration
- [ ] Verify real hardware communication

### **📈 THIS WEEK:**
- [ ] Complete all HTTP API endpoints
- [ ] Add comprehensive error handling
- [ ] Implement performance optimization
- [ ] Add monitoring and diagnostics

---

## 🎯 **PM EXPECTATION**

**FW Team MUST deliver working HTTP API integration TODAY.**

**This is a BLOCKING issue that prevents:**
- Backend testing with real hardware
- System integration validation
- Production deployment preparation
- All real hardware operations

**Success Metric:** Backend integration test showing 100% real hardware communication instead of current 0%.

---

**🚨 CRITICAL PRIORITY - START IMMEDIATELY**

**PM will verify progress in 4 hours. Firmware HTTP API must be accessible by then.**
