# feat: Add Orange Pi 5B platform setup and documentation

## Orange Pi 5B Platform Setup

### Platform Decision
- **Board**: Orange Pi 5B (RK3588, linux 6.1.43-rk3588)
- **RS485**: UART1 (`/dev/ttyS1` → udev symlink `/dev/ttyOHT485`)
- **Relay Control**: GPIO1_D3 (Relay1), GPIO1_D2 (Relay2)

### New Documentation
- `docs/dev_radxa/platform_orangepi_5b.md` - Platform spec with UART1/GPIO mapping
- `docs/dev_radxa/udev_rules_orangepi5b.md` - udev rules guide for `/dev/ttyOHT485`
- Updated `docs/rule.mdc` with detailed EMBED/FW role definition + flowcharts

### New Code Skeleton
- `firmware/hal/hal_relay.h/.c` - HAL relay API for GPIO1_D3/D2
- `firmware/tools/relayctl.c` - CLI demo tool
- `tools/rs485/rs485_loop_tester.py` - RS485 testing tool

### Task Assignment
- **FW-14**: HAL relay_set(channel, on) API - Người A (due 2025-08-22)
- **FW-15**: RS485 device config with fallback - Người A (due 2025-08-20)  
- **EM-10**: Confirm GPIO1_D3/D2 offsets - Người B (due 2025-08-18)
- **EM-11**: Enable UART1, verify /dev/ttyS1 - Người B (due 2025-08-18)

### Updated Files
- `docs/tasks/PM_TASKS_OHT-50.md` - Added FW-14/15, EM-10/11 tasks
- `docs/specs/hardware.md` - Orange Pi 5B platform reference
- `docs/specs/config_spec.md` - UART1 device mapping
- `docs/specs/EMBED_TEST_CHECKLIST.md` - Added relay GPIO testing

### Next Steps
1. Admin review and approve
2. Replace 'Người A/B' with actual assignees
3. EM team to confirm GPIO offsets on actual hardware
4. FW team to implement libgpiod integration

**Files changed**: 14 files, 662 insertions(+), 63 deletions(-)
