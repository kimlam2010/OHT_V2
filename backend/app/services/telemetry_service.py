"""
Mock Telemetry service: generate synthetic telemetry for development.
"""
from __future__ import annotations

import math
import time
from collections import deque
from dataclasses import dataclass
from typing import Deque, List, Tuple

from app.models.telemetry import (
    TelemetryData,
    TelemetryEncoder,
    TelemetrySafety,
    TelemetryStatus,
    TelemetryTarget,
)


@dataclass
class TelemetryState:
    pos_mm: float = 0.0
    vel_mms: float = 0.0
    acc_mms2: float = 0.0
    state: str = "Idle"


class TelemetryService:
    def __init__(self, history_size: int = 1000) -> None:
        self._history: Deque[TelemetryData] = deque(maxlen=history_size)
        self._t = 0.0
        self._state = TelemetryState()
        self._hz: float = 10.0

    def tick(self, dt: float = 0.1) -> TelemetryData:
        self._t += dt
        # Simple harmonic motion mock
        self._state.vel_mms = 200 * math.sin(self._t)
        self._state.pos_mm += self._state.vel_mms * dt
        self._state.acc_mms2 = 200 * math.cos(self._t)
        self._state.state = "Move" if abs(self._state.vel_mms) > 5 else "Idle"

        data = TelemetryData(
            ts=time.time(),
            status=TelemetryStatus(
                state=self._state.state,
                pos_mm=self._state.pos_mm,
                vel_mms=self._state.vel_mms,
                acc_mms2=self._state.acc_mms2,
                target=TelemetryTarget(pos_mm=0.0, vel_mms=0.0),
                safety=TelemetrySafety(
                    estop=False, zone_blocked=False, interlock_active=False
                ),
                enc=TelemetryEncoder(health="ok", count=int(self._state.pos_mm * 10)),
                errors=[],
                warn=[],
            ),
            mission_id=None,
            station_id=None,
            cargo_present=False,
            cargo_locked=False,
            cpu_usage=None,
            memory_usage=None,
            temperature=None,
            center_connected=True,
            rs485_connected=True,
        )
        self._history.append(data)
        return data

    def current(self) -> TelemetryData:
        if self._history:
            return self._history[-1]
        return self.tick()

    def history(self, limit: int = 100) -> List[TelemetryData]:
        return list(self._history)[-limit:]

    # Runtime controls
    def set_rate(self, hz: float) -> None:
        if hz <= 0 or hz > 50:
            raise ValueError("hz must be in (0, 50]")
        self._hz = float(hz)

    def get_rate(self) -> float:
        return self._hz

    def reset(self) -> None:
        self._history.clear()
        self._t = 0.0
        self._state = TelemetryState()

    def stats(self) -> dict:
        count = len(self._history)
        if count == 0:
            return {"count": 0, "last_ts": None, "vel_min": None, "vel_max": None, "vel_avg": None}
        velocities = [item.status.vel_mms for item in self._history]
        return {
            "count": count,
            "last_ts": self._history[-1].ts,
            "vel_min": min(velocities),
            "vel_max": max(velocities),
            "vel_avg": sum(velocities) / count,
        }


telemetry_service = TelemetryService()
