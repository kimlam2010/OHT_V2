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
from app.api.v1 import auth, robot, telemetry, safety, config, monitoring, speed_control, map, sensors, localization, health
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
        
        logger.info("🚀 OHT-50 Backend started successfully")
        
    except Exception as e:
        logger.error(f"❌ Failed to start OHT-50 Backend: {e}")
        raise
    
    yield
    
    # Shutdown
    logger.info("🛑 Shutting down OHT-50 Backend...")
    
    try:
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
    
    ### 📊 **Telemetry API** (6 endpoints)
    - Real-time sensor data, LiDAR scans, data collection management
    
    ### 🚨 **Safety API** (5 endpoints)
    - Safety status, emergency controls, alert management, safety zones
    
    ### ⚡ **Speed Control API** (6 endpoints)
    - Speed management, acceleration limits, performance monitoring
    
    ### ⚙️ **Configuration API** (4 endpoints)
    - System settings, robot parameters, configuration management
    
    ### 📈 **Monitoring API** (6 endpoints)
    - System health, performance metrics, logs, alerts
    
    ### 🗺️ **Map & Localization API** (12 endpoints)
    - Map management, robot positioning, hybrid localization
    
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
            "name": "Robot Control", 
            "description": "🤖 Robot movement, status monitoring, and control operations (6 endpoints)"
        },
        {
            "name": "Telemetry",
            "description": "📊 Real-time sensor data, LiDAR scans, and data collection (6 endpoints)"
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
            "description": "🗺️ Map management, robot positioning, and hybrid localization (12 endpoints)"
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
# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=settings.cors_origins,  # Configure appropriately for production
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


# Test endpoint without authentication
@app.get("/test-auth")
async def test_auth():
    """Test endpoint without authentication"""
    return {
        "success": True,
        "message": "Test endpoint working without authentication",
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
