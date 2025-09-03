"""
OHT-50 Backend Main Application

This is the main entry point for the OHT-50 backend system.
"""

import os
import logging
from contextlib import asynccontextmanager
from fastapi import FastAPI, Request
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse

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
        
        # Start monitoring service
        await monitoring_service.start_monitoring(interval_seconds=30)
        logger.info("✅ Monitoring service started")
        
        # Start WebSocket service
        await websocket_service.start()
        logger.info("✅ WebSocket service started")
        
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
    description="Backend API for OHT-50 Autonomous Mobile Robot System",
    version="2.0.0",
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
    """Health check endpoint"""
    try:
        # Get system health from monitoring service
        health_data = await monitoring_service.get_system_health()
        
        return {
            "success": True,
            "status": "healthy",
            "timestamp": health_data.get("last_updated", "unknown"),
            "system_health": health_data.get("status", "unknown"),
            "overall_health_score": health_data.get("overall_health_score", 0)
        }
    except Exception as e:
        logger.error(f"❌ Health check failed: {e}")
        return {
            "success": False,
            "status": "unhealthy",
            "error": str(e),
            "timestamp": "unknown"
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
