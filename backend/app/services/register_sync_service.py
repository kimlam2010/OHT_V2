"""
Register Sync Service - OHT-50 Backend
Syncs register metadata from Firmware API to Database
"""

from typing import Dict, Any, List
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select
import logging

from app.models.register import Register, RegisterMode, RegisterAccessLevel
from app.services.unified_firmware_service import get_firmware_service

logger = logging.getLogger(__name__)


class RegisterSyncService:
    """Service for syncing register metadata from Firmware to Database"""
    
    # Module mapping
    MODULE_MAP = {
        2: "POWER",
        3: "SAFETY",
        4: "TRAVEL_MOTOR",
        5: "DOCK"
    }
    
    async def sync_module_registers(self, db: AsyncSession, module_addr: int) -> Dict[str, Any]:
        """
        Sync registers for a single module
        
        Args:
            db: Database session
            module_addr: Module address (2=Power, 3=Safety, 4=Motor, 5=Dock)
        
        Returns:
            {
              "success": True,
              "module_addr": 2,
              "module_name": "Power Module",
              "synced": 58,
              "created": 48,
              "updated": 10,
              "failed": 0,
              "errors": []
            }
        """
        logger.info(f"Starting sync for module {module_addr}")
        
        firmware = await get_firmware_service()
        
        # Get register metadata from Firmware
        response = await firmware.get_module_registers(module_addr)
        
        if not response.success:
            logger.error(f"Failed to get registers from Firmware for module {module_addr}: {response.error}")
            return {
                "success": False,
                "module_addr": module_addr,
                "error": response.error
            }
        
        data = response.data.get("data", {})
        registers = data.get("registers", [])
        module_name = data.get("module_name", self.MODULE_MAP.get(module_addr))
        
        logger.info(f"Received {len(registers)} registers from Firmware for module {module_addr} ({module_name})")
        
        created_count = 0
        updated_count = 0
        failed_count = 0
        errors = []
        
        for reg in registers:
            try:
                # Upsert logic
                address = reg["address"].lower()
                
                # Check if exists
                result = await db.execute(
                    select(Register).where(Register.address == address)
                )
                existing = result.scalar_one_or_none()
                
                # Map mode
                mode = self._map_mode(reg.get("mode", "READ"))
                
                # Map access_level (default to USER if not specified)
                access_level = RegisterAccessLevel.USER
                
                if existing:
                    # Update existing register (including module info)
                    existing.name = reg.get("name", existing.name)
                    existing.mode = mode
                    existing.description = reg.get("description", existing.description)
                    existing.module_addr = module_addr  # NEW: Save module address
                    existing.module_name = module_name  # NEW: Save module name
                    updated_count += 1
                    logger.debug(f"Updated register {address}")
                else:
                    # Create new register (including module info)
                    new_reg = Register(
                        name=reg.get("name", f"Register {address}"),
                        address=address,
                        mode=mode,
                        access_level=access_level,
                        is_safe_register=False,
                        unit=reg.get("unit"),
                        description=reg.get("description"),
                        module_addr=module_addr,  # NEW: Save module address
                        module_name=module_name   # NEW: Save module name
                    )
                    db.add(new_reg)
                    created_count += 1
                    logger.debug(f"Created register {address}")
                    
            except Exception as e:
                failed_count += 1
                errors.append({
                    "address": reg.get("address"),
                    "error": str(e)
                })
                logger.error(f"Failed to sync register {reg.get('address')}: {e}")
        
        # Commit transaction
        try:
            await db.commit()
            logger.info(f"Successfully committed sync for module {module_addr}: {created_count} created, {updated_count} updated")
        except Exception as e:
            await db.rollback()
            logger.error(f"Database commit failed for module {module_addr}: {e}")
            return {
                "success": False,
                "module_addr": module_addr,
                "error": f"Database commit failed: {e}"
            }
        
        return {
            "success": True,
            "module_addr": module_addr,
            "module_name": data.get("module_name", self.MODULE_MAP.get(module_addr)),
            "synced": len(registers),
            "created": created_count,
            "updated": updated_count,
            "failed": failed_count,
            "errors": errors
        }
    
    async def sync_all_modules(self, db: AsyncSession) -> Dict[str, Any]:
        """
        Sync registers from all online modules
        
        Args:
            db: Database session
        
        Returns:
            {
              "success": True,
              "total_synced": 146,
              "total_created": 120,
              "total_updated": 26,
              "failed_modules": [],
              "results": [...]
            }
        """
        logger.info("Starting sync for all online modules")
        
        firmware = await get_firmware_service()
        
        # Get online modules
        response = await firmware.get_online_modules()
        
        if not response.success:
            logger.error(f"Failed to get online modules: {response.error}")
            return {
                "success": False,
                "error": f"Failed to get online modules: {response.error}"
            }
        
        modules_data = response.data.get("data", {})
        modules = modules_data.get("modules", [])
        
        if not modules:
            logger.warning("No online modules found")
            return {
                "success": False,
                "error": "No online modules found"
            }
        
        logger.info(f"Found {len(modules)} online modules")
        
        results = []
        total_synced = 0
        total_created = 0
        total_updated = 0
        failed_modules = []
        
        for module in modules:
            module_addr = module["address"]
            
            logger.info(f"Syncing module {module_addr} ({module.get('type')})")
            
            # Sync module
            result = await self.sync_module_registers(db, module_addr)
            results.append(result)
            
            if result["success"]:
                total_synced += result["synced"]
                total_created += result["created"]
                total_updated += result["updated"]
            else:
                failed_modules.append({
                    "address": module_addr,
                    "type": module.get("type"),
                    "error": result.get("error")
                })
        
        logger.info(f"Sync complete: {total_synced} total registers, {total_created} created, {total_updated} updated")
        
        return {
            "success": len(failed_modules) == 0,
            "total_synced": total_synced,
            "total_created": total_created,
            "total_updated": total_updated,
            "failed_modules": failed_modules,
            "results": results
        }
    
    def _map_mode(self, firmware_mode: str) -> RegisterMode:
        """Map Firmware mode to Backend RegisterMode"""
        mode_upper = firmware_mode.upper()
        if mode_upper == "READ":
            return RegisterMode.READ
        elif mode_upper == "WRITE":
            return RegisterMode.WRITE
        elif mode_upper == "READ_WRITE":
            return RegisterMode.READ_WRITE
        else:
            logger.warning(f"Unknown mode '{firmware_mode}', defaulting to READ")
            return RegisterMode.READ  # Default


# Global instance
register_sync_service = RegisterSyncService()

