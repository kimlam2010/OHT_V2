#!/usr/bin/env python3
"""
Simple Backend Server for FW Integration Testing

This is a minimal FastAPI server to test firmware integration
without requiring full backend dependencies.
"""

import asyncio
import logging
import sys
import os
from typing import Dict, Any

# Add backend to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '.'))

# Set environment variables
os.environ['USE_MOCK_FIRMWARE'] = 'false'
os.environ['TESTING'] = 'false'
os.environ['ENVIRONMENT'] = 'development'

from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

# Create FastAPI app
app = FastAPI(
    title="OHT-50 FW Integration Server",
    description="Simple server for testing firmware integration",
    version="1.0.0"
)

# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Global firmware service
firmware_service = None

@app.on_event("startup")
async def startup_event():
    """Initialize firmware service on startup"""
    global firmware_service
    try:
        from app.services.firmware_integration_service import FirmwareIntegrationService
        
        logger.info("🚀 Starting FW Integration Server...")
        firmware_service = FirmwareIntegrationService()
        
        # Initialize firmware service
        success = await firmware_service.initialize()
        if success:
            logger.info("✅ Firmware Integration Service initialized")
        else:
            logger.warning("⚠️ Firmware Integration Service initialization failed")
            
    except Exception as e:
        logger.error(f"❌ Startup error: {e}")

@app.on_event("shutdown")
async def shutdown_event():
    """Cleanup on shutdown"""
    global firmware_service
    if firmware_service:
        await firmware_service.shutdown()
        logger.info("✅ Firmware Integration Service shutdown")

@app.get("/")
async def root():
    """Root endpoint"""
    return {
        "message": "OHT-50 FW Integration Server",
        "status": "running",
        "version": "1.0.0"
    }

@app.get("/health")
async def health():
    """Health check endpoint"""
    return {
        "status": "healthy",
        "service": "fw-integration-server",
        "firmware_connected": firmware_service is not None
    }

@app.get("/api/v1/fw/system/status")
async def get_firmware_system_status():
    """Get firmware system status"""
    if not firmware_service:
        raise HTTPException(status_code=503, detail="Firmware service not available")
    
    try:
        status = await firmware_service.get_system_status()
        return status
    except Exception as e:
        logger.error(f"❌ Failed to get system status: {e}")
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/v1/fw/system/health")
async def get_firmware_system_health():
    """Get firmware system health"""
    if not firmware_service:
        raise HTTPException(status_code=503, detail="Firmware service not available")
    
    try:
        health = await firmware_service.get_system_health()
        return health
    except Exception as e:
        logger.error(f"❌ Failed to get system health: {e}")
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/v1/fw/modules")
async def get_firmware_modules():
    """Get firmware modules"""
    if not firmware_service:
        raise HTTPException(status_code=503, detail="Firmware service not available")
    
    try:
        modules = await firmware_service.get_modules()
        return {
            "success": True,
            "data": modules,
            "count": len(modules)
        }
    except Exception as e:
        logger.error(f"❌ Failed to get modules: {e}")
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/v1/fw/status")
async def get_firmware_connection_status():
    """Get firmware connection status"""
    if not firmware_service:
        return {
            "status": "disconnected",
            "message": "Firmware service not initialized"
        }
    
    try:
        status = firmware_service.get_connection_status()
        return {
            "success": True,
            "data": status
        }
    except Exception as e:
        logger.error(f"❌ Failed to get connection status: {e}")
        return {
            "status": "error",
            "error": str(e)
        }

@app.get("/api/v1/communication/status")
async def get_communication_status():
    """Get communication status"""
    if not firmware_service:
        return {
            "success": False,
            "error": "Firmware service not available"
        }
    
    try:
        # Get firmware connection status
        fw_status = firmware_service.get_connection_status()
        
        # Get system health
        system_health = await firmware_service.get_system_health()
        
        return {
            "success": True,
            "data": {
                "overall_status": "healthy" if fw_status["status"] == "connected" else "degraded",
                "firmware_connection": fw_status,
                "system_health": system_health,
                "communication_channels": {
                    "http_api": {
                        "status": "active" if fw_status["status"] == "connected" else "inactive",
                        "protocol": "HTTP/1.1",
                        "port": 8080
                    },
                    "websocket": {
                        "status": "active" if fw_status["status"] == "connected" else "inactive",
                        "protocol": "WebSocket",
                        "port": 8081
                    }
                }
            }
        }
    except Exception as e:
        logger.error(f"❌ Failed to get communication status: {e}")
        raise HTTPException(status_code=500, detail=str(e))

if __name__ == "__main__":
    import uvicorn
    
    logger.info("🚀 Starting Simple FW Integration Server on port 8000...")
    uvicorn.run(
        "simple_server:app",
        host="0.0.0.0",
        port=8000,
        reload=False,
        log_level="info"
    )
