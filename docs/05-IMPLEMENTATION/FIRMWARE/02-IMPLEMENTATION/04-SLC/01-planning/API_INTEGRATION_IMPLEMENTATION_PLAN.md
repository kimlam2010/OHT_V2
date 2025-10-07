#️⃣ Phần dưới giữ lại (ngắn gọn, thực thi ngay)
#
**Phiên bản:** 2.0.0  
**Ngày cập nhật:** 2025-01-28  
**Team:** FW  
**Ưu tiên:** CRITICAL

## 🎯 Mục tiêu & Phạm vi (rút gọn)
- HTTP REST API + WS theo kiến trúc dự án (FW làm gateway HTTP, không RS485 trực tiếp từ BE)
- Wiring trực tiếp vào `system_controller`, `safety_monitor`, `control_loop`, `system_state_machine`
- Không dùng mock trong production paths; đáp ứng p95 < 100ms

## 🧭 Endpoint Matrix (P0/P1/P2)
## 🧭 Endpoint Matrix (P0/P1/P2) – bám sát core hiện tại

### P0 (ship ngay)
- `GET /api/v1/system/status` — map `system_controller_get_status`
- `GET /api/v1/safety/status` — map `safety_monitor_get_status`
- `POST /api/v1/safety/estop` — map `safety_monitor_trigger_emergency_stop`
- `GET /api/v1/modules` — map `module_manager_list`
- `GET /api/v1/modules/{id}/status` — map `module_manager_get_status_by_id`
- `GET /api/v1/system/state` — map `system_state_machine_get_status`
- `GET /api/v1/control/status` — map `control_loop_get_status` + `control_loop_get_stats`

### P1 (tăng giá trị vận hành)
- `GET /api/v1/system/health` — map `system_controller_get_performance` + counters
- `GET /api/v1/telemetry/current` — snapshot (system/safety/control)
- `POST /api/v1/safety/reset` — map `safety_monitor_reset`
- `GET /api/v1/safety/zones` — map `safety_monitor_get_basic_zones`
- `POST /api/v1/control/mode` — map `control_loop_set_mode`
- `POST /api/v1/control/target` — map `control_loop_set_target_position` / `control_loop_set_target_velocity`

### P2 (module chuyên sâu + chẩn đoán + WS)
- `GET /api/v1/modules/power/status` — điện/battery/charge (theo REQ_POWER_MODULE)
- `GET /api/v1/modules/motor/status` — mode, pos/vel/fault
- `GET /api/v1/modules/dock/status` — align/ready/charging
- `GET /api/v1/diagnostics/summary` — tổng hợp lỗi/counters
- `GET /api/v1/diagnostics/safety` — map `safety_monitor_run_diagnostics`
- `GET /api/v1/diagnostics/state` — map `system_state_machine_get_diagnostics`
- `GET /api/v1/diagnostics/control` — map `control_loop_get_diagnostics`
- `WS /ws/telemetry` — broadcast `system.status` mỗi 1s

Lý do: wiring trực tiếp vào `system_controller`, `safety_monitor`, `control_loop`, `system_state_machine` giúp triển khai nhanh, không chạm RS485 từ backend, đáp ứng mục tiêu Performance/Safety (<50–100ms) và mở rộng tuần 2 cho perf/WS/security nhẹ.

---

## 📊 Progress Tracker (FW team)

| Endpoint | Priority | Owner | Status | Due | Notes |
|---|---|---|---|---|---|
| GET /api/v1/system/status | P0 | FW-Dev#1 | Planned | W1-D3 | Map `system_controller_get_status` |
| GET /api/v1/safety/status | P0 | FW-Dev#2 | Planned | W1-D3 | E‑Stop, zones, watchdog |
| POST /api/v1/safety/estop | P0 | FW-Dev#2 | Planned | W1-D2 | Guarded emergency |
| GET /api/v1/modules | P0 | FW-Dev#3 | Planned | W1-D4 | Discovery list (real) |
| GET /api/v1/modules/{id}/status | P0 | FW-Dev#3 | Planned | W1-D4 | ID parse + status |
| GET /api/v1/system/state | P0 | FW-Dev#1 | Planned | W1-D3 | State machine |
| GET /api/v1/control/status | P0 | FW-Dev#4 | Planned | W1-D5 | pos/vel/error/output |
| GET /api/v1/system/health | P1 | FW-Dev#1 | Pending | W2-D2 | perf + counters |
| GET /api/v1/telemetry/current | P1 | FW-Dev#5 | Pending | W2-D2 | snapshot serializer |
| POST /api/v1/safety/reset | P1 | FW-Dev#2 | Pending | W2-D1 | require HW released |
| GET /api/v1/safety/zones | P1 | FW-Dev#2 | Pending | W2-D1 | thresholds export |
| POST /api/v1/control/mode | P1 | FW-Dev#4 | Pending | W2-D2 | guard transitions |
| POST /api/v1/control/target | P1 | FW-Dev#4 | Pending | W2-D3 | limits/validation |
| modules power/motor/dock | P2 | FW-Dev#3 | Backlog | W3-W4 | per-module handlers |
| diagnostics/* | P2 | FW-Dev#1 | Backlog | W3-W4 | summarize + details |
| WS /ws/telemetry | P2 | FW-Dev#5 | Backlog | W3 | 1s broadcast system.status |

Ghi chú vận hành:
- P0 phải có dữ liệu thật (no mock). P1 bổ sung health và control setting có guard. P2 mở rộng module/diag và WS cho realtime UI.

---

## ⚡ Minimal API v1 - 2-week execution (rút gọn)

### 🎯 Goal (scope minimized, production-ready core)
- REST (minimal):
  - GET `/api/v1/system/status`
  - GET `/api/v1/safety/status`, POST `/api/v1/safety/estop`
  - GET `/api/v1/modules`, GET `/api/v1/modules/{id}/status`
- WebSocket (optional in Week 2): `/ws/telemetry` broadcasting `system.status` only
- Non-goals (postpone to v2): advanced config, diagnostics, module-specific deep endpoints, complex auth.

### 📅 Week 1
- Build & Tests
  - [ ] Fix linker issues (CMake toolchain, libs) – build green
  - [ ] Enable unit + basic integration tests to run
  - [ ] Remove all mock usage in production paths (mocks only under tests)
- API Surface (HTTP only)
  - [ ] Implement 4 minimal endpoints above in `api_manager`
  - [ ] Wire to services: `system_controller`, `safety_monitor`, `module_manager`
  - [ ] Input validation (basic), JSON responses unified
- Ops
  - [ ] Health check: confirm :8080 listening, endpoints <100ms p95 (local)
  - [ ] Logging: request line + status, errors

Deliverables
- [ ] Minimal API v1 spec (this section) locked
- [ ] Short ADR: API Surface v1 (Minimal) + Gateway layering

Acceptance (exit Week 1)
- [ ] Build green; unit/integration smoke pass
- [ ] 4 endpoints return real data (no mock)
- [ ] p95 GET latency < 100ms (bench local)

### 📅 Week 2
- Reliability & Perf
  - [ ] Error handling paths completed (timeouts, not-found, invalid)
  - [ ] Basic rate-limit or debounce where needed (lightweight)
  - [ ] Perf log sampling for slow requests (>100ms)
- WebSocket (optional, if Week 1 stable)
  - [ ] `/ws/telemetry` broadcasting `system.status` every 1s
  - [ ] Client mgmt + backpressure-safe send
- Security (minimal)
  - [ ] Simple token header gate (env-config), dev default disabled
  - [ ] Sanitize/limit request body sizes

Deliverables
- [ ] Bench doc: p95 latency HTTP, WS latency if enabled (<50ms local)
- [ ] Update runbook: start/stop, ports, health-check curl, wscat

Acceptance (exit Week 2)
- [ ] All Week 1 criteria sustained
- [ ] Errors well-formed JSON; no crash under 1h soak
- [ ] Optional: WS stable with 2 clients for 30m

### ⏱️ Daily tracking checklist
- Build & tests
  - [ ] Build green today
  - [ ] Unit tests executed
  - [ ] Integration smoke executed
- API health
  - [ ] /system/status <100ms p95
  - [ ] /safety/status <100ms p95
  - [ ] /modules <100ms p95
  - [ ] /modules/{id}/status <100ms p95
- Quality
  - [ ] No mock in prod paths
  - [ ] Error logs < threshold; no unhandled exceptions

### 🔗 File/code touch points
- `src/app/api/api_manager.c` – register only minimal endpoints, unify responses
- `src/app/managers/{module_manager, safety_manager, telemetry_manager}.c` – service calls
- `src/app/core/{system_controller, safety_monitor}.c` – read-only status hooks
- `tests/` – enable unit/integration smoke for endpoints, safety
- `docs/` – ADR + runbook + bench notes

### 🧪 Validation commands
```bash
curl -s http://localhost:8080/api/v1/system/status | jq .
curl -s http://localhost:8080/api/v1/safety/status | jq .
curl -s http://localhost:8080/api/v1/modules | jq .
curl -s http://localhost:8080/api/v1/modules/1/status | jq .
# optional WS
wscat -c ws://localhost:8081/ws/telemetry
```

### 📌 Risks & guards (for this fast-track)
- Scope creep → hard-freeze to 4 endpoints + 1 WS topic
- Perf regressions → track p95 daily, log >100ms
- Hidden mocks → CI gate to disallow mock symbols in prod build

---

**🚨 Lưu ý:** Plan này phải được thực hiện tuần tự, không được bỏ qua phase nào. Mỗi phase phải hoàn thành 100% trước khi chuyển sang phase tiếp theo.

**🔗 PROJECT INTEGRATION:** Đã align với Project SLC Planning. Scope giữ tối thiểu để ship nhanh.

---

**Changelog v2.0.0 (rút gọn):**
- ✅ Thêm Endpoint Matrix P0/P1/P2 + Progress Tracker
- ✅ Khóa Minimal API v1 (2 tuần) + checklist/commands
- ✅ Lược bỏ mô tả phase dài, risk verbose, timeline chi tiết
