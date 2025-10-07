# 🔧 **LIDAR & SAFETY INTEGRATION PLAN - OHT-50**

**Phiên bản:** v1.0.0  
**Ngày tạo:** 2025-01-28  
**Team:** FW + EMBED  
**Mục tiêu:** Tích hợp LiDAR vào safety system với basic safety zones

---

## 📋 **PHÂN TÍCH TÌNH TRẠNG HIỆN TẠI**

### **✅ ĐÃ CÓ SẴN:**
- LiDAR HAL interface đầy đủ (`hal_lidar.h/c`)
- Safety Monitor framework (`safety_monitor.h/c`)
- Main application loop (`main.c`)
- API endpoints cho safety (`api_endpoints.c`)

### **❌ CHƯA CÓ:**
- LiDAR integration vào main loop
- Safety zones implementation
- Proximity detection logic
- Real-time safety monitoring

---

## 🎯 **BẢNG PHÂN TÍCH CÔNG VIỆC CẦN LÀM**

| **STT** | **Công việc** | **Mức độ ưu tiên** | **Thời gian ước tính** | **Lý do cần thiết** | **Rủi ro** | **Phụ thuộc** |
|---------|---------------|-------------------|----------------------|-------------------|------------|---------------|
| **1** | **LiDAR Integration vào Main Loop** | 🔴 **CRITICAL** | 2-3 giờ | LiDAR phải chạy liên tục để phát hiện chướng ngại vật | Thấp | Không |
| **2** | **Basic Safety Zones (3 zones)** | 🔴 **CRITICAL** | 4-6 giờ | Cần 3 mức cảnh báo: Emergency(0.5m), Warning(1m), Safe(2m) | Trung bình | Task 1 |
| **3** | **Proximity Detection Logic** | 🟡 **HIGH** | 3-4 giờ | Xử lý dữ liệu LiDAR để phát hiện vật thể gần | Trung bình | Task 1, 2 |
| **4** | **Safety Zone Violation Handling** | 🟡 **HIGH** | 2-3 giờ | Xử lý khi vi phạm vùng an toàn | Trung bình | Task 2, 3 |
| **5** | **API Integration cho Safety Data** | 🟡 **HIGH** | 2-3 giờ | BE cần lấy dữ liệu safety qua API | Thấp | Task 1, 2, 3 |
| **6** | **E-Stop Integration với LiDAR** | 🟡 **HIGH** | 2-3 giờ | E-Stop tự động khi phát hiện vật thể quá gần | Trung bình | Task 1, 2, 3 |
| **7** | **Safety Status LED Integration** | 🟢 **MEDIUM** | 1-2 giờ | LED hiển thị trạng thái safety zones | Thấp | Task 2, 3 |
| **8** | **Configuration Management** | 🟢 **MEDIUM** | 2-3 giờ | Cấu hình safety zones qua API | Thấp | Task 2, 5 |
| **9** | **Testing & Validation** | 🔴 **CRITICAL** | 4-6 giờ | Test toàn bộ safety system | Thấp | Tất cả tasks |
| **10** | **Documentation Update** | 🟢 **MEDIUM** | 1-2 giờ | Cập nhật tài liệu kỹ thuật | Thấp | Tất cả tasks |

---

## 📊 **BẢNG TIẾN ĐỘ THỰC HIỆN**

| **STT** | **Công việc** | **Trạng thái** | **Ngày bắt đầu** | **Ngày hoàn thành** | **Thời gian thực tế** | **Ghi chú** |
|---------|---------------|----------------|------------------|---------------------|---------------------|-------------|
| **1** | **LiDAR Integration vào Main Loop** | ✅ **COMPLETED** | 2025-01-28 | 2025-01-28 | 2.5 giờ | Hoàn thành |
| **2** | **Basic Safety Zones (3 zones)** | ✅ **COMPLETED** | 2025-01-28 | 2025-01-28 | 4 giờ | Hoàn thành |
| **3** | **Proximity Detection Logic** | ❌ **REMOVED** | - | - | - | Xóa - không cần thiết |
| **4** | **Safety Zone Violation Handling** | ✅ **COMPLETED** | 2025-01-28 | 2025-01-28 | 2.5 giờ | Hoàn thành |
| **5** | **API Integration cho Safety Data** | ✅ **COMPLETED** | 2025-01-28 | 2025-01-28 | 3 giờ | Hoàn thành |
| **6** | **E-Stop Integration với LiDAR** | ✅ **COMPLETED** | 2025-01-28 | 2025-01-28 | 2.5 giờ | Hoàn thành |
| **7** | **Safety Status LED Integration** | ✅ **COMPLETED** | 2025-01-28 | 2025-01-28 | 2 giờ | Hoàn thành |
| **8** | **Configuration Management** | ✅ **COMPLETED** | 2025-01-28 | 2025-01-28 | 3 giờ | Hoàn thành |
| **9** | **Testing & Validation** | ✅ **COMPLETED** | 2025-01-28 | 2025-01-28 | 3 giờ | Hoàn thành |
| **10** | **Documentation Update** | ✅ **COMPLETED** | 2025-01-28 | 2025-01-28 | 2 giờ | Hoàn thành |

**Chú thích trạng thái:**
- 🔄 **IN PROGRESS** - Đang thực hiện
- ✅ **COMPLETED** - Hoàn thành
- ⏳ **PENDING** - Chờ thực hiện
- ❌ **BLOCKED** - Bị chặn
- 🔧 **REVIEW** - Cần review

---

## 🔧 **CHI TIẾT TỪNG TASK**

### **Task 1: LiDAR Integration vào Main Loop**

**File cần sửa:**
- `firmware_new/src/main.c`

**Code cần thêm:**
```c
// Trong main() function, sau khi init HAL
lidar_config_t lidar_config = {
    .device_path = "/dev/ttyUSB0",
    .baud_rate = 460800,
    .scan_rate_hz = 10,
    .emergency_stop_mm = 500,
    .warning_mm = 1000,
    .safe_mm = 2000
};

if (hal_lidar_init(&lidar_config) == HAL_STATUS_OK) {
    hal_lidar_start_scanning();
    printf("[LIDAR] Started scanning\n");
}

// Trong main loop, thêm:
if (hal_lidar_get_scan_data(&current_scan) == HAL_STATUS_OK) {
    // Process scan data for safety
}
```

**Lý do cần thiết:** LiDAR phải chạy liên tục để phát hiện chướng ngại vật trong thời gian thực.

---

### **Task 2: Basic Safety Zones (3 zones)**

**File cần sửa:**
- `firmware_new/src/app/core/safety_monitor.c`

**Code cần implement:**
```c
typedef struct {
    uint16_t emergency_zone_mm;    // 500mm - E-Stop
    uint16_t warning_zone_mm;      // 1000mm - Warning
    uint16_t safe_zone_mm;         // 2000mm - Safe
    bool emergency_violated;
    bool warning_violated;
    bool safe_violated;
} basic_safety_zones_t;

static hal_status_t safety_monitor_check_zones(void)
{
    lidar_scan_data_t scan_data;
    if (hal_lidar_get_scan_data(&scan_data) == HAL_STATUS_OK) {
        // Check each zone based on minimum distance
        uint16_t min_distance = lidar_calculate_min_distance(&scan_data);
        
        // Update zone violations
        zones.emergency_violated = (min_distance < zones.emergency_zone_mm);
        zones.warning_violated = (min_distance < zones.warning_zone_mm);
        zones.safe_violated = (min_distance < zones.safe_zone_mm);
    }
    return HAL_STATUS_OK;
}
```

**Lý do cần thiết:** Cần 3 mức cảnh báo an toàn để phản ứng phù hợp với khoảng cách.

---

### **Task 3: Proximity Detection Logic**

**File cần tạo:**
- `firmware_new/src/app/core/proximity_detector.c/h`

**Code cần implement:**
```c
typedef struct {
    uint16_t min_distance_mm;
    uint16_t min_distance_angle;
    bool obstacle_detected;
    uint8_t obstacle_count;
    uint16_t obstacle_angles[8];
} proximity_status_t;

hal_status_t proximity_detect_obstacles(const lidar_scan_data_t *scan_data, proximity_status_t *status)
{
    // Analyze scan data to find obstacles
    // Count obstacles in different directions
    // Calculate minimum distance and angle
    return HAL_STATUS_OK;
}
```

**Lý do cần thiết:** Cần logic xử lý dữ liệu LiDAR để phát hiện và phân tích chướng ngại vật.

---

### **Task 4: Safety Zone Violation Handling**

**File cần sửa:**
- `firmware_new/src/app/core/safety_monitor.c`

**Code cần implement:**
```c
static hal_status_t safety_monitor_handle_zone_violation(void)
{
    if (zones.emergency_violated) {
        // Trigger E-Stop immediately
        system_state_machine_process_event(SYSTEM_EVENT_ESTOP_TRIGGERED);
        hal_led_system_error(); // Red LED
    } else if (zones.warning_violated) {
        // Reduce speed, show warning
        hal_led_system_warning(); // Yellow LED
    } else if (zones.safe_violated) {
        // Normal operation, monitor
        hal_led_system_set(LED_STATE_ON); // Green LED
    }
    return HAL_STATUS_OK;
}
```

**Lý do cần thiết:** Cần xử lý phù hợp khi vi phạm các vùng an toàn khác nhau.

---

### **Task 5: API Integration cho Safety Data**

**File cần sửa:**
- `firmware_new/src/app/api/api_endpoints.c`

**Endpoints cần thêm:**
```c
// GET /api/v1/safety/zones
// GET /api/v1/safety/proximity
// GET /api/v1/lidar/status
// GET /api/v1/lidar/scan_data
```

**Response format:**
```json
{
  "safety_zones": {
    "emergency_violated": false,
    "warning_violated": false,
    "safe_violated": false,
    "min_distance_mm": 1500
  },
  "proximity": {
    "obstacle_detected": false,
    "obstacle_count": 0,
    "min_distance_mm": 1500,
    "min_distance_angle": 180
  }
}
```

**Lý do cần thiết:** Backend cần lấy dữ liệu safety để hiển thị trên dashboard và xử lý.

---

### **Task 6: E-Stop Integration với LiDAR**

**File cần sửa:**
- `firmware_new/src/app/core/safety_monitor.c`

**Code cần implement:**
```c
static hal_status_t safety_monitor_check_emergency_stop(void)
{
    if (zones.emergency_violated) {
        // Auto E-Stop when object too close
        hal_estop_trigger_software();
        safety_monitor_process_event(SAFETY_MONITOR_EVENT_EMERGENCY_STOP, "LiDAR emergency stop");
        return HAL_STATUS_OK;
    }
    return HAL_STATUS_OK;
}
```

**Lý do cần thiết:** Cần E-Stop tự động khi phát hiện vật thể quá gần (< 0.5m).

---

### **Task 7: Safety Status LED Integration**

**File cần sửa:**
- `firmware_new/src/app/core/safety_monitor.c`

**Code cần implement:**
```c
static void update_safety_leds(void)
{
    if (zones.emergency_violated) {
        hal_led_system_error(); // Red blinking
    } else if (zones.warning_violated) {
        hal_led_system_warning(); // Yellow blinking
    } else {
        hal_led_system_set(LED_STATE_ON); // Green solid
    }
}
```

**Lý do cần thiết:** LED hiển thị trạng thái safety zones để operator dễ nhận biết.

---

### **Task 8: Configuration Management**

**File cần sửa:**
- `firmware_new/src/app/api/api_endpoints.c`

**Endpoints cần thêm:**
```c
// PUT /api/v1/safety/zones/config
// GET /api/v1/safety/zones/config
```

**Configuration format:**
```json
{
  "emergency_zone_mm": 500,
  "warning_zone_mm": 1000,
  "safe_zone_mm": 2000,
  "enable_auto_estop": true,
  "enable_led_feedback": true
}
```

**Lý do cần thiết:** Cần cấu hình safety zones qua API để điều chỉnh theo môi trường.

---

### **Task 9: Testing & Validation**

**Files cần tạo:**
- `firmware_new/tests/integration/test_lidar_safety.c`
- `firmware_new/tests/performance/test_safety_response.c`

**Test cases:**
```c
void test_emergency_zone_violation(void)
void test_warning_zone_violation(void)
void test_safe_zone_violation(void)
void test_auto_estop_response(void)
void test_led_feedback(void)
void test_api_integration(void)
```

**Lý do cần thiết:** Cần test toàn bộ safety system để đảm bảo hoạt động đúng.

---

### **Task 10: Documentation Update**

**Files cần cập nhật:**
- `docs/05-IMPLEMENTATION/FIRMWARE/SAFETY_SYSTEM.md`
- `docs/05-IMPLEMENTATION/FIRMWARE/LIDAR_INTEGRATION.md`
- `docs/05-IMPLEMENTATION/backend/03-API-SPECIFICATIONS/SAFETY_API.md`

**Lý do cần thiết:** Cập nhật tài liệu kỹ thuật để team khác hiểu và sử dụng.

---

## 📊 **TỔNG KẾT**

### **Thời gian tổng cộng:** 25-35 giờ
### **Độ ưu tiên:** 
- **Critical (3 tasks):** 10-15 giờ
- **High (3 tasks):** 7-10 giờ  
- **Medium (4 tasks):** 8-10 giờ

### **Rủi ro chính:**
1. **LiDAR hardware issues** - Cần test với hardware thực
2. **Safety timing** - Đảm bảo response time < 100ms
3. **Integration complexity** - Nhiều module tương tác

### **Kết quả mong đợi:**
- ✅ LiDAR chạy liên tục và ổn định
- ✅ 3 safety zones hoạt động đúng
- ✅ E-Stop tự động khi cần
- ✅ API cung cấp dữ liệu safety
- ✅ LED feedback rõ ràng
- ✅ Configuration linh hoạt

---

**🚀 Khuyến nghị:** Bắt đầu với Task 1-3 trước, sau đó làm Task 4-6, cuối cùng là Task 7-10.
