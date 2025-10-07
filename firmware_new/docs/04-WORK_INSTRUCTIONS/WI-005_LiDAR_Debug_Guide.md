# LiDAR Debug Guide

## Tổng quan
Hệ thống LiDAR có debug system linh hoạt để hỗ trợ development và troubleshooting mà không làm spam log trong production.

## Debug Configuration

### 1. Enable Debug Mode
Để bật debug mode, uncomment dòng sau trong `hal_lidar.h`:
```c
#define DEBUG_LIDAR_SAFETY
```

### 2. Debug Messages

#### Normal Mode (Production)
- **Safety Alerts:** Chỉ hiển thị khi có vấn đề an toàn
```
[SAFETY] Emergency distance detected: 450 mm (threshold: 500 mm)
```

#### Debug Mode (Development)
- **Input Inspection:** Kiểm tra dữ liệu đầu vào
```
DEBUG: calc_min - point_count=360, scan_complete=1
DEBUG: calc_min - first distances: 1200 1150 1100 1050 1000
DEBUG: calc_min - result=1000
```

## Safety Thresholds

| Threshold | Distance | Purpose |
|-----------|----------|---------|
| `LIDAR_EMERGENCY_STOP_MM` | 500mm | Emergency stop trigger |
| `LIDAR_WARNING_MM` | 1000mm | Warning alert |
| `LIDAR_SAFE_MM` | 2000mm | Safe zone boundary |

## Debug Scenarios

### 1. LiDAR Not Scanning
```
DEBUG: calc_min - point_count=0, scan_complete=0
```
**Nguyên nhân:** LiDAR chưa khởi động hoặc lỗi kết nối

### 2. Invalid Distance Data
```
DEBUG: calc_min - first distances: 0 0 0 0 0
```
**Nguyên nhân:** LiDAR không nhận được tín hiệu phản hồi

### 3. Emergency Stop Triggered
```
[SAFETY] Emergency distance detected: 300 mm (threshold: 500 mm)
```
**Nguyên nhân:** Vật cản quá gần, cần dừng khẩn cấp

## Performance Impact

### Normal Mode
- ✅ Không có performance impact
- ✅ Chỉ log khi cần thiết
- ✅ Phù hợp cho production

### Debug Mode
- ⚠️ Có thể làm chậm hệ thống
- ⚠️ Spam log liên tục
- ⚠️ Chỉ dùng cho development

## Best Practices

1. **Development:** Bật `DEBUG_LIDAR_SAFETY` để debug
2. **Testing:** Tắt debug để test performance
3. **Production:** Luôn tắt debug mode
4. **Troubleshooting:** Bật debug khi có vấn đề

## Example Usage

```c
// Development
#ifdef DEBUG_LIDAR_SAFETY
    printf("DEBUG: LiDAR scan data received\n");
#endif

// Production - chỉ log khi có vấn đề
if (min_distance < LIDAR_EMERGENCY_STOP_MM) {
    printf("[SAFETY] Emergency stop triggered: %u mm\n", min_distance);
}
```

## Changelog
- v1.0: Initial debug system implementation
- v1.1: Added conditional compilation for performance
- v1.2: Added safety alert system
