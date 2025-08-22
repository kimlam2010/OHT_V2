---
title: "Assignments Matrix"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "PM Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['project-management']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# ASSIGNMENTS MATRIX - OHT-50 Master Module

Phiên bản: v1.1  
Ngày cập nhật: 2025-01-28  
PM: Project Manager  
Trạng thái: 🎯 EXECUTION IN PROGRESS

---

## 📦 Module Assignments

Lưu ý: Địa chỉ RS485 mặc định theo bus 0x02–0x07; Owner đã được gán. Mục nào chưa có người, đề xuất Owner/ETA đi kèm.

### Power Module (0x02)
- Team: EMBED, FW, Backend, QA/V&V
- Owner: FW Module Lead (module_power); EMBED Lead (HW)
- Deliverables: Register map (V/I/T, alarms), commands, telemetry/health
- ETA: W4 integration; W5–W6 full features
- Dependencies: COMM, MODULE_MGMT, API, SAFETY
- Risks/Blockers: Chốt register map; HIL tải cao
- DoD: Read/write registers OK; telemetry ±2%; fault/alert hoạt động; E2E with Center
- Tests: Unit (codec), Integration (Modbus), HIL (load), Perf (latency)
- Status: ✅ **DONE** - `power_module_handler.c/h` implemented

### Motor Module (0x03)
- Team: FW, EMBED, QA/V&V
- Owner: FW Motion Lead (proposed)
- Deliverables: pos/vel/acc registers; enable/move/stop; faults
- ETA: W4 framework; W5–W6 motion features
- Dependencies: COMM, SAFETY, STATE_MACHINE
- Risks/Blockers: Interlock an toàn; dừng ±1–2mm
- DoD: Move profile hợp lệ; stop trong dung sai; faults đầy đủ
- Tests: Unit (planner), Integration (commands), HIL (latency)
- Status: ❌ **NOT STARTED** - Missing `motor_module_handler.c/h`

### IO Module (0x04)
- Team: FW, Backend, QA/V&V
- Owner: FW Module Lead (module_io) (proposed)
- Deliverables: DI/DO registers, batch ops, debounce
- ETA: W4 integration; W5 hoàn thiện
- Dependencies: COMM, MODULE_MGMT, API
- Risks/Blockers: Debounce timing; burst events
- DoD: Edge detect ổn định; batch write/read OK; health
- Tests: Unit (mask/edge), Integration (bulk), Perf (burst)
- Status: ❌ **NOT STARTED** - Missing `io_module_handler.c/h`

### Dock Module (0x05)
- Team: FW, Backend, QA/V&V
- Owner: FW Module Lead (module_dock) (proposed)
- Deliverables: dock/undock sequence, sensors, interlocks
- ETA: W5
- Dependencies: SAFETY, STATE_MACHINE, COMM
- Risks/Blockers: Cơ khí/interlock
- DoD: Sequence OK; interlock enforced; errors logged
- Tests: Integration (sequence), HIL (sensor), Latency
- Status: ❌ **NOT STARTED** - Missing `dock_module_handler.c/h`

### DI/DO Module (0x06)
- Team: FW, Backend, QA/V&V
- Owner: FW Module Lead (module_dido) (proposed)
- Deliverables: DI/DO mapping, edge events, health
- ETA: W5
- Dependencies: COMM, MODULE_MGMT
- Risks/Blockers: Event burst
- DoD: Mapping/edge/health hoàn chỉnh; registry cập nhật
- Tests: Unit (edge), Integration (events), Perf (burst)
- Status: ❌ **NOT STARTED** - Missing `dido_module_handler.c/h`

### AI Module (0x07)
- Team: FW, Backend, QA/V&V
- Owner: FW AI Lead (proposed)
- Deliverables: capability/heartbeat/stats; optional vision hooks
- ETA: W6
- Dependencies: COMM, MODULE_MGMT, API
- Risks/Blockers: Bandwidth/latency
- DoD: Capability query; telemetry; faults
- Tests: Integration (cap query), Perf (telemetry), Fault handling
- Status: ❌ **NOT STARTED** - Missing `ai_module_handler.c/h`

---

## 🔗 Cross-Cutting Assignments

| Hạng mục | Team | Owner | ETA | Dependencies | Definition of Done | Tests | Status |
|---|---|---|---|---|---|---|---|
| Module Management (discovery, registry, health) | FW | FW Module Lead | Done (W3) | COMM | Discovery 0x02–0x07; registry YAML; health monitor | Unit, Integration, E2E scan | ✅ Done |
| Communication (RS485/Modbus, failover, monitoring) | EMBED+FW | FW Comms Lead; EMBED Lead | Done (W2) | GPIO, Startup | RS485 115200 8N1; CRC; stats | Unit, HIL loopback, Perf | ✅ Done |
| API (REST/WS per API_SPEC) | Backend | Backend Lead | W5–W6 | MODULE_MGMT | SYSTEM/MODULE/SAFETY endpoints; JWT; docs | API unit/integration/load | 🚨 **BLOCKED** |
| Safety (E‑Stop single‑channel <100ms) | FW+Safety | FW Safety Lead | Done (W1) | GPIO | E‑Stop monitor; relay shutdown; logs | Latency <100ms; HIL | ✅ Done |
| State Machine (states/events/transitions) | FW | FW Core Lead | Done (W3) | SAFETY, MODULE_MGMT | Transition table; logging; validation | Unit, Integration | ✅ Done |
| Startup Sequence (services, init order) | FW+DevOps | DevOps Lead | W4 | COMM, API | Services order; health gates; boot OK | E2E boot; timing | In progress |
| GPIO Mapping (LED/RS485/E‑Stop/Relays) | EMBED | EMBED Lead | Done (W1) | — | Pins validated; docs updated | GPIO tests; HIL | ✅ Done |
| Register Map Validation (vs register_map_summary.md) | FW+QA | QA Lead (proposed) | W5 | MODULE specs | Cross-check all module registers | Static check; on-target probe | ❌ **BLOCKED** |
| QA/V&V Plan (coverage, fault injection, performance) | QA/V&V | QA Lead | W5 | All | Coverage ≥90%; fault/perf plans | Unit/integration/fault/perf | ❌ **BLOCKED** |
| Release/CI (build, test, artefact, rollback) | DevOps | DevOps Lead | W4 | Startup, Tests | CI build+test+artefact+rollback | Pipeline green; rollback drill | In progress |

---

## 🚨 **CRITICAL BLOCKERS**

### **Backend Team Blocked:**
- **BE-16 (FW/Center integration)**: Blocked until all module handlers implemented
- **API Development**: Cannot proceed without module handlers
- **Database Integration**: Waiting for module data structures

### **FW Team Critical Tasks:**
- **FW-29 (Motor Module)**: CRITICAL - Start immediately
- **FW-30 (IO Module)**: HIGH - Start immediately  
- **FW-31 (Dock Module)**: HIGH - Week 5
- **FW-32 (DI/DO Module)**: MEDIUM - Week 5
- **FW-33 (AI Module)**: MEDIUM - Week 6

### **QA/V&V Team Blocked:**
- **Register Map Validation**: Cannot validate until all modules implemented
- **QA/V&V Plan**: Cannot create comprehensive plan without module specs

---

## 🛡️ Responsibility Notes
- Addressing 0x02–0x07 + RS485 discovery: FW Module Lead (logic), EMBED Lead (bus HW).
- **NEW**: Module implementation responsibility: FW Motion Lead (Motor), FW Module Lead (IO/Dock/DI-DO), FW AI Lead (AI).

---

## 📊 **IMPACT ASSESSMENT**

### **Current System Capability:**
- **Module Support**: 17% (1/6 modules)
- **Backend Integration**: Blocked
- **Production Readiness**: 60%

### **Required Actions:**
1. **FW Team**: Implement Motor Module (FW-29) - CRITICAL
2. **FW Team**: Implement IO Module (FW-30) - HIGH
3. **FW Team**: Implement remaining modules (FW-31 to FW-33)
4. **Backend Team**: Wait for module handlers completion
5. **PM Team**: Monitor module implementation progress

---

**Báo cáo bởi:** Project Manager  
**Ngày cập nhật:** 2025-01-28  
**Phiên bản:** v1.1

**🎯 Status:** Critical module handlers missing - immediate action required!


