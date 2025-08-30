"""
Đơn giản hoá dịch vụ lưu trữ Log/Audit bằng file JSON trong thư mục configs
"""

import json
from datetime import datetime
from pathlib import Path
from typing import Dict, List

from app.core.logging import get_logger


logger = get_logger(__name__)


class LogService:
    def __init__(self, config_dir: str = "configs"):
        self.dir = Path(config_dir)
        self.dir.mkdir(exist_ok=True)
        self.log_file = self.dir / "logs.json"
        self.audit_file = self.dir / "audit.json"
        if not self.log_file.exists():
            self._write(self.log_file, [])
        if not self.audit_file.exists():
            self._write(self.audit_file, [])

    def _read(self, file: Path) -> List[Dict]:
        try:
            with open(file, "r", encoding="utf-8") as f:
                data = json.load(f)
                return data if isinstance(data, list) else []
        except Exception:
            return []

    def _write(self, file: Path, data: List[Dict]) -> None:
        with open(file, "w", encoding="utf-8") as f:
            json.dump(data, f, indent=2, ensure_ascii=False)

    # Logs
    def history(self, limit: int = 200) -> List[Dict]:
        data = self._read(self.log_file)
        return list(reversed(data))[:limit]

    def add_log(self, level: str, source: str, message: str) -> Dict:
        rec = {
            "time": datetime.utcnow().isoformat(),
            "level": level,
            "source": source,
            "message": message,
        }
        data = self._read(self.log_file)
        data.append(rec)
        self._write(self.log_file, data)
        return rec

    # Audit
    def audit_history(self, limit: int = 200, user: str = "", action: str = "") -> List[Dict]:
        data = self._read(self.audit_file)
        items = list(reversed(data))
        if user:
            items = [x for x in items if x.get("user", "").lower() == user.lower()]
        if action:
            items = [x for x in items if x.get("action") == action]
        return items[:limit]

    def add_audit(self, **payload) -> Dict:
        rec = {
            "time": datetime.utcnow().isoformat(),
            "user": payload.get("user", "system"),
            "action": payload.get("action", "UNKNOWN"),
            "target": payload.get("target", "-"),
            "result": payload.get("result", "OK"),
            "ip": payload.get("ip", "127.0.0.1"),
            "session": payload.get("session", "S-1"),
            "device": payload.get("device", "OHT-50-01"),
            "correlation_id": payload.get("correlation_id", "c-0000"),
            "signature": payload.get("signature", "sha256:mock"),
        }
        data = self._read(self.audit_file)
        data.append(rec)
        self._write(self.audit_file, data)
        return rec


log_service = LogService()


