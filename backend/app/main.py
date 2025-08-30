"""
OHT-50 Backend Main Application Entry Point
"""

import uvicorn
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from fastapi.middleware.trustedhost import TrustedHostMiddleware

from app.config import settings
from app.core.monitoring import setup_monitoring
from app.api.v1 import router as api_v1_router


def create_app() -> FastAPI:
    """Create and configure FastAPI application"""
    
    app = FastAPI(
        title=settings.app_name,
        version=settings.app_version,
        description="OHT-50 Robot Control System Backend API",
        docs_url="/docs" if settings.debug else None,
        redoc_url="/redoc" if settings.debug else None,
    )
    
    # CORS middleware
    app.add_middleware(
        CORSMiddleware,
        allow_origins=["*"],  # Configure for production
        allow_credentials=True,
        allow_methods=["*"],
        allow_headers=["*"],
    )
    
    # Trusted host middleware
    app.add_middleware(
        TrustedHostMiddleware,
        allowed_hosts=["*"]  # Configure for production
    )
    
    # Setup monitoring
    if settings.enable_metrics:
        setup_monitoring(app)
    
    # Include API routers
    app.include_router(api_v1_router, prefix="/api/v1")
    
    @app.get("/")
    async def root():
        """Root endpoint"""
        return {
            "message": "OHT-50 Backend API",
            "version": settings.app_version,
            "status": "running"
        }
    
    @app.get("/health")
    async def health_check():
        """Health check endpoint"""
        return {"status": "healthy"}
    
    return app


def main():
    """Main application entry point"""
    app = create_app()
    
    uvicorn.run(
        app,
        host=settings.host,
        port=settings.port,
        log_level=settings.log_level.lower(),
        workers=settings.max_workers,
    )


if __name__ == "__main__":
    main()
