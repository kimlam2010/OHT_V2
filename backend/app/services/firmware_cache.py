"""
Firmware Cache Service - OHT-50
Multi-level caching (Memory + Redis) cho firmware data
"""

from typing import Dict, Any, Optional, Callable
import json
import logging
from cachetools import TTLCache
import redis.asyncio as redis

from app.config import settings

logger = logging.getLogger(__name__)

class FirmwareCache:
    """
    Multi-level cache cho firmware data
    L1: Memory cache (fastest)
    L2: Redis cache (persistent)
    L3: Firmware fetch (slowest)
    """
    
    def __init__(self):
        # L1: Memory cache với TTL
        self.memory_cache = TTLCache(maxsize=1000, ttl=1.0)
        
        # L2: Redis cache
        self.redis_client = redis.Redis(
            host=settings.redis_host,
            port=settings.redis_port,
            decode_responses=True,
            socket_connect_timeout=5,
            socket_timeout=5
        )
        
        # Cache TTL configs cho từng loại data
        self.cache_ttls = {
            "robot_status": 0.5,      # 500ms - thay đổi thường xuyên
            "telemetry": 0.2,          # 200ms - real-time data
            "safety_status": 1.0,      # 1s - ổn định hơn
            "modules": 5.0,            # 5s - ít thay đổi
            "battery": 2.0,            # 2s - battery thay đổi chậm
            "configuration": 30.0,    # 30s - config ít thay đổi
            "default": 1.0             # 1s default
        }
        
        logger.info("FirmwareCache initialized với multi-level caching")
    
    async def get_with_cache(self, key: str, fetcher: Callable, cache_type: str = "default") -> Any:
        """
        Get data với multi-level caching
        
        Args:
            key: Cache key
            fetcher: Function để fetch data từ firmware
            cache_type: Loại cache để determine TTL
            
        Returns:
            Cached hoặc fresh data
        """
        # L1: Memory cache check
        if key in self.memory_cache:
            logger.debug(f"Cache HIT (L1): {key}")
            return self.memory_cache[key]
        
        # L2: Redis cache check
        try:
            cached_data = await self.redis_client.get(f"fw:{key}")
            if cached_data:
                data = json.loads(cached_data)
                # Store in L1 cache
                self.memory_cache[key] = data
                logger.debug(f"Cache HIT (L2): {key}")
                return data
        except Exception as e:
            logger.warning(f"Redis cache error: {e}")
        
        # L3: Fetch from firmware
        logger.debug(f"Cache MISS: {key}, fetching from firmware")
        data = await fetcher()
        
        # Store in both caches
        self.memory_cache[key] = data
        
        try:
            ttl = self.cache_ttls.get(cache_type, 1.0)
            await self.redis_client.setex(
                f"fw:{key}", 
                int(ttl), 
                json.dumps(data, default=str)
            )
            logger.debug(f"Stored in cache: {key} (TTL: {ttl}s)")
        except Exception as e:
            logger.warning(f"Redis store error: {e}")
        
        return data
    
    async def get_robot_status_cached(self, fetcher: Callable) -> Dict[str, Any]:
        """Get robot status với caching"""
        return await self.get_with_cache("robot_status", fetcher, "robot_status")
    
    async def get_telemetry_cached(self, fetcher: Callable) -> Dict[str, Any]:
        """Get telemetry data với caching"""
        return await self.get_with_cache("telemetry", fetcher, "telemetry")
    
    async def get_safety_status_cached(self, fetcher: Callable) -> Dict[str, Any]:
        """Get safety status với caching"""
        return await self.get_with_cache("safety_status", fetcher, "safety_status")
    
    async def get_modules_status_cached(self, fetcher: Callable) -> Dict[str, Any]:
        """Get modules status với caching"""
        return await self.get_with_cache("modules_status", fetcher, "modules")
    
    async def get_battery_cached(self, fetcher: Callable) -> Dict[str, Any]:
        """Get battery data với caching"""
        return await self.get_with_cache("battery", fetcher, "battery")
    
    async def invalidate_cache(self, pattern: str = "*"):
        """
        Invalidate cache entries
        
        Args:
            pattern: Pattern để match keys (default: all)
        """
        try:
            # Invalidate Redis cache
            keys = await self.redis_client.keys(f"fw:{pattern}")
            if keys:
                await self.redis_client.delete(*keys)
                logger.info(f"Invalidated {len(keys)} Redis cache entries")
        except Exception as e:
            logger.warning(f"Cache invalidation error: {e}")
        
        # Clear memory cache
        self.memory_cache.clear()
        logger.info("Memory cache cleared")
    
    async def close(self):
        """Close Redis connection"""
        try:
            await self.redis_client.close()
            logger.info("FirmwareCache closed")
        except Exception as e:
            logger.warning(f"Error closing cache: {e}")

# Global cache instance
_cache_instance: Optional[FirmwareCache] = None

async def get_firmware_cache() -> FirmwareCache:
    """Get global firmware cache instance"""
    global _cache_instance
    if _cache_instance is None:
        _cache_instance = FirmwareCache()
    return _cache_instance

async def close_firmware_cache():
    """Close global firmware cache"""
    global _cache_instance
    if _cache_instance:
        await _cache_instance.close()
        _cache_instance = None
