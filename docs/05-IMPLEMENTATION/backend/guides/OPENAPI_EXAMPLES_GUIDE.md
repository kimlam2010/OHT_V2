# OpenAPI Examples Guide - OHT-50 Backend

## Mục tiêu
Hướng dẫn thêm examples chi tiết vào Pydantic models để cải thiện OpenAPI documentation tại `/docs`.

## Quy trình thêm examples

### 1. Cấu trúc json_schema_extra
```python
model_config = ConfigDict(
    json_schema_extra={
        "example": {
            "field1": "value1",
            "field2": 123,
            "nested": {
                "subfield": "subvalue"
            }
        }
    }
)
```

### 2. Examples cần thêm cho từng model

#### Config Models (`backend/app/models/config.py`)
- **MotorConfig**: interface="rs485", r=0.05, vmax_mms=500.0, amax_mms2=300.0, jmax_mms3=5000.0
- **EncoderConfig**: type="incremental", resolution_ppr=2048, invert=False
- **SafetyZone**: start_mm=0.0, end_mm=200.0, vmax_mms=200.0
- **SafetyConfig**: zones array với 2 zones, estop_reset_policy="manual"
- **TelemetryConfig**: ws_hz=10, history_size=1000
- **RS485Config**: device="/dev/ttyOHT485", baud=115200, timeout=1.0
- **CANConfig**: iface="can0", bitrate=500000
- **SystemConfig**: Full config với tất cả sections
- **ConfigHistory**: version=2, action="update", user="alice"
- **ConfigResponse**: success=True, message="Configuration updated successfully"

#### Telemetry Models (`backend/app/models/telemetry.py`)
- **TelemetryTarget**: pos_mm=150.0, vel_mms=100.0
- **TelemetrySafety**: estop=False, zone_blocked=False, interlock_active=False
- **TelemetryEncoder**: health="ok", count=1024
- **TelemetryStatus**: state="Idle", pos_mm=150.0, vel_mms=0.0, acc_mms2=0.0
- **TelemetryData**: Complete telemetry với timestamp, status, performance metrics
- **TelemetryHistory**: data array, total=100, page=1, page_size=10
- **LogRecord**: time="2024-01-15T10:30:00Z", level="INFO", source="api"
- **AuditRecord**: Complete audit trail với user, action, target, signature

#### Center Communication Models (`backend/app/models/center_comm.py`)
- **Position**: x=100.0, y=200.0, z=0.0, theta=0.0
- **Mission**: mission_id="m1", mission_type="move_to", target_position
- **MissionResponse**: status="in_progress", progress=50.0
- **PermissionRequest**: request_id="req1", action="move", urgency="normal"
- **PermissionResponse**: granted=True, conditions={"zone": "A"}
- **StatusReport**: device_id="OHT-50-01", status="Idle", battery_level=85.0
- **CenterMessage**: message_id="msg1", message_type="status_update"
- **CommunicationConfig**: center_url="https://center.example.com", api_key="secret"
- **CommunicationStatus**: connected=True, connection_quality="excellent"

### 3. Validation Examples
Thêm examples cho các trường hợp validation:
- **Valid**: Giá trị hợp lệ theo business rules
- **Edge cases**: Giá trị biên (min/max)
- **Error cases**: Giá trị gây lỗi validation

### 4. API Response Examples
Thêm examples cho các response patterns:
- **Success**: 200 OK với data đầy đủ
- **Error**: 400/422 với error details
- **Empty**: 204 No Content
- **Paginated**: 200 với pagination metadata

## Checklist Implementation
- [ ] MotorConfig examples
- [ ] EncoderConfig examples  
- [ ] SafetyZone examples
- [ ] SafetyConfig examples
- [ ] TelemetryConfig examples
- [ ] RS485Config examples
- [ ] CANConfig examples
- [ ] SystemConfig examples
- [ ] ConfigHistory examples
- [ ] ConfigResponse examples
- [ ] TelemetryTarget examples
- [ ] TelemetrySafety examples
- [ ] TelemetryEncoder examples
- [ ] TelemetryStatus examples
- [ ] TelemetryData examples
- [ ] TelemetryHistory examples
- [ ] LogRecord examples
- [ ] AuditRecord examples
- [ ] Position examples
- [ ] Mission examples
- [ ] MissionResponse examples
- [ ] PermissionRequest examples
- [ ] PermissionResponse examples
- [ ] StatusReport examples
- [ ] CenterMessage examples
- [ ] CommunicationConfig examples
- [ ] CommunicationStatus examples

## Testing Examples
Sau khi thêm examples:
1. Restart backend server
2. Truy cập `/docs` 
3. Kiểm tra examples hiển thị đúng trong OpenAPI UI
4. Test với Postman collection
5. Verify với curl suite

## Maintenance
- Cập nhật examples khi thay đổi model structure
- Đồng bộ examples với business logic
- Review examples định kỳ với team
