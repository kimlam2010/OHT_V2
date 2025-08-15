"""
RS485 Management API: cấu hình, registry module, giám sát điểm đo
"""

from typing import Any, Dict, List, Optional

from fastapi import APIRouter, HTTPException, Query, Request, Depends

from app.core.logging import get_logger
from app.models.rs485 import BusConfig, ModuleInfo, PointsSnapshot
from app.core.security import verify_token
from app.services.rs485_bus import RS485Bus, RS485Settings

logger = get_logger(__name__)
router = APIRouter(tags=["RS485"]) 


# In-memory mock registry & config (placeholder, sẽ nối với service thật)
_bus_config: Optional[BusConfig] = None
_registry: Dict[int, ModuleInfo] = {}
_bus: Optional[RS485Bus] = None


@router.get("/config", response_model=BusConfig, summary="Lấy cấu hình RS485", dependencies=[Depends(verify_token)])
async def get_bus_config():
    global _bus_config
    if _bus_config is None:
        raise HTTPException(status_code=404, detail="Chưa cấu hình RS485")
    return _bus_config


@router.put("/config", response_model=BusConfig, summary="Cập nhật cấu hình RS485", dependencies=[Depends(verify_token)])
async def update_bus_config(cfg: BusConfig):
    global _bus_config
    global _bus
    _bus_config = cfg
    # Tạo bus instance
    _bus = RS485Bus(RS485Settings(device=cfg.device, baud=cfg.baud))
    logger.info("RS485 config updated", device=cfg.device, baud=cfg.baud)
    return cfg


@router.get("/modules", response_model=List[ModuleInfo], summary="Danh sách module trên bus", dependencies=[Depends(verify_token)])
async def list_modules():
    return list(_registry.values())


@router.post("/discover", response_model=List[ModuleInfo], summary="Quét auto-discovery", dependencies=[Depends(verify_token)])
async def discover_modules(request: Request, start: int = Query(16, ge=1, le=247), end: int = Query(64, ge=1, le=247)):
    global _bus
    global _registry
    if _bus is None:
        raise HTTPException(status_code=400, detail="Chưa cấu hình RS485 bus")
    # Gợi ý: Gửi PING/GET_INFO tới dải địa chỉ (theo bus_rs485.md)
    found: List[ModuleInfo] = []
    for addr in range(start, end + 1):
        try:
            resp = _bus.transact(addr=addr, cmd=0x01, payload=b"", request=request)  # 0x01: PING/GET_INFO (gợi ý)
            if resp is not None:
                found.append(ModuleInfo(address=addr, module_type="unknown", name=f"Module@{addr}", online=True))
        except Exception:
            continue
    for m in found:
        _registry[m.address] = m
    logger.info("Discovery completed", extra={"count": len(found)})
    return found


@router.get("/points", response_model=PointsSnapshot, summary="Ảnh chụp điểm đo nhanh", dependencies=[Depends(verify_token)])
async def get_points_snapshot(request: Request) -> PointsSnapshot:
    # Tối thiểu: đọc nhanh một số điểm nếu địa chỉ đã biết
    global _bus_config
    global _bus
    if _bus is None or _bus_config is None:
        raise HTTPException(status_code=400, detail="Chưa cấu hình RS485 bus")

    snapshot = PointsSnapshot()
    try:
        if _bus_config.addresses and _bus_config.addresses.location is not None:
            # Ví dụ đọc encoder count từ Location module
            payload = _bus.transact(_bus_config.addresses.location, 0x20, b"", request=request)  # 0x20: READ_COUNT (gợi ý)
            if payload and len(payload) >= 4:
                # Giải mã 32-bit little-endian
                count = int.from_bytes(payload[:4], byteorder="little", signed=False)
                snapshot.encoder_count = count
        # Tùy module khác: đọc DI/AI/motor feedback nếu có quy ước lệnh
    except Exception as e:
        logger.warning("RS485 points snapshot lỗi", extra={"error": str(e)})
    return snapshot


