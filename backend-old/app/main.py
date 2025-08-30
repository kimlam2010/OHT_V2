"""
FastAPI application entry point cho OHT-50 Backend
"""
import time
from contextlib import asynccontextmanager
from typing import Any, Dict

from fastapi import FastAPI, Request, status
from fastapi.exceptions import RequestValidationError
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse
from fastapi.responses import Response
from starlette.exceptions import HTTPException as StarletteHTTPException

from app.api.v1 import center, config, health, telemetry, rs485, user, auth, mission, safety, fw_integration
from app.config import get_settings
from app.core.exceptions import OHT50Exception
from app.core.logging import CorrelationIdMiddleware, get_logger
from app.core.security import verify_token

# Global logger
logger = get_logger(__name__)

# Settings
settings = get_settings()


@asynccontextmanager
async def lifespan(app: FastAPI):
    """Application lifespan manager"""
    # Startup
    logger.info(
        "Starting OHT-50 Backend Service",
        extra={
            "version": "1.0.0",
            "host": settings.host,
            "port": settings.port,
            "debug": settings.debug,
        },
    )

    # Initialize services here if needed
    # await init_database()
    # await init_telemetry_service()

    yield

    # Shutdown
    logger.info("Shutting down OHT-50 Backend Service")


# Create FastAPI app
app = FastAPI(
    title="OHT-50 Backend Service",
    description="Backend service cho hệ thống điều khiển tự động hóa OHT-50",
    version="1.0.0",
    docs_url="/docs",
    redoc_url="/redoc",
    lifespan=lifespan,
)

# Add CORS middleware (from env)
origins = settings.cors_origins or ["*"]
app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Add correlation ID middleware
app.add_middleware(CorrelationIdMiddleware)


# Exception handlers
@app.exception_handler(OHT50Exception)
async def oht50_exception_handler(request: Request, exc: OHT50Exception):
    """Handle OHT-50 custom exceptions"""
    logger.error(
        "OHT-50 Exception",
        extra={
            "error_code": exc.error_code,
            "message": exc.message,
            "details": exc.details,
            "path": request.url.path,
            "method": request.method,
        },
    )

    return JSONResponse(
        status_code=status.HTTP_400_BAD_REQUEST,
        content={
            "success": False,
            "error_code": exc.error_code,
            "message": exc.message,
            "details": exc.details,
        },
    )


@app.exception_handler(RequestValidationError)
async def validation_exception_handler(request: Request, exc: RequestValidationError):
    """Handle validation errors"""
    logger.error(
        "Validation Error",
        extra={
            "errors": exc.errors(),
            "path": request.url.path,
            "method": request.method,
        },
    )

    return JSONResponse(
        status_code=status.HTTP_422_UNPROCESSABLE_ENTITY,
        content={
            "success": False,
            "error_code": "VALIDATION_FAILED",
            "message": "Dữ liệu không hợp lệ",
            "details": exc.errors(),
        },
    )


@app.exception_handler(StarletteHTTPException)
async def http_exception_handler(request: Request, exc: StarletteHTTPException):
    """Handle HTTP exceptions"""
    logger.error(
        "HTTP Exception",
        extra={
            "status_code": exc.status_code,
            "detail": exc.detail,
            "path": request.url.path,
            "method": request.method,
        },
    )

    return JSONResponse(
        status_code=exc.status_code,
        content={
            "success": False,
            "error_code": f"HTTP_{exc.status_code}",
            "message": exc.detail,
        },
    )


@app.exception_handler(Exception)
async def general_exception_handler(request: Request, exc: Exception):
    """Handle general exceptions"""
    logger.error(
        "General Exception",
        extra={
            "exception_type": type(exc).__name__,
            "exception_message": str(exc),
            "path": request.url.path,
            "method": request.method,
        },
        exc_info=True,
    )

    return JSONResponse(
        status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
        content={
            "success": False,
            "error_code": "INTERNAL_SERVER_ERROR",
            "message": "Lỗi nội bộ server",
        },
    )


# Request logging middleware
@app.middleware("http")
async def log_requests(request: Request, call_next):
    """Log all requests"""
    start_time = time.time()

    # Log request
    logger.info(
        "Request started",
        extra={
            "method": request.method,
            "path": request.url.path,
            "query_params": dict(request.query_params),
            "client_ip": request.client.host if request.client else None,
        },
    )

    # Process request
    response = await call_next(request)

    # Calculate duration
    duration = time.time() - start_time

    # Log response
    logger.info(
        "Request completed",
        extra={
            "method": request.method,
            "path": request.url.path,
            "status_code": response.status_code,
            "duration": round(duration, 3),
        },
    )

    # Add duration header
    response.headers["X-Request-Duration"] = str(round(duration, 3))

    return response


# Include API routers
app.include_router(health.router, prefix="/health")
app.include_router(config.router, prefix="/api/v1/config")
app.include_router(telemetry.router, prefix="/api/v1/telemetry")
app.include_router(center.router, prefix="/api/v1/center")
app.include_router(rs485.router, prefix="/api/v1/rs485")
app.include_router(user.router, prefix="/api/v1/user")
app.include_router(auth.router, prefix="/api/v1/auth")
app.include_router(mission.router, prefix="/api/v1/mission")
app.include_router(safety.router, prefix="/api/v1/safety")

# Import và include admin router riêng biệt
from app.api.v1 import admin
app.include_router(admin.router, prefix="/api/v1/admin", tags=["Admin"])

# Import và include analytics router
from app.api.v1 import analytics
app.include_router(analytics.router, prefix="/api/v1/analytics", tags=["Analytics"])

# Import và include hardware router
from app.api.v1 import hardware
app.include_router(hardware.router, prefix="/api/v1/hardware", tags=["Hardware Control"])
app.include_router(fw_integration.router, prefix="/api/v1/fw", tags=["FW Integration"])


@app.get("/", tags=["Root"])
async def root() -> Dict[str, Any]:
    """Root endpoint"""
    return {
        "success": True,
        "message": "OHT-50 Backend Service",
        "version": "1.0.0",
        "docs": "/docs",
        "health": "/health",
    }


@app.get("/info", tags=["Info"])
async def info() -> Dict[str, Any]:
    """Service information"""
    return {
        "success": True,
        "service": "OHT-50 Backend",
        "version": "1.0.0",
        "description": "Backend service cho hệ thống điều khiển tự động hóa OHT-50",
        "endpoints": {
            "health": "/health",
            "config": "/api/v1/config",
            "telemetry": "/api/v1/telemetry",
            "center": "/api/v1/center",
            "rs485": "/api/v1/rs485",
            "user": "/api/v1/user",
            "auth": "/api/v1/auth",
            "mission": "/api/v1/mission",
            "safety": "/api/v1/safety",
            "admin": "/api/v1/admin",
            "docs": "/docs",
        },
        "settings": {
            "host": settings.host,
            "port": settings.port,
            "debug": settings.debug,
            "log_level": settings.log_level,
        },
    }


@app.get("/favicon.ico")
async def favicon() -> Response:
    return Response(status_code=204)


if __name__ == "__main__":
    import uvicorn

    uvicorn.run(
        "app.main:app",
        host=settings.host,
        port=settings.port,
        reload=settings.debug,
        log_level=settings.log_level.lower(),
    )
