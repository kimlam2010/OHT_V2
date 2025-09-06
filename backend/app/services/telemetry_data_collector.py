"""
Telemetry Data Collector for OHT-50
Real-time data collection from sensors and LiDAR with high performance
"""

import asyncio
import logging
import time
from typing import Dict, Any, List, Optional, Callable
from datetime import datetime, timezone
from dataclasses import dataclass, field
from enum import Enum
from collections import deque
import statistics

logger = logging.getLogger(__name__)


class DataSource(Enum):
    """Data source types"""
    LIDAR = "lidar"
    RFID = "rfid"
    ACCELEROMETER = "accelerometer"
    COMPASS = "compass"
    DOCKING = "docking"
    MOTOR = "motor"
    BATTERY = "battery"
    TEMPERATURE = "temperature"
    SYSTEM = "system"


class DataQuality(Enum):
    """Data quality levels"""
    EXCELLENT = "excellent"
    GOOD = "good"
    FAIR = "fair"
    POOR = "poor"
    INVALID = "invalid"


@dataclass
class TelemetryData:
    """Telemetry data structure"""
    source: DataSource
    data: Dict[str, Any]
    timestamp: datetime
    quality: DataQuality = DataQuality.GOOD
    sequence_id: int = 0
    processing_time_ms: float = 0.0
    
    def __post_init__(self):
        if self.timestamp is None:
            self.timestamp = datetime.now(timezone.utc)


@dataclass
class CollectionStats:
    """Data collection statistics"""
    total_collected: int = 0
    total_processed: int = 0
    total_errors: int = 0
    avg_processing_time_ms: float = 0.0
    max_processing_time_ms: float = 0.0
    min_processing_time_ms: float = float('inf')
    data_rate_per_second: float = 0.0
    error_rate_percent: float = 0.0
    last_update: datetime = field(default_factory=lambda: datetime.now(timezone.utc))


class TelemetryDataCollector:
    """
    High-performance telemetry data collector
    Target: 1000+ data points/second with < 20ms latency
    """
    
    def __init__(self, max_buffer_size: int = 10000):
        self.max_buffer_size = max_buffer_size
        self.data_buffer: deque = deque(maxlen=max_buffer_size)
        self.stats = CollectionStats()
        
        # Data source handlers
        self.source_handlers: Dict[DataSource, Callable] = {}
        self.data_validators: Dict[DataSource, Callable] = {}
        
        # Performance tracking
        self.processing_times: deque = deque(maxlen=1000)
        self.collection_start_time = time.time()
        
        # Data quality thresholds
        self.quality_thresholds = {
            DataQuality.EXCELLENT: 0.95,
            DataQuality.GOOD: 0.85,
            DataQuality.FAIR: 0.70,
            DataQuality.POOR: 0.50
        }
        
        # Collection control
        self.is_collecting = False
        self.collection_tasks: List[asyncio.Task] = []
        
        logger.info("TelemetryDataCollector initialized with buffer size: %d", max_buffer_size)
    
    def register_source_handler(self, source: DataSource, handler: Callable):
        """Register data source handler"""
        self.source_handlers[source] = handler
        logger.info("Registered handler for data source: %s", source.value)
    
    def register_data_validator(self, source: DataSource, validator: Callable):
        """Register data validator for source"""
        self.data_validators[source] = validator
        logger.info("Registered validator for data source: %s", source.value)
    
    async def start_collection(self) -> bool:
        """Start data collection from all registered sources"""
        try:
            if self.is_collecting:
                logger.warning("Data collection already running")
                return True
            
            self.is_collecting = True
            self.collection_start_time = time.time()
            
            # Start collection tasks for each source
            for source, handler in self.source_handlers.items():
                task = asyncio.create_task(self._collect_from_source(source, handler))
                self.collection_tasks.append(task)
            
            logger.info("Started data collection from %d sources", len(self.source_handlers))
            return True
            
        except Exception as e:
            logger.error("Failed to start data collection: %s", e)
            self.is_collecting = False
            return False
    
    async def stop_collection(self) -> bool:
        """Stop data collection"""
        try:
            if not self.is_collecting:
                logger.warning("Data collection not running")
                return True
            
            self.is_collecting = False
            
            # Cancel all collection tasks
            for task in self.collection_tasks:
                task.cancel()
            
            # Wait for tasks to complete
            await asyncio.gather(*self.collection_tasks, return_exceptions=True)
            self.collection_tasks.clear()
            
            logger.info("Stopped data collection")
            return True
            
        except Exception as e:
            logger.error("Failed to stop data collection: %s", e)
            return False
    
    async def _collect_from_source(self, source: DataSource, handler: Callable):
        """Collect data from a specific source"""
        sequence_id = 0
        
        while self.is_collecting:
            try:
                start_time = time.perf_counter()
                
                # Get data from source
                raw_data = await handler()
                
                # Validate data
                if not self._validate_data(source, raw_data):
                    self.stats.total_errors += 1
                    logger.warning("Invalid data from source %s: %s", source.value, raw_data)
                    continue
                
                # Create telemetry data
                processing_time = (time.perf_counter() - start_time) * 1000
                telemetry_data = TelemetryData(
                    source=source,
                    data=raw_data,
                    timestamp=datetime.now(timezone.utc),
                    quality=self._assess_data_quality(source, raw_data),
                    sequence_id=sequence_id,
                    processing_time_ms=processing_time
                )
                
                # Add to buffer
                self.data_buffer.append(telemetry_data)
                
                # Update statistics
                self._update_stats(telemetry_data)
                
                sequence_id += 1
                
                # Small delay to prevent overwhelming
                await asyncio.sleep(0.001)  # 1ms delay
                
            except asyncio.CancelledError:
                logger.info("Collection task cancelled for source: %s", source.value)
                break
            except Exception as e:
                logger.error("Error collecting from source %s: %s", source.value, e)
                self.stats.total_errors += 1
                await asyncio.sleep(0.1)  # Wait before retry
    
    def _validate_data(self, source: DataSource, data: Dict[str, Any]) -> bool:
        """Validate data from source"""
        if not isinstance(data, dict):
            return False
        
        # Use custom validator if available
        if source in self.data_validators:
            try:
                return self.data_validators[source](data)
            except Exception as e:
                logger.error("Validator error for source %s: %s", source.value, e)
                return False
        
        # Default validation
        return len(data) > 0
    
    def _assess_data_quality(self, source: DataSource, data: Dict[str, Any]) -> DataQuality:
        """Assess data quality based on source and content"""
        try:
            # Basic quality assessment
            if not data or len(data) == 0:
                return DataQuality.INVALID
            
            # Source-specific quality assessment
            if source == DataSource.LIDAR:
                return self._assess_lidar_quality(data)
            elif source == DataSource.RFID:
                return self._assess_rfid_quality(data)
            elif source == DataSource.ACCELEROMETER:
                return self._assess_accelerometer_quality(data)
            elif source == DataSource.COMPASS:
                return self._assess_compass_quality(data)
            else:
                return DataQuality.GOOD
                
        except Exception as e:
            logger.error("Error assessing data quality: %s", e)
            return DataQuality.POOR
    
    def _assess_lidar_quality(self, data: Dict[str, Any]) -> DataQuality:
        """Assess LiDAR data quality"""
        try:
            points = data.get("points", [])
            if not points or len(points) < 100:
                return DataQuality.POOR
            
            # Check for reasonable point density
            if len(points) > 1000:
                return DataQuality.EXCELLENT
            elif len(points) > 500:
                return DataQuality.GOOD
            else:
                return DataQuality.FAIR
                
        except Exception:
            return DataQuality.POOR
    
    def _assess_rfid_quality(self, data: Dict[str, Any]) -> DataQuality:
        """Assess RFID data quality"""
        try:
            signal_strength = data.get("signal_strength", 0)
            if signal_strength > 80:
                return DataQuality.EXCELLENT
            elif signal_strength > 60:
                return DataQuality.GOOD
            elif signal_strength > 40:
                return DataQuality.FAIR
            else:
                return DataQuality.POOR
                
        except Exception:
            return DataQuality.POOR
    
    def _assess_accelerometer_quality(self, data: Dict[str, Any]) -> DataQuality:
        """Assess accelerometer data quality"""
        try:
            # Check for reasonable acceleration values
            x = abs(data.get("x", 0))
            y = abs(data.get("y", 0))
            z = abs(data.get("z", 0))
            
            total_acceleration = (x**2 + y**2 + z**2)**0.5
            
            if 9.0 <= total_acceleration <= 11.0:  # Near 1g
                return DataQuality.EXCELLENT
            elif 8.0 <= total_acceleration <= 12.0:
                return DataQuality.GOOD
            elif 7.0 <= total_acceleration <= 13.0:
                return DataQuality.FAIR
            else:
                return DataQuality.POOR
                
        except Exception:
            return DataQuality.POOR
    
    def _assess_compass_quality(self, data: Dict[str, Any]) -> DataQuality:
        """Assess compass data quality"""
        try:
            heading = data.get("heading", 0)
            if 0 <= heading <= 360:
                return DataQuality.GOOD
            else:
                return DataQuality.POOR
                
        except Exception:
            return DataQuality.POOR
    
    def _update_stats(self, telemetry_data: TelemetryData):
        """Update collection statistics"""
        self.stats.total_collected += 1
        self.stats.total_processed += 1
        
        # Update processing time statistics
        processing_time = telemetry_data.processing_time_ms
        self.processing_times.append(processing_time)
        
        if processing_time > self.stats.max_processing_time_ms:
            self.stats.max_processing_time_ms = processing_time
        
        if processing_time < self.stats.min_processing_time_ms:
            self.stats.min_processing_time_ms = processing_time
        
        # Calculate average processing time
        if self.processing_times:
            self.stats.avg_processing_time_ms = statistics.mean(self.processing_times)
        
        # Calculate data rate
        elapsed_time = time.time() - self.collection_start_time
        if elapsed_time > 0:
            self.stats.data_rate_per_second = self.stats.total_collected / elapsed_time
        
        # Calculate error rate
        if self.stats.total_collected > 0:
            self.stats.error_rate_percent = (self.stats.total_errors / self.stats.total_collected) * 100
        
        self.stats.last_update = datetime.now(timezone.utc)
    
    def get_latest_data(self, source: Optional[DataSource] = None, limit: int = 100) -> List[TelemetryData]:
        """Get latest telemetry data"""
        if source is None:
            # Return latest data from all sources
            return list(self.data_buffer)[-limit:]
        else:
            # Return latest data from specific source
            filtered_data = [data for data in self.data_buffer if data.source == source]
            return filtered_data[-limit:]
    
    def get_data_by_time_range(self, start_time: datetime, end_time: datetime, 
                              source: Optional[DataSource] = None) -> List[TelemetryData]:
        """Get data within time range"""
        filtered_data = []
        
        for data in self.data_buffer:
            if start_time <= data.timestamp <= end_time:
                if source is None or data.source == source:
                    filtered_data.append(data)
        
        return filtered_data
    
    def get_collection_stats(self) -> Dict[str, Any]:
        """Get collection statistics"""
        return {
            "total_collected": self.stats.total_collected,
            "total_processed": self.stats.total_processed,
            "total_errors": self.stats.total_errors,
            "avg_processing_time_ms": self.stats.avg_processing_time_ms,
            "max_processing_time_ms": self.stats.max_processing_time_ms,
            "min_processing_time_ms": self.stats.min_processing_time_ms if self.stats.min_processing_time_ms != float('inf') else 0.0,
            "data_rate_per_second": self.stats.data_rate_per_second,
            "error_rate_percent": self.stats.error_rate_percent,
            "buffer_size": len(self.data_buffer),
            "max_buffer_size": self.max_buffer_size,
            "is_collecting": self.is_collecting,
            "active_sources": len(self.source_handlers),
            "last_update": self.stats.last_update.isoformat()
        }
    
    def clear_buffer(self) -> int:
        """Clear data buffer and return number of cleared items"""
        cleared_count = len(self.data_buffer)
        self.data_buffer.clear()
        logger.info("Cleared %d items from data buffer", cleared_count)
        return cleared_count
    
    def get_data_quality_summary(self) -> Dict[str, Any]:
        """Get data quality summary"""
        quality_counts = {
            DataQuality.EXCELLENT: 0,
            DataQuality.GOOD: 0,
            DataQuality.FAIR: 0,
            DataQuality.POOR: 0,
            DataQuality.INVALID: 0
        }
        
        for data in self.data_buffer:
            quality_counts[data.quality] += 1
        
        total_data = len(self.data_buffer)
        quality_percentages = {}
        
        for quality, count in quality_counts.items():
            if total_data > 0:
                quality_percentages[quality.value] = (count / total_data) * 100
            else:
                quality_percentages[quality.value] = 0.0
        
        return {
            "total_data_points": total_data,
            "quality_distribution": quality_percentages,
            "overall_quality": self._calculate_overall_quality(quality_percentages)
        }
    
    def _calculate_overall_quality(self, quality_percentages: Dict[str, float]) -> str:
        """Calculate overall data quality"""
        excellent = quality_percentages.get("excellent", 0)
        good = quality_percentages.get("good", 0)
        fair = quality_percentages.get("fair", 0)
        
        if excellent >= 80:
            return "excellent"
        elif excellent + good >= 80:
            return "good"
        elif excellent + good + fair >= 80:
            return "fair"
        else:
            return "poor"


# Global telemetry data collector instance
telemetry_data_collector = TelemetryDataCollector()
