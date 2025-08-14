"""
Configuration Service
Quản lý cấu hình hệ thống OHT-50 theo config_spec.md
"""

import json
import yaml
from datetime import datetime
from typing import Dict, List, Optional, Any
from pathlib import Path

from app.core.logging import get_logger
from app.core.exceptions import ConfigurationError, ValidationError
from app.models.config import SystemConfig, ConfigHistory, ConfigResponse

logger = get_logger(__name__)


class ConfigService:
    """Service quản lý cấu hình hệ thống"""
    
    def __init__(self, config_dir: str = "configs"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
        self.current_config: Optional[SystemConfig] = None
        self.config_history: List[ConfigHistory] = []
        
        # Load config hiện tại nếu có
        self._load_current_config()
    
    def _load_current_config(self) -> None:
        """Load cấu hình hiện tại từ file"""
        current_file = self.config_dir / "current.json"
        if current_file.exists():
            try:
                with open(current_file, 'r', encoding='utf-8') as f:
                    data = json.load(f)
                    self.current_config = SystemConfig(**data)
                logger.info("Loaded current configuration", config_version=self.current_config.version)
            except Exception as e:
                logger.error("Failed to load current config", error=str(e))
                self.current_config = self._get_default_config()
        else:
            self.current_config = self._get_default_config()
            self._save_current_config()
    
    def _get_default_config(self) -> SystemConfig:
        """Tạo cấu hình mặc định"""
        return SystemConfig(
            version=1,
            hardware={
                "motor": {
                    "interface": "rs485",
                    "r": 0.05,
                    "vmax_mms": 500.0,
                    "amax_mms2": 300.0,
                    "jmax_mms3": 5000.0
                },
                "encoder": {
                    "type": "incremental",
                    "resolution_ppr": 2048,
                    "invert": False
                }
            },
            safety={
                "zones": [
                    {"start_mm": 0, "end_mm": 200, "vmax_mms": 200}
                ],
                "estop_reset_policy": "manual"
            },
            telemetry={
                "ws_hz": 10
            },
            comm={
                "rs485": {
                    "device": "/dev/ttyOHT485",
                    "baud": 115200
                },
                "can": {
                    "iface": "can0"
                }
            }
        )
    
    def _save_current_config(self) -> None:
        """Lưu cấu hình hiện tại"""
        if self.current_config:
            current_file = self.config_dir / "current.json"
            with open(current_file, 'w', encoding='utf-8') as f:
                json.dump(self.current_config.dict(), f, indent=2, ensure_ascii=False)
            logger.info("Saved current configuration", config_version=self.current_config.version)
    
    def _save_config_history(self, config: SystemConfig, action: str, user: str = "system") -> None:
        """Lưu lịch sử cấu hình"""
        history_entry = ConfigHistory(
            version=config.version,
            timestamp=datetime.now(),
            action=action,
            user=user,
            config_data=config.dict()
        )
        
        # Lưu vào file history
        history_file = self.config_dir / f"history_v{config.version}.json"
        with open(history_file, 'w', encoding='utf-8') as f:
            json.dump(history_entry.dict(), f, indent=2, ensure_ascii=False)
        
        # Thêm vào memory history (giữ 10 bản gần nhất)
        self.config_history.append(history_entry)
        if len(self.config_history) > 10:
            self.config_history.pop(0)
        
        logger.info("Saved config history", version=config.version, action=action)
    
    def _validate_config(self, config: SystemConfig) -> None:
        """Validate cấu hình theo rules"""
        # Validate motor config
        motor = config.hardware.get("motor", {})
        if motor.get("vmax_mms", 0) <= 0:
            raise ValidationError("vmax_mms must be greater than 0")
        if motor.get("amax_mms2", 0) <= 0:
            raise ValidationError("amax_mms2 must be greater than 0")
        if motor.get("jmax_mms3", 0) <= 0:
            raise ValidationError("jmax_mms3 must be greater than 0")
        
        # Validate interface consistency
        interface = motor.get("interface", "rs485")
        if interface == "rs485" and "rs485" not in config.comm:
            raise ValidationError("RS485 interface requires rs485 communication config")
        elif interface == "can" and "can" not in config.comm:
            raise ValidationError("CAN interface requires can communication config")
        
        # Validate safety zones
        zones = config.safety.get("zones", [])
        for zone in zones:
            if zone.get("start_mm", 0) >= zone.get("end_mm", 0):
                raise ValidationError("Zone start_mm must be less than end_mm")
            if zone.get("vmax_mms", 0) <= 0:
                raise ValidationError("Zone vmax_mms must be greater than 0")
        
        logger.info("Configuration validation passed", version=config.version)
    
    def get_current_config(self) -> SystemConfig:
        """Lấy cấu hình hiện tại"""
        if not self.current_config:
            raise ConfigurationError("No current configuration available")
        return self.current_config
    
    def get_config_history(self, limit: int = 10) -> List[ConfigHistory]:
        """Lấy lịch sử cấu hình"""
        return self.config_history[-limit:] if self.config_history else []
    
    def get_config_by_version(self, version: int) -> Optional[SystemConfig]:
        """Lấy cấu hình theo version"""
        history_file = self.config_dir / f"history_v{version}.json"
        if history_file.exists():
            try:
                with open(history_file, 'r', encoding='utf-8') as f:
                    data = json.load(f)
                    return SystemConfig(**data["config_data"])
            except Exception as e:
                logger.error("Failed to load config version", version=version, error=str(e))
        return None
    
    def update_config(self, config_data: Dict[str, Any], user: str = "system") -> ConfigResponse:
        """Cập nhật cấu hình"""
        try:
            # Tạo config mới với version tăng
            new_version = (self.current_config.version + 1) if self.current_config else 1
            config_data["version"] = new_version
            
            new_config = SystemConfig(**config_data)
            
            # Validate config
            self._validate_config(new_config)
            
            # Lưu history của config cũ
            if self.current_config:
                self._save_config_history(self.current_config, "updated", user)
            
            # Cập nhật config hiện tại
            self.current_config = new_config
            self._save_current_config()
            
            # Lưu history của config mới
            self._save_config_history(new_config, "created", user)
            
            logger.info("Configuration updated successfully", 
                       old_version=self.current_config.version - 1 if self.current_config else None,
                       new_version=new_config.version)
            
            return ConfigResponse(
                success=True,
                message="Configuration updated successfully",
                config=new_config,
                version=new_config.version
            )
            
        except Exception as e:
            logger.error("Failed to update configuration", error=str(e))
            raise ConfigurationError(f"Failed to update configuration: {str(e)}")
    
    def rollback_config(self, version: int, user: str = "system") -> ConfigResponse:
        """Rollback về version cũ"""
        try:
            # Lấy config từ version
            rollback_config = self.get_config_by_version(version)
            if not rollback_config:
                raise ConfigurationError(f"Configuration version {version} not found")
            
            # Validate config
            self._validate_config(rollback_config)
            
            # Lưu history của config hiện tại
            if self.current_config:
                self._save_config_history(self.current_config, "rollback_from", user)
            
            # Tạo version mới với config rollback
            new_version = (self.current_config.version + 1) if self.current_config else 1
            rollback_config.version = new_version
            
            # Cập nhật config hiện tại
            self.current_config = rollback_config
            self._save_current_config()
            
            # Lưu history của config rollback
            self._save_config_history(rollback_config, "rollback_to", user)
            
            logger.info("Configuration rollback successful", 
                       from_version=version, to_version=new_version)
            
            return ConfigResponse(
                success=True,
                message=f"Configuration rolled back to version {version}",
                config=rollback_config,
                version=new_version
            )
            
        except Exception as e:
            logger.error("Failed to rollback configuration", error=str(e))
            raise ConfigurationError(f"Failed to rollback configuration: {str(e)}")
    
    def export_config(self, format: str = "json") -> str:
        """Export cấu hình hiện tại"""
        if not self.current_config:
            raise ConfigurationError("No current configuration available")
        
        if format.lower() == "yaml":
            return yaml.dump(self.current_config.dict(), default_flow_style=False, allow_unicode=True)
        else:
            return json.dumps(self.current_config.dict(), indent=2, ensure_ascii=False)
    
    def import_config(self, config_content: str, format: str = "json", user: str = "system") -> ConfigResponse:
        """Import cấu hình từ string"""
        try:
            if format.lower() == "yaml":
                config_data = yaml.safe_load(config_content)
            else:
                config_data = json.loads(config_content)
            
            return self.update_config(config_data, user)
            
        except Exception as e:
            logger.error("Failed to import configuration", error=str(e))
            raise ConfigurationError(f"Failed to import configuration: {str(e)}")
    
    def validate_config_data(self, config_data: Dict[str, Any]) -> bool:
        """Validate config data mà không lưu"""
        try:
            config = SystemConfig(**config_data)
            self._validate_config(config)
            return True
        except Exception:
            return False
