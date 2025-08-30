"""
Configuration management for OHT-50 Backend
"""

import os
from typing import Optional
from pydantic_settings import BaseSettings
from pydantic import Field


class Settings(BaseSettings):
    """Application settings"""
    
    # Application
    app_name: str = "OHT-50 Backend"
    app_version: str = "1.0.0"
    debug: bool = Field(default=False, env="DEBUG")
    
    # Server
    host: str = Field(default="0.0.0.0", env="HOST")
    port: int = Field(default=8000, env="PORT")
    
    # Database
    database_url: str = Field(
        default="sqlite+aiosqlite:///./oht50.db",
        env="DATABASE_URL"
    )
    
    # Redis
    redis_url: str = Field(
        default="redis://localhost:6379",
        env="REDIS_URL"
    )
    
    # Security
    jwt_secret: str = Field(default="your-secret-key-change-in-production", env="JWT_SECRET")
    jwt_algorithm: str = Field(default="HS256", env="JWT_ALGORITHM")
    jwt_expiry: int = Field(default=3600, env="JWT_EXPIRY")  # 1 hour
    
    # Rate Limiting
    rate_limit_requests: int = Field(default=1000, env="RATE_LIMIT_REQUESTS")
    rate_limit_window: int = Field(default=60, env="RATE_LIMIT_WINDOW")  # 1 minute
    
    # Hardware Integration
    rs485_port: str = Field(default="/dev/ttyOHT485", env="RS485_PORT")
    rs485_baud_rate: int = Field(default=115200, env="RS485_BAUD_RATE")
    rs485_timeout: float = Field(default=1.0, env="RS485_TIMEOUT")
    
    # Performance
    max_workers: int = Field(default=4, env="MAX_WORKERS")
    log_level: str = Field(default="INFO", env="LOG_LEVEL")
    
    # Monitoring
    enable_metrics: bool = Field(default=True, env="ENABLE_METRICS")
    metrics_port: int = Field(default=9090, env="METRICS_PORT")
    
    class Config:
        env_file = ".env"
        case_sensitive = False


# Global settings instance
settings = Settings()


def get_settings() -> Settings:
    """Get application settings"""
    return settings
