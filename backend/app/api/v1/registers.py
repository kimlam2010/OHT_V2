"""
Registers CRUD API for OHT-50 Backend
"""

from typing import List
from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.ext.asyncio import AsyncSession

from app.core.database import get_db
from app.core.security import require_permission
from app.models.user import User
from app.schemas.registers import RegisterOut, RegisterCreate, RegisterUpdate
from app.services.register_service import register_service


router = APIRouter(prefix="/api/v1/registers", tags=["registers"]) 


@router.get("/", response_model=List[RegisterOut])
async def list_registers(
    db: AsyncSession = Depends(get_db),
    current_user: User = Depends(require_permission("monitoring", "read")),
):
    items = await register_service.list(db)
    return items


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


