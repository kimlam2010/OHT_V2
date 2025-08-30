"""
OHT-50 Backend Main Application Entry Point
"""

import uvicorn
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from fastapi.middleware.trustedhost import TrustedHostMiddleware

from app.config import settings
from app.core.monitoring import setup_monitoring
from app.core.database import init_db
from app.core.security import create_default_roles, create_default_admin_user
from app.api.v1 import router as api_v1_router
from app.api.websocket import router as websocket_router


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
    app.include_router(websocket_router, prefix="/ws")
    
    @app.get("/")
    async def root():
        """Root endpoint"""
        return {
            "message": "OHT-50 Backend API",
            "version": settings.app_version,
            "status": "running",
            "docs": "/docs" if settings.debug else None
        }
    
    @app.get("/health")
    async def health_check():
        """Health check endpoint"""
        return {"status": "healthy"}
    
    # Database initialization
    @app.on_event("startup")
    async def startup_event():
        """Initialize database and create default data"""
        try:
            # Initialize database
            await init_db()
            print("Database initialized successfully")
            
            # Create default roles and admin user
            from app.core.database import AsyncSessionLocal
            async with AsyncSessionLocal() as db:
                await create_default_roles(db)
                await create_default_admin_user(db)
            print("Default roles and admin user created")
            
        except Exception as e:
            print(f"Startup error: {e}")
    
    return app


def main():
    """Main application entry point"""
    app = create_app()
    
    uvicorn.run(  # type: ignore
        app,
        host=settings.host,
        port=settings.port,
        log_level=settings.log_level.lower(),
        workers=settings.max_workers,
    )


if __name__ == "__main__":
    main()
