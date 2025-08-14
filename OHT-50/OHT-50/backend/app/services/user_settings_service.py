import json
from pathlib import Path
from typing import Optional

from app.core.logging import get_logger
from app.models.user import UserSettings


logger = get_logger(__name__)


class UserSettingsService:
	"""Lưu/đọc User Settings đơn giản từ file JSON."""

	def __init__(self, config_dir: str = "configs"):
		self.config_dir = Path(config_dir)
		self.config_dir.mkdir(exist_ok=True)
		self.file_path = self.config_dir / "user_settings.json"
		if not self.file_path.exists():
			self._write_all({})

	def _read_all(self) -> dict:
		try:
			with open(self.file_path, "r", encoding="utf-8") as f:
				return json.load(f)
		except Exception:
			return {}

	def _write_all(self, data: dict) -> None:
		with open(self.file_path, "w", encoding="utf-8") as f:
			json.dump(data, f, indent=2, ensure_ascii=False)

	def get_user_settings(self, username: str) -> Optional[UserSettings]:
		data = self._read_all()
		raw = data.get(username)
		if not raw:
			return None
		try:
			return UserSettings(**raw)
		except Exception as e:
			logger.error("Invalid user settings payload", error=str(e), username=username)
			return None

	def update_user_settings(self, settings: UserSettings) -> UserSettings:
		data = self._read_all()
		data[settings.username] = settings.dict()
		self._write_all(data)
		logger.info("User settings saved", username=settings.username)
		return settings

"""
User Settings Service
Quản lý cài đặt người dùng lưu dưới dạng JSON (đóng vai trò database tối giản)
"""

import json
from pathlib import Path
from typing import Dict, Optional

from app.core.logging import get_logger
from app.models.user import UserSettings


logger = get_logger(__name__)


class UserSettingsService:
    """Service quản lý cài đặt người dùng đơn giản bằng file JSON"""

    def __init__(self, config_dir: str = "configs"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
        self.store_file = self.config_dir / "user_settings.json"
        self._store: Dict[str, Dict] = {}
        self._load()

    def _load(self) -> None:
        if self.store_file.exists():
            try:
                with open(self.store_file, "r", encoding="utf-8") as f:
                    data = json.load(f)
                    if isinstance(data, dict):
                        self._store = data
                    else:
                        self._store = {}
                logger.info("Loaded user settings store", entries=len(self._store))
            except Exception as e:
                logger.error("Failed to load user settings store", error=str(e))
                self._store = {}
        else:
            self._store = {}
            self._save()

    def _save(self) -> None:
        try:
            with open(self.store_file, "w", encoding="utf-8") as f:
                json.dump(self._store, f, indent=2, ensure_ascii=False)
            logger.info("Saved user settings store", entries=len(self._store))
        except Exception as e:
            logger.error("Failed to save user settings store", error=str(e))

    def get(self, username: str) -> UserSettings:
        """Lấy cài đặt theo username, nếu chưa có trả về mặc định"""
        if not username:
            username = "operator"
        data = self._store.get(username)
        if not data:
            # default settings
            settings = UserSettings(
                username=username,
                display_name=username,
                email=f"{username}@example.com",
                theme="light",
            )
            self._store[username] = settings.model_dump()
            self._save()
            return settings
        return UserSettings(**data)

    def update(self, username: str, update_data: Dict) -> UserSettings:
        """Cập nhật (partial) cài đặt người dùng và lưu file"""
        current = self.get(username)
        merged = current.model_dump()
        # hợp nhất đệ quy đơn giản cho level 1
        for k, v in update_data.items():
            if isinstance(v, dict) and isinstance(merged.get(k), dict):
                merged[k].update(v)
            else:
                merged[k] = v
        merged["username"] = username
        self._store[username] = merged
        self._save()
        return UserSettings(**merged)



