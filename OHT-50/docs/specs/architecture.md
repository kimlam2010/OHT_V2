# Kiến trúc tổng thể OHT-50 (mức hệ thống)

## Mục tiêu
- Tách lớp rõ: UI/Services/Control/HAL/Sim/Tests.
- Giao diện ổn định, có khả năng chạy giả lập trước khi tích hợp thiết bị thật.

## Sơ đồ kiến trúc
```mermaid
flowchart LR
  CENTER["Center (Coordinator)"]
  NET["Ethernet/Wi‑Fi"]
  UI["UI/Diagnostics"]
  SVC["Services (HTTP/WS)"]
  CTRL["Control Core\n(State Machine + Planner)"]
  SAFE["Safety\n(E-Stop, Limits, Watchdog)"]
  HAL["Drivers/HAL\n(RS485 Bus + Modules)"]
  BUS["RS485 Bus"]
  CPU["Radxa S0 (Master)"]
  DCM["DC Motor Drivers"]
  STM["Stepper Drivers"]
  DIO["Digital IO 8In/8Out"]
  AIO["Analog IO In/Out"]
  LOC["Location Module (RFID+Encoder)"]
  SMOD["Safety Module"]
  SIM["Sim 1D Rail"]

  CENTER <--> NET <--> SVC
  UI <--> SVC
  SVC <--> CTRL
  CTRL <--> SAFE
  CTRL -->|setpoints| HAL
  HAL -->|feedback| CTRL
  CTRL <--> SIM
  SVC -->|telemetry| UI

  CPU --- HAL
  HAL <--> BUS
  BUS <--> DCM
  BUS <--> STM
  BUS <--> DIO
  BUS <--> AIO
  BUS <--> LOC
  BUS <--> SMOD
```

## Cấu trúc cơ khí & trục điều khiển
```mermaid
flowchart TB
  U["Thân trên (Upper Body)"]
  C["Thùng Cargo (Lower Body)"]
  U --> AD["axis_drive (DC/Stepper + encoder)"]
  U --> AL["axis_lift (DC/Stepper + sensors)"]
  C --> DOOR["axis_door (DC + limit switches)"]
```

## Phân rã module
```mermaid
flowchart TB
  subgraph Services
    API["HTTP API"]
    WS["WS Telemetry"]
    CC["Center Client (WS/MQTT)"]
  end
  subgraph Control
    SM["State Machine"]
    PL["Planner S-curve (axis_drive)"]
    SEQ["Station/Docking Sequencer\n(door/lift)"]
    SF["Safety Hooks"]
  end
  subgraph HAL
    RS["RS485 Adapter"]
    REG["Module Registry"]
    MIF["Motor IF (DC/Stepper)"]
    IOIF["IO IF (DI/DO, AI/AO)"]
    LOCIF["Location IF (RFID+Encoder)"]
    SAFIF["Safety IF"]
  end
  SIM["Sim rail 1D"]

  API --> SM
  WS --> SM
  CC --> SM
  SM --> PL
  SM --> SEQ
  PL --> SF
  SEQ --> SF
  SM --> HAL
  HAL --> SM
  SM <--> SIM
```

## Luồng dữ liệu tóm tắt
- Center giao nhiệm vụ tới Services → `State Machine` → `Planner/Sequencer` → HAL.
- HAL qua RS485 điều khiển các trục/IO và thu nhận phản hồi (định vị RFID+encoder, encoder, IO); `Control` hợp nhất và phát telemetry tới UI/Center.
- `Safety` giám sát tốc độ/vùng cấm/E‑Stop và liên động cửa/nâng.

## Tham số hệ thống (TBD)
- `m_total`, `r`, ma sát/hiệu suất, dốc tối đa.
- `v_max`, `a_max`, `j_max`, chiều dài ray, dung sai dừng.
- Giới hạn trục phụ: tốc độ/nấc nâng, thời gian mở/đóng cửa.
- RS485: baudrate, địa chỉ, retry/timeout, termination.
- Kênh Center: WS/MQTT, heartbeat/permission/mission.
