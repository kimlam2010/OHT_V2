"""
Admin Register APIs - OHT-50 Backend
Admin-only endpoints for register management and sync
"""

from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.ext.asyncio import AsyncSession
from pydantic import BaseModel
from typing import Optional

from app.core.database import get_db
from app.core.security import require_permission
from app.models.user import User
from app.services.register_sync_service import register_sync_service

router = APIRouter(prefix="/api/v1/admin/registers", tags=["admin-registers"])


class SyncRequest(BaseModel):
    """Request model for sync endpoint"""
    module_addr: Optional[int] = None  # If None, sync all modules


@router.post("/sync-from-firmware")
async def sync_registers_from_firmware(
    request: SyncRequest,
    db: AsyncSession = Depends(get_db),
    current_user: User = Depends(require_permission("system", "configure")),
):
    """
    Sync register metadata from Firmware to Database
    
    **Permission:** SYSTEM/CONFIGURE (ADMIN role required)
    
    **Request Body:**
    ```json
    {
      "module_addr": 2  // Optional: specific module (2-5), or omit to sync all
    }
    ```
    
    **Response:**
    ```json
    {
      "success": true,
      "total_synced": 51,
      "total_created": 48,
      "total_updated": 3,
      "failed_modules": [],
      "message": "Successfully synced module 2"
    }
    ```
    
    **Module Addresses:**
    - 2: Power Module
    - 3: Safety Module
    - 4: Travel Motor Module
    - 5: Dock Module
    
    **Errors:**
    - 400: Invalid module_addr
    - 403: Admin role required
    - 500: Sync failed
    """
    # Validate admin role
    if (current_user.role or "").lower() not in ("admin", "system"):
        raise HTTPException(status_code=403, detail="Admin role required")
    
    # Sync specific module or all modules
    if request.module_addr:
        # Validate module address
        if request.module_addr not in [2, 3, 4, 5]:
            raise HTTPException(
                status_code=400,
                detail="Invalid module_addr. Must be 2 (Power), 3 (Safety), 4 (Motor), or 5 (Dock)"
            )
        
        # Sync single module
        result = await register_sync_service.sync_module_registers(db, request.module_addr)
        
        if not result["success"]:
            raise HTTPException(status_code=500, detail=result.get("error", "Sync failed"))
        
        return {
            "success": True,
            "total_synced": result["synced"],
            "total_created": result["created"],
            "total_updated": result["updated"],
            "failed_modules": [],
            "message": f"Successfully synced module {request.module_addr}"
        }
    else:
        # Sync all modules
        result = await register_sync_service.sync_all_modules(db)
        
        if not result["success"]:
            raise HTTPException(
                status_code=500,
                detail=result.get("error", "Sync failed for some modules")
            )
        
        return {
            "success": True,
            "total_synced": result["total_synced"],
            "total_created": result["total_created"],
            "total_updated": result["total_updated"],
            "failed_modules": result["failed_modules"],
            "message": "Successfully synced all modules"
        }

