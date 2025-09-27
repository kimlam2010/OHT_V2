# 📊 Module Telemetry Validation API Documentation

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-01-28  
**Issue:** #144 - Add Value Range Validation for Module Telemetry Data

## 🎯 **TỔNG QUAN**

API Module Telemetry Validation cung cấp **value range validation** cho module telemetry data, đảm bảo data integrity và hỗ trợ Frontend validation.

### **Tính năng chính:**
- ✅ **Server-side validation** - Validate data từ Firmware
- ✅ **API validation** - Validate input từ Frontend
- ✅ **Data integrity** - Đảm bảo data hợp lệ
- ✅ **Error handling** - Proper error responses
- ✅ **Frontend integration** - Support Frontend validation

---

## 📋 **API ENDPOINTS**

### **1. GET /api/v1/modules/{module_id}/telemetry**

**Mô tả:** Lấy module telemetry data với value range validation

**Parameters:**
- `module_id` (int): Module ID

**Response:**
```json
{
  "success": true,
  "data": {
    "module_id": 2,
    "module_name": "Power Module",
    "telemetry": {
      "voltage": {
        "value": 24.1,
        "min_value": 0.0,
        "max_value": 30.0,
        "unit": "V",
        "description": "Supply voltage",
        "valid": true
      },
      "current": {
        "value": 2.5,
        "min_value": 0.0,
        "max_value": 10.0,
        "unit": "A",
        "description": "Load current",
        "valid": true
      }
    },
    "timestamp": 1706441400,
    "validation_status": "valid"
  },
  "message": "Module telemetry retrieved successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

**Error Response:**
```json
{
  "success": false,
  "detail": "Module 2 not found"
}
```

---

### **2. POST /api/v1/modules/{module_id}/telemetry/validate**

**Mô tả:** Validate module telemetry data against value ranges

**Parameters:**
- `module_id` (int): Module ID

**Request Body:**
```json
{
  "module_id": 2,
  "module_name": "Power Module",
  "telemetry": {
    "voltage": {
      "value": 24.1,
      "min_value": 0.0,
      "max_value": 30.0,
      "unit": "V",
      "description": "Supply voltage"
    }
  },
  "timestamp": 1706441400
}
```

**Response:**
```json
{
  "success": true,
  "validation_status": "valid",
  "valid": true,
  "errors": [],
  "validation_details": {
    "module_id": 2,
    "module_name": "Power Module",
    "total_fields": 1,
    "valid_fields": 1,
    "invalid_fields": 0,
    "field_details": {
      "voltage": {
        "value": 24.1,
        "valid": true,
        "unit": "V"
      }
    }
  },
  "message": "Validation completed successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

**Invalid Response:**
```json
{
  "success": true,
  "validation_status": "invalid",
  "valid": false,
  "errors": [
    {
      "field": "voltage",
      "value": 35.0,
      "range": {"min": 0.0, "max": 30.0},
      "error": "Value 35.0 is out of range [0.0, 30.0]"
    }
  ],
  "message": "Validation completed successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

### **3. POST /api/v1/modules/{module_id}/telemetry/update**

**Mô tả:** Update module telemetry data với validation

**Parameters:**
- `module_id` (int): Module ID

**Request Body:**
```json
{
  "module_id": 2,
  "module_name": "Power Module",
  "telemetry": {
    "voltage": {
      "value": 24.1,
      "min_value": 0.0,
      "max_value": 30.0,
      "unit": "V",
      "description": "Supply voltage"
    }
  },
  "timestamp": 1706441400,
  "force": false
}
```

**Response:**
```json
{
  "success": true,
  "validation_status": "valid",
  "data": {
    "module_id": 2,
    "updated_fields": ["voltage"],
    "validation_result": {
      "valid": true,
      "errors": [],
      "validation_details": {
        "module_id": 2,
        "total_fields": 1,
        "valid_fields": 1,
        "invalid_fields": 0
      }
    }
  },
  "message": "Telemetry updated successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

**Force Update Response:**
```json
{
  "success": true,
  "validation_status": "invalid_forced",
  "data": {
    "module_id": 2,
    "updated_fields": ["voltage"],
    "validation_result": null
  },
  "message": "Telemetry updated successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

### **4. GET /api/v1/modules/{module_id}/telemetry/field-configs**

**Mô tả:** Lấy field configurations cho module telemetry validation

**Parameters:**
- `module_id` (int): Module ID

**Response:**
```json
{
  "success": true,
  "data": {
    "module_id": 2,
    "field_configs": {
      "voltage": {
        "min_value": 0.0,
        "max_value": 30.0,
        "unit": "V",
        "description": "Supply voltage"
      },
      "current": {
        "min_value": 0.0,
        "max_value": 10.0,
        "unit": "A",
        "description": "Load current"
      },
      "temperature": {
        "min_value": -40.0,
        "max_value": 85.0,
        "unit": "°C",
        "description": "Operating temperature"
      }
    },
    "total_fields": 3
  },
  "message": "Field configurations retrieved successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

### **5. PUT /api/v1/modules/{module_id}/telemetry/field-configs/{field_name}**

**Mô tả:** Update field configuration cho telemetry validation

**Parameters:**
- `module_id` (int): Module ID
- `field_name` (string): Field name

**Request Body:**
```json
{
  "min_value": 10.0,
  "max_value": 40.0,
  "unit": "V",
  "description": "Updated voltage range"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "module_id": 2,
    "field_name": "voltage",
    "updated_config": {
      "min_value": 10.0,
      "max_value": 40.0,
      "unit": "V",
      "description": "Updated voltage range"
    }
  },
  "message": "Field configuration voltage updated successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

### **6. GET /api/v1/modules/telemetry/validation-status**

**Mô tả:** Lấy overall telemetry validation status

**Response:**
```json
{
  "success": true,
  "data": {
    "total_modules": 5,
    "valid_modules": 4,
    "invalid_modules": 1,
    "validation_rate": 80.0,
    "recent_errors": [
      {
        "module_id": 2,
        "module_name": "Power Module",
        "errors": [
          {
            "field": "voltage",
            "value": 35.0,
            "range": {"min": 0.0, "max": 30.0},
            "error": "Value 35.0 is out of range [0.0, 30.0]"
          }
        ],
        "timestamp": 1706441400
      }
    ],
    "last_updated": "2025-01-28T10:30:00Z"
  },
  "message": "Validation status retrieved successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

## 🔧 **TELEMETRY FIELD CONFIGURATIONS**

### **Predefined Field Configurations:**

```python
TELEMETRY_FIELD_CONFIGS = {
    "voltage": {
        "min_value": 0.0,
        "max_value": 30.0,
        "unit": "V",
        "description": "Supply voltage"
    },
    "current": {
        "min_value": 0.0,
        "max_value": 10.0,
        "unit": "A", 
        "description": "Load current"
    },
    "power": {
        "min_value": 0.0,
        "max_value": 300.0,
        "unit": "W",
        "description": "Power consumption"
    },
    "temperature": {
        "min_value": -40.0,
        "max_value": 85.0,
        "unit": "°C",
        "description": "Operating temperature"
    },
    "battery_level": {
        "min_value": 0.0,
        "max_value": 100.0,
        "unit": "%",
        "description": "Battery level percentage"
    },
    "motor_speed": {
        "min_value": 0.0,
        "max_value": 5000.0,
        "unit": "RPM",
        "description": "Motor rotation speed"
    },
    "position": {
        "min_value": 0.0,
        "max_value": 1000.0,
        "unit": "mm",
        "description": "Position on rail"
    },
    "pressure": {
        "min_value": 0.0,
        "max_value": 10.0,
        "unit": "bar",
        "description": "Hydraulic pressure"
    }
}
```

---

## 📊 **DATA MODELS**

### **TelemetryField Model:**
```python
class TelemetryField(BaseModel):
    value: float                    # Current field value
    min_value: float               # Minimum allowed value
    max_value: float               # Maximum allowed value
    unit: str                      # Value unit (e.g., 'V', 'A', '%', '°C')
    description: str               # Field description
    valid: bool                    # Field validation status
```

### **ModuleTelemetry Model:**
```python
class ModuleTelemetry(BaseModel):
    module_id: int                 # Module ID
    module_name: str               # Module name
    telemetry: Dict[str, TelemetryField]  # Telemetry fields with validation
    timestamp: int                 # Timestamp
    validation_status: str         # Overall validation status
```

---

## 🚨 **ERROR HANDLING**

### **HTTP Status Codes:**
- **200 OK:** Request successful
- **400 Bad Request:** Invalid request data
- **404 Not Found:** Module not found
- **500 Internal Server Error:** Server error

### **Error Response Format:**
```json
{
  "success": false,
  "detail": "Error message description",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **Validation Error Format:**
```json
{
  "success": true,
  "valid": false,
  "errors": [
    {
      "field": "voltage",
      "value": 35.0,
      "range": {"min": 0.0, "max": 30.0},
      "error": "Value 35.0 is out of range [0.0, 30.0]"
    }
  ],
  "message": "Validation failed",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

## 🧪 **TESTING**

### **Unit Tests:**
```bash
# Run telemetry validation tests
cd backend
python -m pytest tests/test_telemetry_validation.py -v
```

### **Test Coverage:**
- ✅ TelemetryField validation
- ✅ ModuleTelemetry validation
- ✅ ValidationService functionality
- ✅ DatabaseService operations
- ✅ End-to-end integration

---

## 📈 **PERFORMANCE REQUIREMENTS**

### **Response Time Targets:**
- **GET /telemetry:** < 200ms
- **POST /validate:** < 100ms
- **POST /update:** < 300ms
- **GET /field-configs:** < 50ms
- **GET /validation-status:** < 150ms

### **Success Metrics:**
- **API Response Time:** < 200ms
- **Validation Accuracy:** 100% accurate validation
- **Test Coverage:** > 90% for new functions
- **Error Rate:** < 1% for valid requests
- **Frontend Integration:** Ready for Frontend validation

---

## 🔗 **INTEGRATION**

### **Frontend Integration:**
```javascript
// Example Frontend usage
const validateTelemetry = async (moduleId, telemetryData) => {
  const response = await fetch(`/api/v1/modules/${moduleId}/telemetry/validate`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(telemetryData)
  });
  
  const result = await response.json();
  
  if (result.valid) {
    console.log('✅ Telemetry validation passed');
    return result.validation_details;
  } else {
    console.log('❌ Telemetry validation failed:', result.errors);
    return null;
  }
};
```

### **Firmware Integration:**
```python
# Example Firmware integration
async def send_validated_telemetry(module_id, telemetry_data):
    # Validate data first
    validation_result = await validation_service.validate_telemetry_data(telemetry_data)
    
    if validation_result.valid:
        # Send to firmware
        await firmware_service.update_module_telemetry(module_id, telemetry_data)
        return True
    else:
        logger.warning(f"Invalid telemetry data: {validation_result.errors}")
        return False
```

---

## 📝 **CHANGELOG**

### **v1.0 (2025-01-28):**
- ✅ Created enhanced telemetry models với value range validation
- ✅ Implemented ValidationService với range validation logic
- ✅ Added enhanced API endpoints với validation
- ✅ Created database service cho validated data
- ✅ Added comprehensive unit và integration tests
- ✅ Updated API documentation

---

**🏷️ Labels:** `backend`, `api`, `validation`, `telemetry`, `frontend-integration`  
**👥 Team:** Backend Team  
**📅 Completed:** 2025-01-28  
**🎯 Milestone:** Frontend Integration Enhancement
