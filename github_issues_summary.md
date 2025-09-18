# 📋 **GitHub Issues Summary - RS485 Telemetry Table Implementation**

## 🎯 **OVERVIEW:**
Implement Telemetry Table trong Module Detail Modal với format giống công cụ Modbus

---

## 🔧 **ISSUE 1: [FIRMWARE] RS485 Modules WebSocket Telemetry Support**

### **Title:** 
`🔧 [FIRMWARE] Implement RS485 Modules WebSocket Telemetry Support`

### **Description:**
Implement WebSocket telemetry streaming cho RS485 modules để support Frontend Telemetry Table

### **Requirements:**

#### **1. WebSocket Telemetry Format (theo Firmware registers):**
```json
{
  "type": "telemetry",
  "data": {
    "module_addr": "0x02",
    "module_name": "Power",
    "registers": [
      // DalyBMS Status Registers
      {"addr": "0x0000", "name": "Battery_Voltage", "value": 24.2, "unit": "V", "mode": "R", "scaling": 0.1},
      {"addr": "0x0001", "name": "Battery_Current", "value": 2.1, "unit": "A", "mode": "R", "scaling": 0.1},
      {"addr": "0x0002", "name": "Battery_SOC", "value": 85.4, "unit": "%", "mode": "R", "scaling": 0.1},
      {"addr": "0x0003", "name": "Max_Cell_Voltage", "value": 3450, "unit": "mV", "mode": "R"},
      {"addr": "0x0004", "name": "Min_Cell_Voltage", "value": 3420, "unit": "mV", "mode": "R"},
      {"addr": "0x0008", "name": "Temperature", "value": 42, "unit": "°C", "mode": "R"},
      {"addr": "0x0009", "name": "Connection_Status", "value": 1, "unit": "bool", "mode": "R"},
      {"addr": "0x000B", "name": "Charge_MOS", "value": 1, "unit": "bool", "mode": "R"},
      {"addr": "0x000C", "name": "Discharge_MOS", "value": 1, "unit": "bool", "mode": "R"},
      {"addr": "0x0011", "name": "Charge_Status", "value": 0, "unit": "bool", "mode": "R"},
      
      // SK60X Power Supply Registers  
      {"addr": "0x0032", "name": "Output_Voltage", "value": 24.0, "unit": "V", "mode": "R", "scaling": 0.1},
      {"addr": "0x0033", "name": "Output_Current", "value": 2.0, "unit": "A", "mode": "R", "scaling": 0.1},
      {"addr": "0x0034", "name": "Output_Power", "value": 48.0, "unit": "W", "mode": "R", "scaling": 0.1},
      {"addr": "0x003C", "name": "Output_ON_OFF", "value": 1, "unit": "bool", "mode": "RW"},
      
      // INA219 Multi-rail Monitoring
      {"addr": "0x0040", "name": "12V_Output_Voltage", "value": 12.1, "unit": "V", "mode": "R", "scaling": 0.1},
      {"addr": "0x0041", "name": "12V_Output_Current", "value": 1.5, "unit": "A", "mode": "R", "scaling": 0.1},
      {"addr": "0x0043", "name": "5V_Output_Voltage", "value": 5.0, "unit": "V", "mode": "R", "scaling": 0.1},
      {"addr": "0x0044", "name": "5V_Output_Current", "value": 0.8, "unit": "A", "mode": "R", "scaling": 0.1},
      {"addr": "0x0046", "name": "3V3_Output_Voltage", "value": 3.3, "unit": "V", "mode": "R", "scaling": 0.1},
      {"addr": "0x0047", "name": "3V3_Output_Current", "value": 0.5, "unit": "A", "mode": "R", "scaling": 0.1},
      
      // Relay Controls
      {"addr": "0x0049", "name": "12V_Relay", "value": 1, "unit": "bool", "mode": "RW"},
      {"addr": "0x004A", "name": "5V_Relay", "value": 1, "unit": "bool", "mode": "RW"},
      {"addr": "0x004B", "name": "3V3_Relay", "value": 1, "unit": "bool", "mode": "RW"}
    ],
    "timestamp": "2025-09-18T13:47:15.123456"
  }
}
```

#### **2. Required Endpoints:**
- `GET /api/v1/modules/{addr}/telemetry` - Get module telemetry table
- `WS /ws/telemetry` - Real-time telemetry streaming

#### **3. Module Types Support (theo Firmware thực tế):**

**Power (0x02) - DalyBMS + SK60X + INA219:**
- Battery: Voltage, Current, SOC, Cell Voltages (0-5), Temperature (0-1)
- Charging: Charge/Discharge MOS, Charge Status, Charge Request
- Power Supply: 12V/5V/3.3V Output (V/I/P), Relay States
- BMS: Life Cycle, Residual Capacity, Fault Flags, Connection Status
- Thresholds: Cell/Pack voltage thresholds, Usage threshold

**Safety (0x03) - Proximity Sensors + Relays:**
- Safety: Status, Emergency Stop, Safety Zone, Proximity Alert
- Sensors: 4 Analog Inputs (raw + processed), 4 Digital Inputs
- Relays: 4 Relay Controls, Relay Output Status
- Zones: 4 Zone Thresholds, Proximity Threshold, Safety Mode
- System: Temperature, Voltage, Response Time, Auto Reset

**Travel Motor (0x04) - Motor Driver:**
- Motion: Speed (Target/Actual), Position (Target/Actual), Direction
- Control: Enable, Mode, Acceleration, Deceleration, Jerk, Torque Limit
- Status: Running, Ready, Fault, Warning, Emergency Stop, Home, Limit Switch
- Sensors: Encoder Status, Temperature, Voltage, Current
- Commands: Move, Stop, Home, Reset Faults, Hard Stop

**Dock & Location (0x05) - IMU + Positioning:**
- Position: Target Position, Current Position, Distance to Dock
- Alignment: Alignment Angle, Approach Speed, Final Speed, Accuracy Threshold
- Control: Enable, Start/Stop Docking, Emergency Stop, Reset Faults, Calibrate
- Status: Dock Status, IMU Status, Fault Status
- Settings: Set Dock Position, Approach Distance

**Master (0x06) - CHƯA CÓ IMPLEMENTATION:**
- Communication Stats, System Status (cần implement)

**Lifter (0x07) - CHƯA CÓ IMPLEMENTATION:**
- Lift Position, Load Status (cần implement)

**Cargo (0x08) - CHƯA CÓ IMPLEMENTATION:**
- Cargo Status, Weight (cần implement)

#### **4. Detailed Register Examples:**

**Travel Motor (0x04) Registers:**
```json
"registers": [
  {"addr": "0x0000", "name": "Motor_Enable", "value": 1, "unit": "bool", "mode": "RW"},
  {"addr": "0x0001", "name": "Operation_Mode", "value": 2, "unit": "enum", "mode": "RW"},
  {"addr": "0x0002", "name": "Speed_Target", "value": 1500, "unit": "rpm", "mode": "RW"},
  {"addr": "0x0003", "name": "Speed_Actual", "value": 1485, "unit": "rpm", "mode": "R"},
  {"addr": "0x0004", "name": "Position_Target", "value": 15000, "unit": "mm", "mode": "RW"},
  {"addr": "0x0005", "name": "Position_Actual", "value": 14998, "unit": "mm", "mode": "R"},
  {"addr": "0x0006", "name": "Direction", "value": 1, "unit": "enum", "mode": "RW"},
  {"addr": "0x000B", "name": "Temperature", "value": 45, "unit": "°C", "mode": "R"},
  {"addr": "0x000C", "name": "Voltage", "value": 240, "unit": "V", "mode": "R", "scaling": 0.1},
  {"addr": "0x000D", "name": "Current", "value": 32, "unit": "A", "mode": "R", "scaling": 0.1},
  {"addr": "0x000E", "name": "Fault_Status", "value": 0, "unit": "bitmask", "mode": "R"}
]
```

**Safety (0x03) Registers:**
```json
"registers": [
  {"addr": "0x0000", "name": "Safety_Status", "value": 1, "unit": "bool", "mode": "R"},
  {"addr": "0x0001", "name": "Emergency_Stop", "value": 0, "unit": "bool", "mode": "R"},
  {"addr": "0x0002", "name": "Safety_Zone", "value": 2, "unit": "enum", "mode": "R"},
  {"addr": "0x0003", "name": "Proximity_Alert", "value": 0, "unit": "bool", "mode": "R"},
  {"addr": "0x0010", "name": "Analog_Input_1", "value": 2.5, "unit": "V", "mode": "R"},
  {"addr": "0x0011", "name": "Analog_Input_2", "value": 1.8, "unit": "V", "mode": "R"},
  {"addr": "0x0012", "name": "Analog_Input_3", "value": 3.1, "unit": "V", "mode": "R"},
  {"addr": "0x0013", "name": "Analog_Input_4", "value": 0.9, "unit": "V", "mode": "R"},
  {"addr": "0x0020", "name": "Digital_Inputs", "value": 12, "unit": "bitmask", "mode": "R"},
  {"addr": "0x0030", "name": "Relay_Control", "value": 5, "unit": "bitmask", "mode": "RW"}
]
```

**Dock & Location (0x05) Registers:**
```json
"registers": [
  {"addr": "0x7000", "name": "Position_Target", "value": 1500, "unit": "mm", "mode": "RW"},
  {"addr": "0x7001", "name": "Current_Position", "value": 1498, "unit": "mm", "mode": "R"},
  {"addr": "0x7002", "name": "Approach_Speed", "value": 100, "unit": "mm/s", "mode": "RW"},
  {"addr": "0x7005", "name": "Dock_Status", "value": 2, "unit": "enum", "mode": "R"},
  {"addr": "0x7006", "name": "Distance_to_Dock", "value": 25, "unit": "mm", "mode": "R"},
  {"addr": "0x7007", "name": "Alignment_Angle", "value": 15, "unit": "deg*10", "mode": "R"},
  {"addr": "0x8000", "name": "Dock_Enable", "value": 1, "unit": "bool", "mode": "RW"},
  {"addr": "0x8001", "name": "Start_Docking", "value": 0, "unit": "cmd", "mode": "W"},
  {"addr": "0x9000", "name": "Fault_Status", "value": 0, "unit": "bitmask", "mode": "R"}
]
```

#### **5. Technical Requirements:**
- ✅ Match existing `modules.yaml` format
- ✅ WebSocket streaming mỗi 1-2 giây
- ✅ **THỰC TẾ Modbus register mapping** (như trên)
- ✅ Error handling cho communication failures
- ✅ Register metadata (address, name, unit, mode, scaling)
- ✅ **Chỉ implement modules ĐÃ CÓ:** Power, Safety, Travel Motor, Dock & Location
- ⚠️ **KHÔNG implement:** Master, Lifter, Cargo (chưa có firmware)

**Labels:** firmware, rs485, telemetry, websocket, high-priority  
**Timeline:** 2-3 ngày

---

## 💻 **ISSUE 2: [BACKEND] RS485 Telemetry API Integration**

### **Title:**
`💻 [BACKEND] Implement RS485 Telemetry API Integration`

### **Description:**
Tạo API endpoints và WebSocket integration cho RS485 module telemetry table

### **Requirements:**

#### **1. New API Endpoints:**
```python
GET    /api/v1/rs485/modules/{addr}/telemetry    # Get module telemetry table
POST   /api/v1/rs485/modules/{addr}/telemetry    # Update module register (if writable)
```

#### **2. Response Format:**
```json
{
  "success": true,
  "data": {
    "module_addr": "0x02",
    "module_name": "Power", 
    "registers": [
      {
        "address": "0x0001",
        "name": "Battery Level",
        "value": 85.4,
        "unit": "%",
        "mode": "R",
        "status": "normal",
        "threshold_warning": 20,
        "threshold_critical": 10
      }
    ],
    "last_updated": "2025-09-18T13:47:15.123456"
  }
}
```

#### **3. Schema Updates:**
- `RS485RegisterInfo` - Register information
- `RS485ModuleTelemetry` - Module telemetry data
- `RS485TelemetryResponse` - API response format

#### **4. Service Layer:**
- `get_module_telemetry(address)` method
- `update_module_register(address, register, value)` method
- WebSocket telemetry broadcasting

#### **5. Mock Service:**
- Generate telemetry registers cho mỗi module type
- Simulate register value changes
- Support writable registers

**Labels:** backend, rs485, telemetry, api, websocket, high-priority  
**Timeline:** 1-2 ngày

---

## 🎨 **ISSUE 3: [DESIGN] Update Monitoring Interface - Add Telemetry Table Modal**

### **Title:**
`🎨 [DESIGN] Update Monitoring Interface - Add Telemetry Table Modal`

### **Description:**
Update `5-1-monitoring-interface.html` để thêm Telemetry Tab với table format giống công cụ Modbus

### **Requirements:**

#### **1. Modal Structure Update:**
- Thêm "Telemetry" tab vào existing modal
- Table layout với columns: Address, Name, Value, Unit, Mode, Status
- Real-time value updates từ WebSocket
- Value change highlighting (flash effect)

#### **2. Telemetry Table Example (Power Module 0x02):**
```html
<table class="telemetry-table">
  <thead>
    <tr>
      <th>Address</th><th>Parameter Name</th><th>Value</th><th>Unit</th><th>Mode</th><th>Status</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>0x0000</td><td>Battery Voltage</td><td class="value-cell">24.2</td><td>V</td><td>R</td><td>🟢 Normal</td></tr>
    <tr><td>0x0001</td><td>Battery Current</td><td class="value-cell">2.1</td><td>A</td><td>R</td><td>🟢 Normal</td></tr>
    <tr><td>0x0002</td><td>Battery SOC</td><td class="value-cell">85.4</td><td>%</td><td>R</td><td>🟢 Normal</td></tr>
    <tr><td>0x0003</td><td>Max Cell Voltage</td><td class="value-cell">3450</td><td>mV</td><td>R</td><td>🟢 Normal</td></tr>
    <tr><td>0x0008</td><td>Temperature</td><td class="value-cell">42</td><td>°C</td><td>R</td><td>🟢 Normal</td></tr>
    <tr><td>0x003C</td><td>Output ON/OFF</td><td class="value-cell">1</td><td>bool</td><td>RW</td><td>🟢 Normal</td></tr>
    <tr><td>0x0040</td><td>12V Output Voltage</td><td class="value-cell">12.1</td><td>V</td><td>R</td><td>🟢 Normal</td></tr>
    <tr><td>0x0049</td><td>12V Relay</td><td class="value-cell">1</td><td>bool</td><td>RW</td><td>🟢 Normal</td></tr>
  </tbody>
</table>
```

#### **3. Table Features:**
- ✅ **Modbus-style table** với monospace font
- ✅ **Real-time updates** từ WebSocket (mỗi 2s)
- ✅ **Value highlighting** khi thay đổi (flash yellow)
- ✅ **Status indicators** (🟢🟡🔴 based on thresholds)
- ✅ **Writable registers** (RW mode với input field)
- ✅ **Export functionality** (CSV/Excel)
- ✅ **Responsive design** (mobile table)
- ✅ **Register filtering** (by mode: R/W/RW)
- ✅ **Search functionality** (find register by name/address)

#### **3. JavaScript Integration:**
- Update `RS485WebSocketClient` để handle telemetry messages
- Add `updateTelemetryTable()` function
- Add `generateTelemetryRows()` function
- Integrate với existing modal system

#### **4. CSS Styling:**
- Modbus tool appearance
- Professional table styling
- Animation cho value changes
- Status color coding

#### **5. File to Update:**
- `superdesign/design_iterations/functional_pages/5-1-monitoring-interface.html`

**Labels:** design, frontend, telemetry, ui, high-priority  
**Timeline:** 1-2 ngày

---

## 📊 **TECHNICAL SPECIFICATIONS:**

### **🔧 Firmware Requirements:**
1. **Modbus Register Mapping** cho mỗi module type
2. **WebSocket streaming** với register data
3. **HTTP API endpoints** cho telemetry access
4. **Error handling** cho register read/write failures

### **💻 Backend Requirements:**
1. **API endpoints** cho telemetry data
2. **WebSocket integration** với firmware
3. **Mock service** với register simulation
4. **Schema definitions** cho telemetry data

### **🎨 Design Requirements:**
1. **Modal tab system** update
2. **Modbus-style table** implementation
3. **Real-time updates** integration
4. **Professional UI/UX** design

---

## 🔗 **INTEGRATION FLOW:**

```
Firmware RS485 Modules
    ↓ (Modbus RTU)
Firmware WebSocket Server (port 8081)
    ↓ (WS /ws/telemetry)
Backend RS485 Service
    ↓ (WS /ws/rs485)
Frontend Monitoring Interface
    ↓ (Modal Telemetry Tab)
User sees Modbus-style table
```

---

## ⏰ **TIMELINE TỔNG:**
- **Firmware:** 2-3 ngày
- **Backend:** 1-2 ngày  
- **Design:** 1-2 ngày
- **Integration & Testing:** 1 ngày
- **Total:** 5-8 ngày

## 🏷️ **PRIORITY:** High - Critical for monitoring functionality

---

**📝 NOTE:** Tạo các issues này trên GitHub repo để teams có thể track progress và coordinate implementation.
