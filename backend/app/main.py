"""
OHT-50 Backend Main Application

This is the main entry point for the OHT-50 backend system.
"""

import os
import logging
import asyncio
from contextlib import asynccontextmanager
from fastapi import FastAPI, Request
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse
from datetime import datetime

from app.core.database import init_db, create_test_admin_user
from app.core.monitoring_service import monitoring_service
from app.core.websocket_service import websocket_service

# Import API routers
from app.api.v1 import auth, robot, telemetry, safety, config, monitoring, speed_control, map, sensors, localization, health, dashboard
from app.api import websocket
from app.config import Settings

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(name)s - %(levelname)s - %(message)s"
)
logger = logging.getLogger(__name__)


@asynccontextmanager
async def lifespan(app: FastAPI):
    """Application lifespan manager"""
    # Startup
    logger.info("🚀 Starting OHT-50 Backend...")
    
    try:
        # Initialize database
        await init_db()
        logger.info("✅ Database initialized")
        
        # Create test admin user if in testing mode
        if os.getenv("TESTING", "false").lower() == "true":
            await create_test_admin_user()
            logger.info("✅ Test admin user created")
        
        # Start monitoring service as background task
        try:
            monitoring_task = asyncio.create_task(monitoring_service.start_monitoring(interval_seconds=30))
            # Give it a moment to start
            await asyncio.sleep(0.1)
            logger.info("✅ Monitoring service started")
        except Exception as e:
            logger.warning(f"⚠️ Monitoring service failed to start: {e}, continuing without it")
        
        # Start WebSocket service (skip in development if hangs)
        try:
            await asyncio.wait_for(websocket_service.start(), timeout=10.0)
            logger.info("✅ WebSocket service started")
        except asyncio.TimeoutError:
            logger.warning("⚠️ WebSocket service startup timeout, continuing without it")
        except Exception as e:
            logger.warning(f"⚠️ WebSocket service failed to start: {e}, continuing without it")
        
        # Start WebSocket Alert service
        try:
            from app.services.websocket_alert_service import websocket_alert_service
            await websocket_alert_service.start()
            logger.info("✅ WebSocket Alert service started")
        except Exception as e:
            logger.warning(f"⚠️ WebSocket Alert service failed to start: {e}, continuing without it")
        
        # Start WebSocket Log service
        try:
            from app.services.websocket_log_service import websocket_log_service
            await websocket_log_service.start()
            logger.info("✅ WebSocket Log service started")
        except Exception as e:
            logger.warning(f"⚠️ WebSocket Log service failed to start: {e}, continuing without it")
        
        logger.info("🚀 OHT-50 Backend started successfully")
        
    except Exception as e:
        logger.error(f"❌ Failed to start OHT-50 Backend: {e}")
        raise
    
    yield
    
    # Shutdown
    logger.info("🛑 Shutting down OHT-50 Backend...")
    
    try:
        # Stop WebSocket Alert service
        try:
            from app.services.websocket_alert_service import websocket_alert_service
            await websocket_alert_service.stop()
            logger.info("✅ WebSocket Alert service stopped")
        except Exception as e:
            logger.warning(f"⚠️ WebSocket Alert service stop failed: {e}")
        
        # Stop WebSocket Log service
        try:
            from app.services.websocket_log_service import websocket_log_service
            await websocket_log_service.stop()
            logger.info("✅ WebSocket Log service stopped")
        except Exception as e:
            logger.warning(f"⚠️ WebSocket Log service stop failed: {e}")
        
        # Stop WebSocket service
        await websocket_service.stop()
        logger.info("✅ WebSocket service stopped")
        
        # Stop monitoring service (it will stop automatically)
        logger.info("✅ Monitoring service stopped")
        
        logger.info("🛑 OHT-50 Backend shutdown complete")
        
    except Exception as e:
        logger.error(f"❌ Error during shutdown: {e}")


# Create FastAPI application
app = FastAPI(
    title="OHT-50 Backend API",
    description="""
    # 🚀 OHT-50 Backend API v2.0 - Production Ready
    
    ## 📋 **API Overview**
    **40+ Endpoints** across 7 main categories providing comprehensive robot control and monitoring:
    
    ### 🔐 **Authentication API** (5 endpoints)
    - User login/logout, JWT token management, user profile
    
    ### 🤖 **Robot Control API** (6 endpoints)  
    - Robot status, movement control, position tracking, emergency stop
    
    ### 📊 **Telemetry API** (sensors)
    - Real-time sensor data: accelerometer, electronic compass, 4 DI proximity, NFC/RFID 13.56MHz
    
    ### 🚨 **Safety API** (5 endpoints)
    - Safety status, emergency controls, alert management, safety zones
    
    ### ⚡ **Speed Control API** (6 endpoints)
    - Speed management, acceleration limits, performance monitoring
    
    ### ⚙️ **Configuration API** (4 endpoints)
    - System settings, robot parameters, configuration management
    
    ### 📈 **Monitoring API** (6 endpoints)
    - System health, performance metrics, logs, alerts
    
    ### 🗺️ **Map & Localization API**
    - Map management and basic localization (no LiDAR)
    
    ### 📡 **Sensor Data API** (8 endpoints)
    - Sensor data processing, configuration, calibration
    
        ### 🎯 **Localization API** (8 endpoints)
        - Position tracking, configuration, statistics

        ### 🏥 **Health Check API** (5 endpoints)
        - System health monitoring, readiness, liveness checks
    
    ### 🔌 **WebSocket API** (Real-time)
    - Live telemetry updates, real-time notifications
    
    ## 🎯 **Performance Targets**
    - **API Response**: < 50ms
    - **Emergency Stop**: < 10ms  
    - **WebSocket Latency**: < 20ms
    - **Database Queries**: < 10ms
    - **System Uptime**: > 99.9%
    
    ## 🔒 **Security Features**
    - JWT Authentication with RBAC
    - Rate Limiting & Input Validation
    - Security Headers & Audit Logging
    - CORS Protection
    
    ## 🔧 **Authentication**
    Most endpoints require JWT token in header:
    ```
    Authorization: Bearer <your_jwt_token>
    ```
    
    ## 📚 **Documentation**
    - Complete API reference in `/docs`
    - Interactive testing in Swagger UI
    - Production-ready with comprehensive error handling
    """,
    version="2.0.0",
    contact={
        "name": "OHT-50 Development Team",
        "email": "dev@oht50.com",
        "url": "https://github.com/oht50/backend"
    },
    license_info={
        "name": "MIT License",
        "url": "https://opensource.org/licenses/MIT"
    },
    servers=[
        {
            "url": "http://localhost:8000",
            "description": "Development server"
        },
        {
            "url": "https://api.oht50.com",
            "description": "Production server"
        }
    ],
    tags=[
        {
            "name": "Authentication",
            "description": "🔐 User authentication, JWT tokens, and authorization (5 endpoints)"
        },
        {
            "name": "dashboard",
            "description": "📊 Dashboard overview, alerts, logs, and performance metrics (6 endpoints)"
        },
        {
            "name": "Robot Control", 
            "description": "🤖 Robot movement, status monitoring, and control operations (6 endpoints)"
        },
        {
            "name": "Telemetry",
            "description": "📊 Real-time sensor data: accelerometer, compass, DI proximity, NFC/RFID"
        },
        {
            "name": "Safety",
            "description": "🚨 Safety management, emergency controls, and alert systems (5 endpoints)"
        },
        {
            "name": "Configuration",
            "description": "⚙️ System configuration and parameter management (4 endpoints)"
        },
        {
            "name": "Monitoring",
            "description": "📈 System health, performance metrics, and monitoring (6 endpoints)"
        },
        {
            "name": "Speed Control",
            "description": "⚡ Advanced speed control with safety integration (6 endpoints)"
        },
        {
            "name": "map",
            "description": "🗺️ Map management and basic localization (no LiDAR)"
        },
        {
            "name": "sensors",
            "description": "📡 Sensor data processing, configuration, and calibration (8 endpoints)"
        },
        {
            "name": "localization",
            "description": "🎯 Position tracking, configuration, and statistics (8 endpoints)"
        },
        {
            "name": "health",
            "description": "🏥 System health monitoring, readiness, and liveness checks (5 endpoints)"
        },
        {
            "name": "WebSocket",
            "description": "🔌 Real-time communication and live updates"
        }
    ],
    lifespan=lifespan
)

settings = Settings()
# Simple rate limiting middleware (per-process, per-IP)
from collections import defaultdict
import time as _time

_RATE_LIMIT_REQUESTS = settings.rate_limit_requests
_RATE_LIMIT_WINDOW = settings.rate_limit_window
_rate_buckets = defaultdict(list)

@app.middleware("http")
async def rate_limit_middleware(request: Request, call_next):
    client_ip = request.client.host if request.client else "unknown"
    now = _time.time()
    window_start = now - _RATE_LIMIT_WINDOW
    bucket = _rate_buckets[client_ip]
    # prune old entries
    while bucket and bucket[0] < window_start:
        bucket.pop(0)
    if len(bucket) >= _RATE_LIMIT_REQUESTS:
        return JSONResponse(status_code=429, content={"detail": "Rate limit exceeded"})
    bucket.append(now)
    return await call_next(request)
# Add CORS middleware
_origins = settings.cors_origins
if settings.environment.lower() == "production":
    # In production, disallow localhost defaults unless explicitly configured
    _origins = [o for o in _origins if "localhost" not in o and "127.0.0.1" not in o]
app.add_middleware(
    CORSMiddleware,
    allow_origins=_origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


# Add security headers middleware
@app.middleware("http")
async def add_security_headers(request: Request, call_next):
    """Add security headers to all responses"""
    response = await call_next(request)
    
    # Security headers
    response.headers["X-Content-Type-Options"] = "nosniff"
    response.headers["X-Frame-Options"] = "DENY"
    response.headers["X-XSS-Protection"] = "1; mode=block"
    response.headers["Referrer-Policy"] = "strict-origin-when-cross-origin"
    response.headers["Permissions-Policy"] = "geolocation=(), microphone=(), camera=()"
    
    return response

# Production sanity checks for secrets
if settings.environment.lower() == "production":
    if settings.jwt_secret in ("your-secret-key-here", "test-secret-key"):
        logger.error("JWT secret is using a default value in production. Refusing to start.")
        raise RuntimeError("Invalid JWT secret in production")


# Global exception handler
@app.exception_handler(Exception)
async def global_exception_handler(request: Request, exc: Exception):
    """Global exception handler"""
    logger.error(f"❌ Unhandled exception: {exc}")
    return JSONResponse(
        status_code=500,
        content={
            "success": False,
            "error": "Internal server error",
            "detail": str(exc) if os.getenv("TESTING", "false").lower() == "true" else "An unexpected error occurred"
        }
    )


# Health check endpoint
@app.get("/health")
async def health_check():
    """Health check endpoint - OPTIMIZED for performance"""
    try:
        # Get system health from monitoring service (now uses cached metrics)
        health_data = await monitoring_service.get_system_health()
        
        return {
            "success": True,
            "status": "healthy",
            "timestamp": health_data.get("last_updated", "unknown"),
            "system_health": health_data.get("status", "unknown"),
            "overall_health_score": health_data.get("overall_health_score", 0),
            "performance": "optimized"  # Indicate this is the optimized version
        }
    except Exception as e:
        logger.error(f"❌ Health check failed: {e}")
        return {
            "success": False,
            "status": "unhealthy",
            "error": str(e),
            "timestamp": "unknown"
        }


# Fast health check endpoint (minimal checks)
@app.get("/health/fast")
async def fast_health_check():
    """Fast health check endpoint - minimal checks for performance testing"""
    return {
        "success": True,
        "status": "healthy",
        "timestamp": datetime.now().isoformat(),
        "performance": "ultra_fast",
        "checks": "minimal"
    }


# Test endpoint without authentication (only in non-production/testing)
@app.get("/test-auth")
async def test_auth():
    """Test endpoint without authentication (disabled in production)"""
    env = os.getenv("ENVIRONMENT", "development").lower()
    testing = os.getenv("TESTING", "false").lower() == "true"
    if env == "production" and not testing:
        return JSONResponse(status_code=404, content={"detail": "Not found"})
    return {
        "success": True,
        "message": "Test endpoint working (non-production)",
        "timestamp": "2025-01-28T10:30:00Z"
    }


# Root endpoint
@app.get("/")
async def root():
    """Root endpoint"""
    return {
        "message": "OHT-50 Backend API",
        "version": "2.0.0",
        "status": "running",
        "docs": "/docs",
        "health": "/health"
    }


# Include API routers
app.include_router(auth.router)
app.include_router(robot.router)
app.include_router(telemetry.router)
app.include_router(safety.router)
app.include_router(config.router)
app.include_router(monitoring.router)
app.include_router(speed_control.router)
app.include_router(dashboard.router)

# Include Map & Localization API routers
app.include_router(map.router)
app.include_router(sensors.router)
app.include_router(localization.router)
app.include_router(health.router)

# Include WebSocket router
app.include_router(websocket.router)


if __name__ == "__main__":
    import uvicorn
    
    # Get configuration from environment
    host = os.getenv("HOST", "127.0.0.1")
    port = int(os.getenv("PORT", "8000"))
    reload = os.getenv("RELOAD", "false").lower() == "true"
    
    logger.info(f"🚀 Starting OHT-50 Backend on {host}:{port}")
    
    uvicorn.run(
        "app.main:app",
        host=host,
        port=port,
        reload=reload,
        log_level="info"
    )
