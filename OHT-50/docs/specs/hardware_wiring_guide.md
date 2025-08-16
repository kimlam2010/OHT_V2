# Hardware Wiring Guide - RS485 cho OHT-50 (v1.0)

## 🎯 **Mục tiêu**
Kết nối RS485 transceiver với Orange Pi 5B UART1 để giao tiếp với các module OHT-50.

## 📋 **Components cần thiết**

### 1. **RS485 Transceiver Module**
- **Model**: MAX485 hoặc SN75176B
- **Voltage**: 3.3V hoặc 5V (tương thích với Orange Pi 5B)
- **Speed**: Hỗ trợ 115200 baud
- **Package**: DIP-8 hoặc SOIC-8

### 2. **Resistors**
- **Termination**: 120Ω (2 cái) - hai đầu bus
- **Bias**: 680Ω-10kΩ (2 cái) - nếu cần
- **Pull-up/Pull-down**: 4.7kΩ (2 cái) - cho DE/RE

### 3. **Connectors**
- **26-pin header**: Kết nối với Orange Pi 5B
- **Screw terminal**: Kết nối với module OHT-50
- **Jumper wires**: Kết nối các thành phần

## 🔌 **Wiring Diagram**

```
Orange Pi 5B (26-pin header)     RS485 Transceiver     Module OHT-50
┌─────────────────┐              ┌─────────────┐       ┌─────────────┐
│                 │              │             │       │             │
│ Pin 5 (TX) ────┼──────────────┼─ TX         │       │             │
│                 │              │             │       │             │
│ Pin 3 (RX) ────┼──────────────┼─ RX         │       │             │
│                 │              │             │       │             │
│ GND ───────────┼──────────────┼─ GND        │       │             │
│                 │              │             │       │             │
│ Pin 3 (GPIO47) ─┼──────────────┼─ DE/RE      │       │             │
│                 │              │             │       │             │
│                 │              │ A ──────────┼───────┼─ A          │
│                 │              │             │       │             │
│                 │              │ B ──────────┼───────┼─ B          │
│                 │              │             │       │             │
│                 │              │ VCC ────────┼─ 3.3V │             │
└─────────────────┘              └─────────────┘       └─────────────┘
                                        │
                                   ┌────┴────┐
                                   │ 120Ω    │
                                   │ Resistor│
                                   └─────────┘
```

## 🔧 **Chi tiết kết nối (CORRECTED)**

### 1. **UART1 Connections (NOT USED)**
```
Orange Pi 5B Pin → RS485 Transceiver
├─ Pin 5 (TX) ────→ NOT USED
├─ Pin 3 (RX) ────→ NOT USED
└─ GND ───────────→ GND (Pin 5)
```

### 2. **GPIO Control (Manual Communication)**
```
Orange Pi 5B Pin → RS485 Transceiver
├─ GPIO 46 (wPi 1) ─→ TX (Pin 4)  # Output - Send data
└─ GPIO 47 (wPi 0) ─→ RX (Pin 1)  # Input - Read data
```

### 3. **Power Supply**
```
Orange Pi 5B → RS485 Transceiver
├─ 3.3V ─────────→ VCC (Pin 8)
└─ GND ───────────→ GND (Pin 5)
```

### 4. **RS485 Bus**
```
RS485 Transceiver → Module OHT-50
├─ A (Pin 6) ─────→ A (RS485+)
└─ B (Pin 7) ─────→ B (RS485-)
```

## ⚡ **Termination Setup**

### 1. **End-of-line Termination**
```
Module A ────┬─── Module B ────┬─── Module C
             │                 │
          [120Ω]           [120Ω]
             │                 │
           GND               GND
```

### 2. **Bias Resistors (Optional)**
```
RS485 Bus
    │
    ├─ A ────[680Ω]─── 3.3V
    │
    └─ B ────[680Ω]─── GND
```

## 🧪 **Test Setup (CORRECTED)**

### 1. **Manual Communication Test**
```
Orange Pi 5B GPIO ──→ RS485 Transceiver ──→ Manual TX/RX
├─ GPIO 46 (TX) ──→ TX (Pin 4)
└─ GPIO 47 (RX) ──→ RX (Pin 1)
```

### 2. **Module Test**
```
Orange Pi 5B GPIO ──→ RS485 Transceiver ──→ Module OHT-50
├─ GPIO 47 (TX) ──→ TX (Pin 4) ──→ Module
└─ GPIO 46 (RX) ──→ RX (Pin 1) ──→ Module
```

## 📏 **Physical Layout**

### 1. **Breadboard Setup (CORRECTED)**
```
┌─────────────────────────────────────────┐
│                                         │
│  Orange Pi 5B    RS485 Transceiver      │
│  ┌─────────┐     ┌─────────────┐        │
│  │ GPIO46  │─────│ TX (Pin 4)  │        │
│  │ GPIO47  │─────│ RX (Pin 1)  │        │
│  │ GND     │─────│ GND (Pin 5) │        │
│  │ 3.3V    │─────│ VCC (Pin 8) │        │
│  └─────────┘     └─────────────┘        │
│                    │     │              │
│                    │     │              │
│                 [120Ω] [120Ω]           │
│                    │     │              │
│                    │     │              │
│                 A ─┴─────┴─ B           │
│                                         │
└─────────────────────────────────────────┘
```

### 2. **PCB Layout Considerations**
- **Signal traces**: Ngắn và đối xứng
- **Ground plane**: Tách biệt digital/analog
- **Termination**: Gần module cuối
- **EMI protection**: TVS diodes nếu cần

## 🔍 **Verification Steps**

### 1. **Continuity Test**
```bash
# Test kết nối với multimeter
# TX → TX: Continuity
# RX → RX: Continuity  
# GND → GND: Continuity
# DE → GPIO47: Continuity
```

### 2. **Voltage Test**
```bash
# Test voltage levels
# VCC: 3.3V ±5%
# A-B differential: 0V (idle), ±2V (active)
# DE/RE: 0V (receive), 3.3V (transmit)
```

### 3. **Signal Test**
```bash
# Test với oscilloscope/logic analyzer
# TX signal: 115200 baud, 8N1
# DE signal: Synchronized with TX
# A-B differential: Clean signal
```

## 🚨 **Safety Considerations**

### 1. **Power Supply**
- **Voltage**: Không vượt quá 3.3V cho logic
- **Current**: Đảm bảo đủ current cho transceiver
- **Ground**: Common ground giữa tất cả devices

### 2. **ESD Protection**
- **TVS diodes**: Bảo vệ A/B lines
- **Grounding**: Proper grounding cho shield
- **Handling**: ESD-safe khi thao tác

### 3. **EMI Considerations**
- **Shielded cable**: Cho RS485 bus
- **Termination**: Đúng impedance
- **Routing**: Tránh noise sources

## 📋 **Checklist**

### Pre-wiring
- [ ] Components đã chuẩn bị đầy đủ
- [ ] Multimeter/oscilloscope sẵn sàng
- [ ] Work area clean và organized
- [ ] Safety equipment (ESD strap)

### Wiring
- [ ] UART1 connections (TX, RX, GND)
- [ ] DE/RE control (GPIO47)
- [ ] Power supply (3.3V, GND)
- [ ] RS485 bus (A, B)
- [ ] Termination resistors (120Ω)

### Testing
- [ ] Continuity test pass
- [ ] Voltage levels correct
- [ ] Signal quality acceptable
- [ ] Loopback test pass
- [ ] Module communication test

## 🔧 **Troubleshooting**

### Common Issues
1. **No communication**: Kiểm tra wiring, termination
2. **Signal noise**: Kiểm tra grounding, shielding
3. **Wrong baud rate**: Verify 115200/8N1
4. **DE/RE timing**: Check GPIO control timing

### Debug Commands
```bash
# Test UART1
sudo python3 quick_test.py

# Test GPIO control
gpio write 0 1; sleep 0.1; gpio write 0 0

# Monitor serial
sudo python3 -c "
import serial
with serial.Serial('/dev/ttyS1', 115200) as ser:
    while True:
        data = ser.read(1)
        if data:
            print(f'RX: {data.hex()}')
"
```

---

**Status**: 📋 Hardware wiring guide ready
**Next**: ⚡ Termination setup và production test
