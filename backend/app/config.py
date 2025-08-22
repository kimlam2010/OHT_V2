"""
Cấu hình môi trường cho Backend Service OHT-50
"""
from typing import Optional, List

from pydantic import Field
from pydantic_settings import BaseSettings


class Settings(BaseSettings):
    """Cấu hình ứng dụng từ environment variables"""

    # Server settings
    host: str = Field(default="0.0.0.0", description="Host để bind server")
    port: int = Field(default=8000, description="Port để bind server")
    debug: bool = Field(default=True, description="Chế độ debug")
    # CORS
    cors_origins: List[str] = Field(
        default_factory=lambda: ["*"], description="Danh sách origins cho CORS"
    )

    # Database
    database_url: str = Field(
        default="sqlite:///./oht50.db", description="Database connection URL"
    )

    # Logging
    log_level: str = Field(default="INFO", description="Log level")
    log_format: str = Field(default="json", description="Log format")

    # Security
    secret_key: str = Field(
        default="oht50-secret-key-change-in-production",
        description="Secret key cho JWT",
    )
    access_token_expire_minutes: int = Field(
        default=30, description="Thời gian hết hạn access token (phút)"
    )

    # Telemetry
    telemetry_ws_rate: int = Field(
        default=10, description="Tốc độ gửi telemetry qua WebSocket (Hz)"
    )
    telemetry_history_size: int = Field(
        default=1000, description="Số lượng telemetry records lưu trong memory"
    )

    # Center Communication
    center_ws_url: str = Field(
        default="ws://localhost:8080/ws", description="WebSocket URL của Center"
    )
    center_http_base: str = Field(
        default="http://localhost:8081", description="HTTP base URL của FW/Center nội bộ"
    )
    center_http_timeout_s: float = Field(
        default=3.0, description="Timeout (giây) khi BE gọi HTTP tới FW/Center"
    )
    center_heartbeat_interval: int = Field(
        default=2, description="Khoảng thời gian gửi heartbeat (giây)"
    )

    # RS485 Communication
    rs485_device: str = Field(
        default="/dev/ttyOHT485", description="Device path cho RS485"
    )
    rs485_baud: int = Field(default=115200, description="Baud rate cho RS485")

    # Motor Configuration
    motor_vmax_mms: float = Field(
        default=500.0, description="Tốc độ tối đa motor (mm/s)"
    )
    motor_amax_mms2: float = Field(
        default=300.0, description="Gia tốc tối đa motor (mm/s²)"
    )
    motor_jmax_mms3: float = Field(
        default=5000.0, description="Jerk tối đa motor (mm/s³)"
    )

    # Safety
    estop_reset_policy: str = Field(
        default="manual", description="Chính sách reset E-Stop (manual/auto)"
    )

    # FW Integration
    FW_HOST: str = Field(
        default="localhost", description="FW server host"
    )
    FW_HTTP_PORT: int = Field(
        default=8080, description="FW HTTP server port"
    )
    FW_WS_PORT: int = Field(
        default=8081, description="FW WebSocket server port"
    )
    FW_AUTH_TOKEN: Optional[str] = Field(
        default=None, description="FW authentication token"
    )
    FW_SECRET_KEY: Optional[str] = Field(
        default=None, description="FW secret key for message signing"
    )

    class Config:
        env_file = ".env"
        env_file_encoding = "utf-8"
        case_sensitive = False


# Global settings instance
settings = Settings()


def get_settings() -> Settings:
    """Lấy cấu hình ứng dụng"""
    return settings
