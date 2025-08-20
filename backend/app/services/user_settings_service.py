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

    # Public API expected by existing routes
    def get_user_settings(self, username: str) -> Optional[UserSettings]:
        data = self._store.get(username)
        if not data:
            return None
        try:
            return UserSettings(**data)
        except Exception as e:
            logger.error("Invalid user settings payload", error=str(e), username=username)
            return None

    def update_user_settings(self, settings: UserSettings) -> UserSettings:
        self._store[settings.username] = settings.model_dump()
        self._save()
        logger.info("User settings saved", username=settings.username)
        return settings

    # Newer convenience API retained internally
    def get(self, username: str) -> UserSettings:
        if not username:
            username = "operator"
        settings = self.get_user_settings(username)
        if settings is not None:
            return settings
        default = UserSettings(
            username=username,
            display_name=username,
            email=f"{username}@example.com",
            theme="light",
        )
        self._store[username] = default.model_dump()
        self._save()
        return default

    def update(self, username: str, update_data: Dict) -> UserSettings:
        current = self.get(username)
        merged = current.model_dump()
        for k, v in update_data.items():
            if isinstance(v, dict) and isinstance(merged.get(k), dict):
                merged[k].update(v)
            else:
                merged[k] = v
        merged["username"] = username
        updated = UserSettings(**merged)
        return self.update_user_settings(updated)

