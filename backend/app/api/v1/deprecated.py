"""
Deprecated endpoints returning 410 Gone with migration hints
"""

from fastapi import APIRouter, HTTPException


router = APIRouter()


def _gone(new_path: str):
    raise HTTPException(status_code=410, detail={
        "message": "Endpoint deprecated",
        "use": new_path
    })


# Firmware health → health
@router.api_route("/api/v1/firmware_health/{path:path}", methods=["GET", "POST", "PUT", "PATCH", "DELETE"])
async def deprecated_firmware_health(path: str):
    return _gone("/api/v1/health")


# Module telemetry → rs485 telemetry
@router.api_route("/api/v1/module_telemetry/{path:path}", methods=["GET", "POST", "PUT", "PATCH", "DELETE"])
async def deprecated_module_telemetry(path: str):
    return _gone("/api/v1/rs485/modules/{address}/telemetry")


# FW integration → internal service only
@router.api_route("/api/v1/fw_integration/{path:path}", methods=["GET", "POST", "PUT", "PATCH", "DELETE"])
async def deprecated_fw_integration(path: str):
    return _gone("(internal service) RS485/HTTP firmware client")


# Speed control → robot
@router.api_route("/api/v1/speed_control/{path:path}", methods=["GET", "POST", "PUT", "PATCH", "DELETE"])
async def deprecated_speed_control(path: str):
    return _gone("/api/v1/robot/*")


# Map, localization, communication, network → parked
@router.api_route("/api/v1/map/{path:path}", methods=["GET", "POST", "PUT", "PATCH", "DELETE"])
async def deprecated_map(path: str):
    return _gone("(parked) to be reintroduced under /api/v1/navigation/* later")


@router.api_route("/api/v1/localization/{path:path}", methods=["GET", "POST", "PUT", "PATCH", "DELETE"])
async def deprecated_localization(path: str):
    return _gone("(parked) to be reintroduced under /api/v1/navigation/* later")


@router.api_route("/api/v1/communication/{path:path}", methods=["GET", "POST", "PUT", "PATCH", "DELETE"])
async def deprecated_communication(path: str):
    return _gone("(parked) consolidated into internal services")


@router.api_route("/api/v1/network/{path:path}", methods=["GET", "POST", "PUT", "PATCH", "DELETE"])
async def deprecated_network(path: str):
    return _gone("(parked) consolidated into ops endpoints later")


