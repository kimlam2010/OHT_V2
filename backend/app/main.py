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
from app.api.v1 import auth, robot, telemetry, safety, config, monitoring
from app.api import websocket

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
        
        # Start monitoring service (with timeout)
        try:
            await asyncio.wait_for(monitoring_service.start_monitoring(interval_seconds=30), timeout=10.0)
            logger.info("✅ Monitoring service started")
        except asyncio.TimeoutError:
            logger.warning("⚠️ Monitoring service startup timeout, continuing without it")
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
    # OHT-50 Autonomous Mobile Robot System - Backend API
    
    ## Overview
    This is the backend API for the OHT-50 Autonomous Mobile Robot System, providing:
    
    - **Robot Control**: Movement, status monitoring, and emergency controls
    - **Telemetry**: Real-time sensor data and system monitoring
    - **Safety Management**: Safety zones, alerts, and emergency procedures
    - **Configuration**: System settings and parameter management
    - **Authentication**: User management and access control
    - **WebSocket**: Real-time communication for live updates
    
    ## Key Features
    - Real-time robot control and monitoring
    - Comprehensive safety management system
    - High-performance telemetry processing
    - Secure authentication and authorization
    - WebSocket-based real-time updates
    - Comprehensive error handling and recovery
    
    ## Performance Targets
    - API Response Time: < 50ms
    - WebSocket Latency: < 20ms
    - System Uptime: > 99.9%
    - Error Rate: < 0.1%
    
    ## Authentication
    Most endpoints require authentication. Use the `/api/v1/auth/login` endpoint to obtain a JWT token,
    then include it in the Authorization header: `Bearer <token>`
    
    ## Error Handling
    All endpoints return consistent error responses with appropriate HTTP status codes.
    Detailed error information is provided in development mode.
    
    ## Rate Limiting
    API endpoints are rate-limited to ensure system stability.
    Contact administrators for rate limit adjustments.
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
            "description": "User authentication and authorization endpoints"
        },
        {
            "name": "Robot Control",
            "description": "Robot movement, status, and control endpoints"
        },
        {
            "name": "Telemetry",
            "description": "Real-time sensor data and monitoring endpoints"
        },
        {
            "name": "Safety",
            "description": "Safety management and emergency control endpoints"
        },
        {
            "name": "Configuration",
            "description": "System configuration and parameter management"
        },
        {
            "name": "Monitoring",
            "description": "System health and performance monitoring"
        },
        {
            "name": "WebSocket",
            "description": "Real-time communication endpoints"
        }
    ],
    lifespan=lifespan
)

# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Configure appropriately for production
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


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
app.include_router(auth.router, prefix="/api/v1")
app.include_router(robot.router, prefix="/api/v1")
app.include_router(telemetry.router, prefix="/api/v1")
app.include_router(safety.router, prefix="/api/v1")
app.include_router(config.router, prefix="/api/v1")
app.include_router(monitoring.router, prefix="/api/v1")

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
