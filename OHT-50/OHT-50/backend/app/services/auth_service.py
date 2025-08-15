"""
Auth service tối giản: lưu user trong JSON, bcrypt hash, phát token giả lập
"""

import json
import secrets
from pathlib import Path
from typing import Dict

from passlib.hash import bcrypt

from app.core.logging import get_logger


logger = get_logger(__name__)


class AuthService:
    def __init__(self, config_dir: str = "configs"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
        self.users_file = self.config_dir / "users.json"
        self._users: Dict[str, Dict] = {}
        self._load()
        self._ensure_default_admin()

    @staticmethod
    def hash_password(password: str) -> str:
        return bcrypt.hash(password)

    def _load(self) -> None:
        if self.users_file.exists():
            try:
                with open(self.users_file, "r", encoding="utf-8") as f:
                    data = json.load(f)
                    if isinstance(data, dict):
                        self._users = data
                    else:
                        self._users = {}
                logger.info("Loaded users store", entries=len(self._users))
            except Exception as e:
                logger.error("Failed to load users store", error=str(e))
                self._users = {}
        else:
            self._users = {}
            self._save()

    def _save(self) -> None:
        try:
            with open(self.users_file, "w", encoding="utf-8") as f:
                json.dump(self._users, f, indent=2, ensure_ascii=False)
            logger.info("Saved users store", entries=len(self._users))
        except Exception as e:
            logger.error("Failed to save users store", error=str(e))

    def _ensure_default_admin(self) -> None:
        if "admin" not in self._users:
            pwd_hash = bcrypt.hash("admin")
            self._users["admin"] = {"username": "admin", "password_hash": pwd_hash, "role": "admin"}
            self._save()
            logger.info("Created default admin account")

    def verify_user(self, username: str, password: str) -> bool:
        rec = self._users.get(username)
        if not rec:
            return False
        return bcrypt.verify(password, rec.get("password_hash", ""))

    def issue_token(self, username: str) -> str:
        # Token giả lập (random hex); có thể nâng cấp sang JWT sau
        token = secrets.token_hex(16)
        return token

    # Admin helpers
    def list_users(self):
        return [{"username": u, "role": v.get("role", "operator"), "last": v.get("last") } for u, v in self._users.items()]

    def add_user(self, username: str, password: str, role: str = "operator"):
        if username in self._users:
            raise ValueError("User exists")
        self._users[username] = {
            "username": username,
            "password_hash": self.hash_password(password),
            "role": role,
        }
        self._save()

    def set_password(self, username: str, password: str):
        if username not in self._users:
            raise ValueError("User not found")
        self._users[username]["password_hash"] = self.hash_password(password)
        self._save()

    def delete_user(self, username: str):
        if username in self._users:
            del self._users[username]
            self._save()

