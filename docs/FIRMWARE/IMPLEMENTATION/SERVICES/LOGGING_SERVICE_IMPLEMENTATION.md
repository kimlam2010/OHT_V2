# 📝 LOGGING SERVICE IMPLEMENTATION

**Version:** 1.0  
**Last Updated:** 2025-01-28  
**Team:** FW (Firmware)  
**Status:** 🚧 IN PROGRESS  
**Dependencies:** HAL Layer, Configuration Service

---

## 📋 **TỔNG QUAN**

Logging Service cung cấp hệ thống logging toàn diện cho OHT-50 Master Module, bao gồm:
- Log levels (DEBUG, INFO, WARN, ERROR, FATAL)
- File rotation và log management
- Structured logging với JSON format
- Real-time log streaming
- Log filtering và search
- Performance monitoring
- Telemetry data collection

---

## 🏗️ **KIẾN TRÚC**

### **Service Structure:**
```
firmware/src/services/
├── logging/
│   ├── logging_service.c         # Main service
│   ├── log_manager.c             # Log management
│   ├── log_formatter.c           # Log formatting
│   ├── log_storage.c             # File system storage
│   ├── log_rotation.c            # File rotation
│   ├── log_stream.c              # Real-time streaming
│   ├── log_filter.c              # Log filtering
│   └── log_api.c                 # REST API interface
├── include/
│   └── logging_service.h         # Public API
└── tests/
    └── logging_service_test.c    # Unit tests
```

### **Log Levels:**
```c
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_ERROR = 3,
    LOG_LEVEL_FATAL = 4
} log_level_t;

#define LOG_DEBUG(fmt, ...) log_write(LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  log_write(LOG_LEVEL_INFO,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  log_write(LOG_LEVEL_WARN,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_write(LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) log_write(LOG_LEVEL_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
```

---

## 🔧 **IMPLEMENTATION DETAILS**

### **1. Logging Service Core (logging_service.c)**

```c
// Logging service state
typedef struct {
    log_level_t current_level;
    bool service_initialized;
    bool file_logging_enabled;
    bool console_logging_enabled;
    bool syslog_enabled;
    pthread_mutex_t log_mutex;
    log_config_t config;
    log_stats_t stats;
} logging_service_t;

// Main service functions
int logging_service_init(const log_config_t* config);
int logging_service_deinit(void);
int logging_service_set_level(log_level_t level);
int logging_service_get_level(log_level_t* level);
int logging_service_write(log_level_t level, const char* file, int line, const char* format, ...);
int logging_service_flush(void);
```

### **2. Log Manager (log_manager.c)**

```c
// Log management functions
int log_manager_init(void);
int log_manager_deinit(void);
int log_manager_write_log(const log_entry_t* entry);
int log_manager_get_stats(log_stats_t* stats);
int log_manager_clear_logs(void);
int log_manager_export_logs(const char* filename, time_t start_time, time_t end_time);

// Log entry structure
typedef struct {
    time_t timestamp;
    log_level_t level;
    const char* file;
    int line;
    const char* function;
    const char* message;
    uint32_t thread_id;
    uint32_t sequence_number;
} log_entry_t;
```

### **3. Log Formatter (log_formatter.c)**

```c
// Log formatting functions
int log_formatter_format_text(const log_entry_t* entry, char* buffer, size_t buffer_size);
int log_formatter_format_json(const log_entry_t* entry, char* buffer, size_t buffer_size);
int log_formatter_format_syslog(const log_entry_t* entry, char* buffer, size_t buffer_size);

// Format examples
// Text format: [2025-01-28 10:30:15] [INFO] [main.c:123] System initialized successfully
// JSON format: {"timestamp":"2025-01-28T10:30:15Z","level":"INFO","file":"main.c","line":123,"message":"System initialized"}
```

### **4. Log Storage (log_storage.c)**

```c
// File system operations
int log_storage_init(const char* log_directory);
int log_storage_write_entry(const log_entry_t* entry);
int log_storage_read_entries(time_t start_time, time_t end_time, log_entry_t* entries, size_t max_entries);
int log_storage_get_file_size(const char* filename);
int log_storage_cleanup_old_files(time_t cutoff_time);

// Storage locations
#define LOG_DIRECTORY "/var/log/oht50/"
#define LOG_FILE_PREFIX "oht50_"
#define LOG_FILE_SUFFIX ".log"
#define LOG_ARCHIVE_DIRECTORY "/var/log/oht50/archive/"
```

### **5. Log Rotation (log_rotation.c)**

```c
// File rotation functions
int log_rotation_init(const log_rotation_config_t* config);
int log_rotation_check_and_rotate(void);
int log_rotation_force_rotate(void);
int log_rotation_cleanup_old_files(void);

// Rotation configuration
typedef struct {
    size_t max_file_size_mb;
    uint32_t max_files;
    bool compress_old_files;
    time_t rotation_interval_seconds;
} log_rotation_config_t;
```

### **6. Log Stream (log_stream.c)**

```c
// Real-time streaming functions
int log_stream_init(void);
int log_stream_add_client(int client_fd);
int log_stream_remove_client(int client_fd);
int log_stream_broadcast(const log_entry_t* entry);
int log_stream_get_client_count(void);

// WebSocket streaming
int log_stream_websocket_init(void);
int log_stream_websocket_broadcast(const char* message);
```

### **7. Log Filter (log_filter.c)**

```c
// Log filtering functions
int log_filter_init(const log_filter_config_t* config);
int log_filter_should_log(const log_entry_t* entry);
int log_filter_add_rule(const log_filter_rule_t* rule);
int log_filter_remove_rule(const char* rule_name);

// Filter configuration
typedef struct {
    log_level_t min_level;
    const char* include_patterns[MAX_FILTER_PATTERNS];
    const char* exclude_patterns[MAX_FILTER_PATTERNS];
    uint32_t pattern_count;
} log_filter_config_t;
```

---

## 🌐 **REST API INTERFACE**

### **Logging API Endpoints:**

```c
// GET /api/v1/logs
// Lấy logs với filtering
int log_api_get_logs(http_request_t* req, http_response_t* resp);

// GET /api/v1/logs/stream
// WebSocket stream cho real-time logs
int log_api_stream_logs(http_request_t* req, http_response_t* resp);

// GET /api/v1/logs/stats
// Lấy thống kê logging
int log_api_get_stats(http_request_t* req, http_response_t* resp);

// POST /api/v1/logs/clear
// Xóa logs
int log_api_clear_logs(http_request_t* req, http_response_t* resp);

// POST /api/v1/logs/export
// Export logs
int log_api_export_logs(http_request_t* req, http_response_t* resp);

// PUT /api/v1/logs/level
// Thay đổi log level
int log_api_set_level(http_request_t* req, http_response_t* resp);
```

### **API Response Format:**

```json
{
  "status": "success",
  "data": {
    "logs": [
      {
        "timestamp": "2025-01-28T10:30:15Z",
        "level": "INFO",
        "file": "main.c",
        "line": 123,
        "function": "main",
        "message": "System initialized successfully",
        "thread_id": 1234,
        "sequence_number": 5678
      }
    ],
    "total_count": 1000,
    "filtered_count": 50
  },
  "message": "Logs retrieved successfully"
}
```

---

## 📊 **TELEMETRY & MONITORING**

### **Log Statistics:**
```c
typedef struct {
    uint64_t total_logs;
    uint64_t logs_by_level[5];  // DEBUG, INFO, WARN, ERROR, FATAL
    uint64_t logs_by_module[MAX_MODULES];
    time_t first_log_time;
    time_t last_log_time;
    uint32_t rotation_count;
    uint64_t total_bytes_written;
    uint32_t current_file_size_mb;
} log_stats_t;
```

### **Performance Metrics:**
- Log write latency
- File I/O performance
- Memory usage
- Disk usage
- Rotation frequency

### **Health Checks:**
```c
typedef struct {
    bool service_running;
    bool file_writable;
    bool disk_space_ok;
    uint32_t error_count;
    time_t last_error_time;
    uint32_t client_count;
} log_health_t;

int logging_service_get_health(log_health_t* health);
```

---

## 🔄 **LOGGING LIFECYCLE**

### **1. Initialization:**
```c
// 1. Load logging configuration
log_config_t config;
config_service_get_logging_config(&config);

// 2. Initialize logging service
if (logging_service_init(&config) != 0) {
    // Fallback to console logging
    logging_service_init_console_only();
}

// 3. Set initial log level
logging_service_set_level(config.level);

// 4. Start log rotation
log_rotation_init(&config.rotation);
```

### **2. Runtime Logging:**
```c
// 1. Check if log should be written
if (log_filter_should_log(&entry)) {
    // 2. Format log entry
    char formatted_log[1024];
    log_formatter_format_json(&entry, formatted_log, sizeof(formatted_log));
    
    // 3. Write to storage
    log_storage_write_entry(&entry);
    
    // 4. Broadcast to stream clients
    log_stream_broadcast(&entry);
    
    // 5. Check rotation
    log_rotation_check_and_rotate();
}
```

### **3. Shutdown:**
```c
// 1. Flush pending logs
logging_service_flush();

// 2. Close all clients
log_stream_close_all_clients();

// 3. Final rotation
log_rotation_force_rotate();

// 4. Deinitialize service
logging_service_deinit();
```

---

## 🧪 **TESTING STRATEGY**

### **Unit Tests:**
```c
// Test logging functionality
void test_logging_service_init(void);
void test_logging_service_write(void);
void test_logging_service_levels(void);

// Test log formatting
void test_log_formatter_text(void);
void test_log_formatter_json(void);

// Test log storage
void test_log_storage_write_read(void);
void test_log_storage_rotation(void);

// Test log filtering
void test_log_filter_rules(void);
void test_log_filter_patterns(void);
```

### **Integration Tests:**
```c
// Test logging with real modules
void test_logging_with_power_module(void);
void test_logging_with_motor_module(void);
void test_logging_with_io_module(void);

// Test logging persistence
void test_logging_persistence_across_reboots(void);
void test_logging_performance_under_load(void);
```

---

## 🔒 **SECURITY CONSIDERATIONS**

### **Access Control:**
- Log access requires authentication
- Sensitive data filtering
- Log file permissions
- Audit trail for log access

### **Data Protection:**
- Log encryption at rest
- Secure log transmission
- Data retention policies
- GDPR compliance

---

## 📚 **DEPENDENCIES**

### **Internal Dependencies:**
- HAL Layer (File System)
- Configuration Service
- Module Management Service
- Safety System

### **External Dependencies:**
- JSON library (jansson)
- File system access
- HTTP server (for API)
- WebSocket library (for streaming)
- Compression library (for rotation)

---

## 🚀 **DEPLOYMENT**

### **Build Configuration:**
```makefile
# Add to firmware Makefile
LOGGING_SERVICE_SOURCES = \
    src/services/logging/logging_service.c \
    src/services/logging/log_manager.c \
    src/services/logging/log_formatter.c \
    src/services/logging/log_storage.c \
    src/services/logging/log_rotation.c \
    src/services/logging/log_stream.c \
    src/services/logging/log_filter.c \
    src/services/logging/log_api.c

LOGGING_SERVICE_HEADERS = \
    include/logging_service.h
```

### **Installation:**
```bash
# Create log directory
sudo mkdir -p /var/log/oht50
sudo mkdir -p /var/log/oht50/archive

# Set permissions
sudo chown oht50:oht50 /var/log/oht50
sudo chmod 755 /var/log/oht50

# Create logrotate configuration
sudo cp config/oht50-logrotate /etc/logrotate.d/oht50
```

---

## 📝 **CHANGELOG**

**v1.0 (2025-01-28):**
- ✅ Initial implementation
- ✅ Log levels và formatting
- ✅ File storage và rotation
- ✅ Real-time streaming
- ✅ REST API interface
- ✅ Log filtering
- ✅ Performance monitoring
- ✅ Unit test framework

---

**Next Steps:**
- [ ] Implement log encryption
- [ ] Add log compression
- [ ] Implement log search functionality
- [ ] Add log analytics dashboard
- [ ] Implement log retention policies
- [ ] Add log backup functionality