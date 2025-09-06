"""
Data Aggregation Service for OHT-50
Advanced data aggregation methods for telemetry data processing
Target: < 10ms aggregation latency for real-time processing
"""

import asyncio
import logging
import statistics
import time
from typing import Dict, Any, List, Optional, Union, Tuple
from datetime import datetime, timezone, timedelta
from dataclasses import dataclass, field
from enum import Enum
import math

logger = logging.getLogger(__name__)


class AggregationType(Enum):
    """Types of data aggregation"""
    MEAN = "mean"
    MEDIAN = "median"
    MODE = "mode"
    SUM = "sum"
    COUNT = "count"
    MIN = "min"
    MAX = "max"
    STD_DEV = "std_dev"
    VARIANCE = "variance"
    PERCENTILE = "percentile"
    RANGE = "range"
    TREND = "trend"
    RATE = "rate"


class AggregationWindow(Enum):
    """Aggregation time windows"""
    SECOND = "1s"
    MINUTE = "1m"
    HOUR = "1h"
    DAY = "1d"
    CUSTOM = "custom"


@dataclass
class AggregationConfig:
    """Configuration for data aggregation"""
    aggregation_type: AggregationType
    window_size: int = 10
    time_window: AggregationWindow = AggregationWindow.SECOND
    percentile: float = 50.0  # For percentile aggregation
    enabled: bool = True
    min_data_points: int = 1


@dataclass
class AggregatedData:
    """Aggregated data result"""
    original_count: int
    aggregated_value: Union[float, int, Dict[str, Any]]
    aggregation_type: AggregationType
    window_size: int
    processing_time_ms: float
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))
    metadata: Dict[str, Any] = field(default_factory=dict)


class DataAggregator:
    """
    Advanced data aggregation service for real-time telemetry processing
    Supports multiple aggregation types with configurable time windows
    """

    def __init__(self):
        self.aggregation_configs: Dict[AggregationType, AggregationConfig] = {}
        self.data_buffers: Dict[str, List[Tuple[float, datetime]]] = {}
        self.aggregation_history: Dict[str, List[AggregatedData]] = {}
        self.processing_times: List[float] = []
        self.aggregation_stats: Dict[AggregationType, Dict[str, Any]] = {}
        
        # Initialize default configurations
        self._initialize_default_configs()
        
        logger.info("DataAggregator initialized with %d aggregation types", len(self.aggregation_configs))

    def _initialize_default_configs(self):
        """Initialize default aggregation configurations"""
        self.aggregation_configs = {
            AggregationType.MEAN: AggregationConfig(
                aggregation_type=AggregationType.MEAN,
                window_size=10,
                time_window=AggregationWindow.SECOND
            ),
            AggregationType.MEDIAN: AggregationConfig(
                aggregation_type=AggregationType.MEDIAN,
                window_size=10,
                time_window=AggregationWindow.SECOND
            ),
            AggregationType.SUM: AggregationConfig(
                aggregation_type=AggregationType.SUM,
                window_size=10,
                time_window=AggregationWindow.SECOND
            ),
            AggregationType.MIN: AggregationConfig(
                aggregation_type=AggregationType.MIN,
                window_size=10,
                time_window=AggregationWindow.SECOND
            ),
            AggregationType.MAX: AggregationConfig(
                aggregation_type=AggregationType.MAX,
                window_size=10,
                time_window=AggregationWindow.SECOND
            ),
            AggregationType.STD_DEV: AggregationConfig(
                aggregation_type=AggregationType.STD_DEV,
                window_size=20,
                time_window=AggregationWindow.SECOND
            ),
            AggregationType.PERCENTILE: AggregationConfig(
                aggregation_type=AggregationType.PERCENTILE,
                window_size=10,
                time_window=AggregationWindow.SECOND,
                percentile=95.0
            ),
            AggregationType.TREND: AggregationConfig(
                aggregation_type=AggregationType.TREND,
                window_size=30,
                time_window=AggregationWindow.SECOND
            ),
            AggregationType.RATE: AggregationConfig(
                aggregation_type=AggregationType.RATE,
                window_size=10,
                time_window=AggregationWindow.SECOND
            )
        }
        
        # Initialize statistics
        for agg_type in self.aggregation_configs:
            self.aggregation_stats[agg_type] = {
                "total_processed": 0,
                "avg_processing_time_ms": 0.0,
                "last_updated": datetime.now(timezone.utc)
            }

    async def aggregate_data(self, data_source: str, data_points: List[float], 
                           aggregation_type: AggregationType,
                           custom_config: Optional[AggregationConfig] = None) -> AggregatedData:
        """
        Aggregate data points using specified aggregation method
        """
        start_time = time.perf_counter()
        
        config = custom_config or self.aggregation_configs.get(aggregation_type)
        if not config or not config.enabled:
            return AggregatedData(
                original_count=len(data_points),
                aggregated_value=0.0,
                aggregation_type=aggregation_type,
                window_size=0,
                processing_time_ms=0.0
            )

        try:
            # Filter data points based on time window
            filtered_data = self._filter_data_by_time_window(data_points, config)
            
            if len(filtered_data) < config.min_data_points:
                return AggregatedData(
                    original_count=len(data_points),
                    aggregated_value=0.0,
                    aggregation_type=aggregation_type,
                    window_size=len(filtered_data),
                    processing_time_ms=(time.perf_counter() - start_time) * 1000,
                    metadata={"error": "Insufficient data points"}
                )

            # Apply aggregation algorithm
            aggregated_value = await self._apply_aggregation_algorithm(filtered_data, aggregation_type, config)
            
            # Update data buffers and history
            self._update_data_buffers(data_source, data_points)
            self._update_aggregation_history(data_source, aggregated_value, aggregation_type, config)
            
            # Update statistics
            processing_time_ms = (time.perf_counter() - start_time) * 1000
            self._update_aggregation_stats(aggregation_type, processing_time_ms)
            
            if processing_time_ms > 10.0:  # Target latency
                logger.warning(f"Aggregation {aggregation_type.value} processing took {processing_time_ms:.2f}ms, exceeding 10ms target")
            
            return AggregatedData(
                original_count=len(data_points),
                aggregated_value=aggregated_value,
                aggregation_type=aggregation_type,
                window_size=len(filtered_data),
                processing_time_ms=processing_time_ms,
                metadata={"data_source": data_source, "config": config.__dict__}
            )
            
        except Exception as e:
            logger.error(f"Error aggregating data with {aggregation_type.value}: {e}")
            return AggregatedData(
                original_count=len(data_points),
                aggregated_value=0.0,
                aggregation_type=aggregation_type,
                window_size=0,
                processing_time_ms=(time.perf_counter() - start_time) * 1000,
                metadata={"error": str(e)}
            )

    def _filter_data_by_time_window(self, data_points: List[float], 
                                  config: AggregationConfig) -> List[float]:
        """Filter data points based on time window"""
        if config.time_window == AggregationWindow.SECOND:
            # Use last N data points
            return data_points[-config.window_size:] if len(data_points) >= config.window_size else data_points
        elif config.time_window == AggregationWindow.MINUTE:
            # Simulate minute window (in real implementation, would use timestamps)
            return data_points[-config.window_size * 6:] if len(data_points) >= config.window_size * 6 else data_points
        else:
            return data_points[-config.window_size:] if len(data_points) >= config.window_size else data_points

    async def _apply_aggregation_algorithm(self, data_points: List[float], 
                                         aggregation_type: AggregationType,
                                         config: AggregationConfig) -> Union[float, int, Dict[str, Any]]:
        """Apply specific aggregation algorithm"""
        
        if aggregation_type == AggregationType.MEAN:
            return statistics.mean(data_points)
        elif aggregation_type == AggregationType.MEDIAN:
            return statistics.median(data_points)
        elif aggregation_type == AggregationType.SUM:
            return sum(data_points)
        elif aggregation_type == AggregationType.COUNT:
            return len(data_points)
        elif aggregation_type == AggregationType.MIN:
            return min(data_points)
        elif aggregation_type == AggregationType.MAX:
            return max(data_points)
        elif aggregation_type == AggregationType.STD_DEV:
            return statistics.stdev(data_points) if len(data_points) > 1 else 0.0
        elif aggregation_type == AggregationType.VARIANCE:
            return statistics.variance(data_points) if len(data_points) > 1 else 0.0
        elif aggregation_type == AggregationType.PERCENTILE:
            return self._calculate_percentile(data_points, config.percentile)
        elif aggregation_type == AggregationType.RANGE:
            return max(data_points) - min(data_points)
        elif aggregation_type == AggregationType.TREND:
            return self._calculate_trend(data_points)
        elif aggregation_type == AggregationType.RATE:
            return self._calculate_rate(data_points)
        else:
            return 0.0

    def _calculate_percentile(self, data_points: List[float], percentile: float) -> float:
        """Calculate percentile of data points"""
        if not data_points:
            return 0.0
        
        sorted_data = sorted(data_points)
        index = (percentile / 100.0) * (len(sorted_data) - 1)
        
        if index.is_integer():
            return sorted_data[int(index)]
        else:
            lower = sorted_data[int(index)]
            upper = sorted_data[int(index) + 1]
            return lower + (upper - lower) * (index - int(index))

    def _calculate_trend(self, data_points: List[float]) -> float:
        """Calculate trend (slope) of data points"""
        if len(data_points) < 2:
            return 0.0
        
        n = len(data_points)
        x_values = list(range(n))
        
        # Simple linear regression
        sum_x = sum(x_values)
        sum_y = sum(data_points)
        sum_xy = sum(x * y for x, y in zip(x_values, data_points))
        sum_x2 = sum(x * x for x in x_values)
        
        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x)
        return slope

    def _calculate_rate(self, data_points: List[float]) -> float:
        """Calculate rate of change"""
        if len(data_points) < 2:
            return 0.0
        
        # Calculate rate as difference between first and last values
        return data_points[-1] - data_points[0]

    def _update_data_buffers(self, data_source: str, data_points: List[float]):
        """Update data buffers for time-based aggregation"""
        current_time = datetime.now(timezone.utc)
        
        if data_source not in self.data_buffers:
            self.data_buffers[data_source] = []
        
        # Add data points with timestamps
        for data_point in data_points:
            self.data_buffers[data_source].append((data_point, current_time))
        
        # Clean old data (keep last 1000 points)
        if len(self.data_buffers[data_source]) > 1000:
            self.data_buffers[data_source] = self.data_buffers[data_source][-1000:]

    def _update_aggregation_history(self, data_source: str, aggregated_value: Union[float, int, Dict[str, Any]],
                                  aggregation_type: AggregationType, config: AggregationConfig):
        """Update aggregation history"""
        if data_source not in self.aggregation_history:
            self.aggregation_history[data_source] = []
        
        aggregated_data = AggregatedData(
            original_count=0,  # Will be updated by caller
            aggregated_value=aggregated_value,
            aggregation_type=aggregation_type,
            window_size=config.window_size,
            processing_time_ms=0.0,  # Will be updated by caller
            metadata={"data_source": data_source}
        )
        
        self.aggregation_history[data_source].append(aggregated_data)
        
        # Keep last 100 aggregations per source
        if len(self.aggregation_history[data_source]) > 100:
            self.aggregation_history[data_source] = self.aggregation_history[data_source][-100:]

    def _update_aggregation_stats(self, aggregation_type: AggregationType, processing_time_ms: float):
        """Update aggregation statistics"""
        stats = self.aggregation_stats[aggregation_type]
        
        stats["total_processed"] += 1
        stats["last_updated"] = datetime.now(timezone.utc)
        
        # Update average processing time
        self.processing_times.append(processing_time_ms)
        if len(self.processing_times) > 1000:
            self.processing_times.pop(0)
        
        stats["avg_processing_time_ms"] = statistics.mean(self.processing_times)

    def configure_aggregation(self, aggregation_type: AggregationType, config: AggregationConfig):
        """Configure aggregation parameters"""
        self.aggregation_configs[aggregation_type] = config
        logger.info(f"Configured aggregation {aggregation_type.value} with window_size={config.window_size}")

    def get_aggregation_stats(self, aggregation_type: Optional[AggregationType] = None) -> Dict[str, Any]:
        """Get aggregation statistics"""
        if aggregation_type:
            return self.aggregation_stats.get(aggregation_type, {})
        
        return {
            "all_aggregations": self.aggregation_stats,
            "total_processing_times": len(self.processing_times),
            "avg_processing_time_ms": statistics.mean(self.processing_times) if self.processing_times else 0.0,
            "data_sources": list(self.data_buffers.keys()),
            "total_data_buffers": len(self.data_buffers)
        }

    def get_aggregation_history(self, data_source: str, limit: int = 10) -> List[AggregatedData]:
        """Get aggregation history for data source"""
        return self.aggregation_history.get(data_source, [])[-limit:]

    def get_data_buffer_stats(self) -> Dict[str, Any]:
        """Get data buffer statistics"""
        buffer_stats = {}
        
        for source, buffer_data in self.data_buffers.items():
            if buffer_data:
                values = [point[0] for point in buffer_data]
                timestamps = [point[1] for point in buffer_data]
                
                buffer_stats[source] = {
                    "data_points": len(buffer_data),
                    "latest_value": values[-1] if values else 0.0,
                    "latest_timestamp": timestamps[-1].isoformat() if timestamps else None,
                    "value_range": max(values) - min(values) if values else 0.0,
                    "avg_value": statistics.mean(values) if values else 0.0
                }
        
        return buffer_stats

    async def batch_aggregate(self, data_sources: List[str], 
                            data_points_map: Dict[str, List[float]],
                            aggregation_type: AggregationType,
                            custom_config: Optional[AggregationConfig] = None) -> Dict[str, AggregatedData]:
        """Aggregate data for multiple sources"""
        results = {}
        
        for source in data_sources:
            if source in data_points_map:
                aggregated_data = await self.aggregate_data(
                    source, 
                    data_points_map[source], 
                    aggregation_type, 
                    custom_config
                )
                results[source] = aggregated_data
        
        return results

    def reset_aggregation_data(self, data_source: Optional[str] = None):
        """Reset aggregation data"""
        if data_source:
            if data_source in self.data_buffers:
                self.data_buffers[data_source] = []
            if data_source in self.aggregation_history:
                self.aggregation_history[data_source] = []
            logger.info(f"Reset aggregation data for source {data_source}")
        else:
            self.data_buffers.clear()
            self.aggregation_history.clear()
            logger.info("Reset all aggregation data")


# Global instance
data_aggregator = DataAggregator()
