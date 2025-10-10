"""
Registers CRUD API for OHT-50 Backend
"""

from typing import List, Optional
from fastapi import APIRouter, Depends, HTTPException, status, Query
from sqlalchemy.ext.asyncio import AsyncSession

from app.core.database import get_db
from app.core.security import require_permission
from app.models.user import User
from app.schemas.registers import RegisterOut, RegisterCreate, RegisterUpdate
from app.services.register_service import register_service


router = APIRouter(prefix="/api/v1/registers", tags=["registers"]) 


@router.get("/", response_model=List[RegisterOut])
async def list_registers(
    mode: Optional[str] = Query(None, description="Filter by mode (READ/WRITE/READ_WRITE)"),
    access_level: Optional[str] = Query(None, description="Filter by access level (USER/ADMIN/SYSTEM)"),
    module_addr: Optional[int] = Query(None, description="Filter by module address (future - not used yet)"),
    db: AsyncSession = Depends(get_db),
    current_user: User = Depends(require_permission("monitoring", "read")),
):
    """
    List registers with optional filters
    
    **Query Parameters:**
    - `mode`: Filter by mode (READ/WRITE/READ_WRITE)
    - `access_level`: Filter by access level (USER/ADMIN/SYSTEM)
    - `module_addr`: Filter by module address (future enhancement)
    
    **Examples:**
    - `/api/v1/registers` - List all registers
    - `/api/v1/registers?mode=READ` - List only READ registers
    - `/api/v1/registers?access_level=USER` - List only USER access registers
    - `/api/v1/registers?mode=READ&access_level=USER` - Combined filters
    """
    items = await register_service.list_with_filters(db, mode, access_level, module_addr)
    return items


@router.get("/by-address/{address}", response_model=RegisterOut)
async def get_register_by_address(
    address: str,
    db: AsyncSession = Depends(get_db),
    current_user: User = Depends(require_permission("monitoring", "read")),
):
    """
    Get register by hex address
    
    **Example:** `/api/v1/registers/by-address/0x0000`
    """
    item = await register_service.get_by_address(db, address)
    if not item:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Register not found")
    return item


@router.get("/{register_id}", response_model=RegisterOut)
async def get_register(
    register_id: int,
    db: AsyncSession = Depends(get_db),
    current_user: User = Depends(require_permission("monitoring", "read")),
):
    item = await register_service.get(db, register_id)
    if not item:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Register not found")
    return item


@router.post("/", response_model=RegisterOut)
async def create_register(
    payload: RegisterCreate,
    db: AsyncSession = Depends(get_db),
    current_user: User = Depends(require_permission("system", "configure")),
):
    # Only admins/system should create
    if (current_user.role or "").lower() not in ("admin", "system"):
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Admin or system role required")
    return await register_service.create(db, payload)


@router.put("/{register_id}", response_model=RegisterOut)
async def update_register(
    register_id: int,
    payload: RegisterUpdate,
    db: AsyncSession = Depends(get_db),
    current_user: User = Depends(require_permission("system", "configure")),
):
    if (current_user.role or "").lower() not in ("admin", "system"):
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Admin or system role required")
    item = await register_service.update(db, register_id, payload)
    if not item:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Register not found")
    return item


@router.delete("/{register_id}")
async def delete_register(
    register_id: int,
    db: AsyncSession = Depends(get_db),
    current_user: User = Depends(require_permission("system", "configure")),
):
    if (current_user.role or "").lower() not in ("admin", "system"):
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Admin or system role required")
    ok = await register_service.delete(db, register_id)
    if not ok:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Register not found")
    return {"success": True}


