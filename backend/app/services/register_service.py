"""
Register service: CRUD and access control for RS485 registers
"""

from typing import List, Optional
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select

from app.models.register import Register as RegisterModel, RegisterMode, RegisterAccessLevel
from app.schemas.registers import RegisterCreate, RegisterUpdate
from app.models.user import User


class RegisterService:
    async def list(self, db: AsyncSession) -> List[RegisterModel]:
        result = await db.execute(select(RegisterModel))
        return list(result.scalars().all())

    async def get(self, db: AsyncSession, register_id: int) -> Optional[RegisterModel]:
        result = await db.execute(select(RegisterModel).where(RegisterModel.id == register_id))
        return result.scalar_one_or_none()

    async def get_by_address(self, db: AsyncSession, address: str) -> Optional[RegisterModel]:
        result = await db.execute(select(RegisterModel).where(RegisterModel.address == address.lower()))
        return result.scalar_one_or_none()

    async def create(self, db: AsyncSession, data: RegisterCreate) -> RegisterModel:
        item = RegisterModel(
            name=data.name,
            address=data.address.lower(),
            mode=data.mode,
            access_level=data.access_level,
            is_safe_register=data.is_safe_register,
            unit=data.unit,
            description=data.description,
        )
        db.add(item)
        await db.commit()
        await db.refresh(item)
        return item

    async def update(self, db: AsyncSession, register_id: int, data: RegisterUpdate) -> Optional[RegisterModel]:
        item = await self.get(db, register_id)
        if not item:
            return None
        for field in ("name", "address", "mode", "access_level", "is_safe_register", "unit", "description"):
            value = getattr(data, field)
            if value is not None:
                setattr(item, field, value if field != "address" else value.lower())
        await db.commit()
        await db.refresh(item)
        return item

    async def delete(self, db: AsyncSession, register_id: int) -> bool:
        item = await self.get(db, register_id)
        if not item:
            return False
        await db.delete(item)  # type: ignore[arg-type]
        await db.commit()
        return True

    def validate_write_permission(self, user: User, register: RegisterModel, *, force: bool = False) -> None:
        # Mode gate
        if register.mode == RegisterMode.READ:
            raise PermissionError("Register is read-only")

        # Access level gate
        role = (user.role or "viewer").lower()
        if register.access_level == RegisterAccessLevel.ADMIN and role != "admin":
            raise PermissionError("Admin access required")
        if register.access_level == RegisterAccessLevel.SYSTEM and role not in ("admin", "system"):
            raise PermissionError("System access required")

        # Safety register gate
        if register.is_safe_register and not force:
            raise PermissionError("Safe register change requires 'force' flag and elevated permissions")


register_service = RegisterService()


