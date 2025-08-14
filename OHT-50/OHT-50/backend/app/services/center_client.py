"""
Client tích hợp FW/Center cho BE: gọi /modules và /points (HTTP)
"""

from __future__ import annotations

from typing import Any, Dict, List, Optional

import httpx

from app.core.logging import get_logger

logger = get_logger(__name__)


class CenterClient:
    def __init__(self, base_url: str, timeout_s: float = 3.0) -> None:
        self.base_url = base_url.rstrip('/')
        self.timeout_s = timeout_s
        self._client = httpx.AsyncClient(base_url=self.base_url, timeout=self.timeout_s)

    async def close(self) -> None:
        await self._client.aclose()

    async def get_modules(self) -> List[Dict[str, Any]]:
        try:
            resp = await self._client.get('/modules')
            resp.raise_for_status()
            return resp.json()
        except Exception as e:
            logger.error("CenterClient.get_modules lỗi", extra={"error": str(e)})
            return []

    async def get_points(self) -> Dict[str, Any]:
        try:
            resp = await self._client.get('/points')
            resp.raise_for_status()
            return resp.json()
        except Exception as e:
            logger.error("CenterClient.get_points lỗi", extra={"error": str(e)})
            return {}


