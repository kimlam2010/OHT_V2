from typing import List, Dict, Any
import os
from datetime import datetime, timezone
from fastapi import APIRouter, HTTPException, Depends
from pydantic import BaseModel

try:
    import httpx  # type: ignore
except Exception:  # pragma: no cover
    httpx = None  # lazy import guard


router = APIRouter(prefix="/api/v1/wifi", tags=["wifi"]) 


class WifiConnectRequest(BaseModel):
    ssid: str
    password: str | None = None


def _is_dev_mode() -> bool:
    env = os.getenv("ENVIRONMENT", "development").lower()
    testing = os.getenv("TESTING", "false").lower() == "true"
    return env != "production" or testing


@router.get("/status")
async def wifi_status() -> Dict[str, Any]:
    """Get current WiFi connection status (read-only).

    In production, this should reflect real device state (via Firmware HTTP API).
    In dev/testing, returns mocked values.
    """
    if _is_dev_mode():
        return {
            "connected": True,
            "ssid": "OHT50-DEV",
            "rssi": -52,
            "link_quality": 78,
            "interface": "wlan0",
        }

    # Production: try firmware endpoint if configured
    firmware_url = os.getenv("FIRMWARE_URL", os.getenv("FIRMWARE_BASE_URL", "")).rstrip("/")
    if firmware_url and httpx is not None:
        async with httpx.AsyncClient(base_url=firmware_url, timeout=5) as client:
            try:
                resp = await client.get("/api/v1/network/wifi/status")
                resp.raise_for_status()
                return resp.json()
            except Exception as _e:
                # Fall back to minimal info
                return {"connected": False, "ssid": None, "rssi": None, "link_quality": None, "interface": "wlan0"}

    # If no firmware URL, return minimal
    return {"connected": False, "ssid": None, "rssi": None, "link_quality": None, "interface": "wlan0"}


@router.get("/scan")
async def wifi_scan() -> Dict[str, Any]:
    """Scan available WiFi networks.
    Dev/testing only. In production, requires Firmware API support.
    """
    if not _is_dev_mode():
        raise HTTPException(status_code=403, detail="WiFi scan is disabled in production. Use Firmware API.")

    # Mock scan list
    return {
        "networks": [
            {"ssid": "OHT50-DEV", "rssi": -48, "security": "WPA2"},
            {"ssid": "Office-2G", "rssi": -60, "security": "WPA2"},
            {"ssid": "Guest", "rssi": -72, "security": "Open"},
        ]
    }


@router.post("/connect")
async def wifi_connect(body: WifiConnectRequest) -> Dict[str, Any]:
    """Connect to a WiFi network.
    - Dev/testing: mock success
    - Production: proxy to Firmware HTTP API if available
    """
    if _is_dev_mode():
        return {"success": True, "message": f"Connected to {body.ssid}", "ssid": body.ssid}

    firmware_url = os.getenv("FIRMWARE_URL", os.getenv("FIRMWARE_BASE_URL", "")).rstrip("/")
    if firmware_url and httpx is not None:
        async with httpx.AsyncClient(base_url=firmware_url, timeout=5) as client:
            try:
                resp = await client.post("/api/v1/network/wifi/connect", json=body.dict())
                resp.raise_for_status()
                return resp.json()
            except httpx.HTTPStatusError as e:
                raise HTTPException(status_code=e.response.status_code, detail=e.response.text)
            except Exception as e:
                raise HTTPException(status_code=502, detail=f"Firmware proxy failed: {e}")

    raise HTTPException(status_code=501, detail="WiFi connect not available. Configure FIRMWARE_URL.")


@router.post("/disconnect")
async def wifi_disconnect() -> Dict[str, Any]:
    """Disconnect from current WiFi network.
    - Dev/testing: mock success
    - Production: proxy to Firmware HTTP API if available
    """
    if _is_dev_mode():
        return {"success": True, "message": "Disconnected", "ssid": None}

    firmware_url = os.getenv("FIRMWARE_URL", os.getenv("FIRMWARE_BASE_URL", "")).rstrip("/")
    if firmware_url and httpx is not None:
        async with httpx.AsyncClient(base_url=firmware_url, timeout=5) as client:
            try:
                resp = await client.post("/api/v1/network/wifi/disconnect")
                resp.raise_for_status()
                return resp.json()
            except httpx.HTTPStatusError as e:
                raise HTTPException(status_code=e.response.status_code, detail=e.response.text)
            except Exception as e:
                raise HTTPException(status_code=502, detail=f"Firmware proxy failed: {e}")

    raise HTTPException(status_code=501, detail="WiFi disconnect not available. Configure FIRMWARE_URL.")


@router.get("/ip-config")
async def wifi_ip_config() -> Dict[str, Any]:
    """Get WiFi IP configuration (IP, gateway, DNS, etc.).
    - Dev/testing: mock configuration
    - Production: proxy to Firmware HTTP API if available
    """
    if _is_dev_mode():
        return {
            "success": True,
            "data": {
                "interface": "wlan0",
                "ip_address": "192.168.1.100",
                "subnet_mask": "255.255.255.0",
                "gateway": "192.168.1.1",
                "dns_servers": ["8.8.8.8", "8.8.4.4"],
                "dhcp_enabled": True,
                "mac_address": "aa:bb:cc:dd:ee:ff",
                "mtu": 1500,
                "connection_type": "dhcp"
            },
            "timestamp": datetime.now(timezone.utc).isoformat()
        }

    firmware_url = os.getenv("FIRMWARE_URL", os.getenv("FIRMWARE_BASE_URL", "")).rstrip("/")
    if firmware_url and httpx is not None:
        async with httpx.AsyncClient(base_url=firmware_url, timeout=5) as client:
            try:
                resp = await client.get("/api/v1/network/wifi/ip-config")
                resp.raise_for_status()
                return resp.json()
            except httpx.HTTPStatusError as e:
                raise HTTPException(status_code=e.response.status_code, detail=e.response.text)
            except Exception as e:
                raise HTTPException(status_code=502, detail=f"Firmware proxy failed: {e}")

    raise HTTPException(status_code=501, detail="WiFi IP config not available. Configure FIRMWARE_URL.")


