from typing import Dict, Any, List
import os
from fastapi import APIRouter, HTTPException

try:
    import httpx  # type: ignore
except Exception:
    httpx = None


router = APIRouter(prefix="/api/v1/ap", tags=["wifi-ap"]) 


def _is_dev_mode() -> bool:
    env = os.getenv("ENVIRONMENT", "development").lower()
    testing = os.getenv("TESTING", "false").lower() == "true"
    return env != "production" or testing


@router.get("/status")
async def ap_status() -> Dict[str, Any]:
    """Get WiFi AP status (running, ssid, channel, clients)."""
    if _is_dev_mode():
        return {
            "running": True,
            "ssid": "OHT50-AP",
            "channel": 6,
            "interface": "wlan0",
            "clients": [
                {"mac": "AA:BB:CC:DD:EE:01", "ip": "192.168.50.10", "rssi": -50},
                {"mac": "AA:BB:CC:DD:EE:02", "ip": "192.168.50.11", "rssi": -62},
            ],
        }

    firmware_url = os.getenv("FIRMWARE_URL", os.getenv("FIRMWARE_BASE_URL", "")).rstrip("/")
    if firmware_url and httpx is not None:
        async with httpx.AsyncClient(base_url=firmware_url, timeout=5) as client:
            try:
                resp = await client.get("/api/v1/network/ap/status")
                resp.raise_for_status()
                return resp.json()
            except Exception:
                return {"running": False, "ssid": None, "channel": None, "interface": "wlan0", "clients": []}
    return {"running": False, "ssid": None, "channel": None, "interface": "wlan0", "clients": []}


@router.post("/start")
async def ap_start() -> Dict[str, Any]:
    """Start WiFi AP (dev mock or firmware proxy)."""
    if _is_dev_mode():
        return {"success": True, "message": "AP started", "ssid": "OHT50-AP", "channel": 6}

    firmware_url = os.getenv("FIRMWARE_URL", os.getenv("FIRMWARE_BASE_URL", "")).rstrip("/")
    if firmware_url and httpx is not None:
        async with httpx.AsyncClient(base_url=firmware_url, timeout=5) as client:
            try:
                resp = await client.post("/api/v1/network/ap/start")
                resp.raise_for_status()
                return resp.json()
            except httpx.HTTPStatusError as e:
                raise HTTPException(status_code=e.response.status_code, detail=e.response.text)
            except Exception as e:
                raise HTTPException(status_code=502, detail=f"Firmware proxy failed: {e}")
    raise HTTPException(status_code=501, detail="AP start not available. Configure FIRMWARE_URL.")


@router.post("/stop")
async def ap_stop() -> Dict[str, Any]:
    """Stop WiFi AP (dev mock or firmware proxy)."""
    if _is_dev_mode():
        return {"success": True, "message": "AP stopped"}

    firmware_url = os.getenv("FIRMWARE_URL", os.getenv("FIRMWARE_BASE_URL", "")).rstrip("/")
    if firmware_url and httpx is not None:
        async with httpx.AsyncClient(base_url=firmware_url, timeout=5) as client:
            try:
                resp = await client.post("/api/v1/network/ap/stop")
                resp.raise_for_status()
                return resp.json()
            except httpx.HTTPStatusError as e:
                raise HTTPException(status_code=e.response.status_code, detail=e.response.text)
            except Exception as e:
                raise HTTPException(status_code=502, detail=f"Firmware proxy failed: {e}")
    raise HTTPException(status_code=501, detail="AP stop not available. Configure FIRMWARE_URL.")


@router.get("/clients")
async def ap_clients() -> Dict[str, Any]:
    """List connected clients to WiFi AP."""
    if _is_dev_mode():
        return {
            "clients": [
                {"mac": "AA:BB:CC:DD:EE:01", "ip": "192.168.50.10", "rssi": -50},
                {"mac": "AA:BB:CC:DD:EE:02", "ip": "192.168.50.11", "rssi": -62},
            ]
        }

    firmware_url = os.getenv("FIRMWARE_URL", os.getenv("FIRMWARE_BASE_URL", "")).rstrip("/")
    if firmware_url and httpx is not None:
        async with httpx.AsyncClient(base_url=firmware_url, timeout=5) as client:
            try:
                resp = await client.get("/api/v1/network/ap/clients")
                resp.raise_for_status()
                return resp.json()
            except Exception:
                return {"clients": []}
    return {"clients": []}


