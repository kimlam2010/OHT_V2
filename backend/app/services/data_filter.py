"""
Data Filtering Service for OHT-50
Advanced filtering algorithms for telemetry data processing
Target: < 5ms filtering latency for real-time processing
"""

import asyncio
import logging
import statistics
import time
from typing import Dict, Any, List, Optional, Callable, Union
from datetime import datetime, timezone, timedelta
from dataclasses import dataclass, field
from enum import Enum
import numpy as np

logger = logging.getLogger(__name__)


class FilterType(Enum):
    """Types of data filters"""
    LOW_PASS = "low_pass"
    HIGH_PASS = "high_pass"
    BAND_PASS = "band_pass"
    MEDIAN = "median"
    MOVING_AVERAGE = "moving_average"
    OUTLIER_REMOVAL = "outlier_removal"
    NOISE_REDUCTION = "noise_reduction"
    TREND_FILTER = "trend_filter"


class FilterQuality(Enum):
    """Filter quality levels"""
    EXCELLENT = "excellent"
    GOOD = "good"
    FAIR = "fair"
    POOR = "poor"


@dataclass
class FilterConfig:
    """Configuration for data filters"""
    filter_type: FilterType
    window_size: int = 10
    cutoff_frequency: float = 0.1
    threshold: float = 2.0
    alpha: float = 0.1  # Smoothing factor
    enabled: bool = True
    quality_threshold: float = 0.8


@dataclass
class FilteredData:
    """Filtered data result"""
    original_value: float
    filtered_value: float
    filter_type: FilterType
    quality_score: float
    processing_time_ms: float
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))


class DataFilter:
    """
    Advanced data filtering service for real-time telemetry processing
    Supports multiple filter types with configurable parameters
    """

    def __init__(self):
        self.filter_configs: Dict[FilterType, FilterConfig] = {}
        self.filter_history: Dict[FilterType, List[float]] = {}
        self.processing_times: List[float] = []
        self.filter_stats: Dict[FilterType, Dict[str, Any]] = {}
        
        # Initialize default filter configurations
        self._initialize_default_configs()
        
        logger.info("DataFilter initialized with %d filter types", len(self.filter_configs))

    def _initialize_default_configs(self):
        """Initialize default filter configurations"""
        self.filter_configs = {
            FilterType.LOW_PASS: FilterConfig(
                filter_type=FilterType.LOW_PASS,
                window_size=10,
                cutoff_frequency=0.1,
                alpha=0.1
            ),
            FilterType.HIGH_PASS: FilterConfig(
                filter_type=FilterType.HIGH_PASS,
                window_size=10,
                cutoff_frequency=0.1,
                alpha=0.1
            ),
            FilterType.MEDIAN: FilterConfig(
                filter_type=FilterType.MEDIAN,
                window_size=5,
                threshold=2.0
            ),
            FilterType.MOVING_AVERAGE: FilterConfig(
                filter_type=FilterType.MOVING_AVERAGE,
                window_size=10,
                alpha=0.1
            ),
            FilterType.OUTLIER_REMOVAL: FilterConfig(
                filter_type=FilterType.OUTLIER_REMOVAL,
                window_size=20,
                threshold=2.5
            ),
            FilterType.NOISE_REDUCTION: FilterConfig(
                filter_type=FilterType.NOISE_REDUCTION,
                window_size=15,
                alpha=0.05
            ),
            FilterType.TREND_FILTER: FilterConfig(
                filter_type=FilterType.TREND_FILTER,
                window_size=30,
                alpha=0.2
            )
        }
        
        # Initialize filter history
        for filter_type in self.filter_configs:
            self.filter_history[filter_type] = []
            self.filter_stats[filter_type] = {
                "total_processed": 0,
                "avg_processing_time_ms": 0.0,
                "quality_scores": [],
                "last_updated": datetime.now(timezone.utc)
            }

    async def apply_filter(self, data: float, filter_type: FilterType, 
                          custom_config: Optional[FilterConfig] = None) -> FilteredData:
        """
        Apply specified filter to data point
        """
        start_time = time.perf_counter()
        
        config = custom_config or self.filter_configs.get(filter_type)
        if not config or not config.enabled:
            return FilteredData(
                original_value=data,
                filtered_value=data,
                filter_type=filter_type,
                quality_score=1.0,
                processing_time_ms=0.0
            )

        try:
            # Apply filter based on type
            filtered_value = await self._apply_filter_algorithm(data, filter_type, config)
            
            # Calculate quality score
            quality_score = self._calculate_quality_score(data, filtered_value, filter_type)
            
            # Update filter history
            self._update_filter_history(filter_type, data, filtered_value)
            
            # Update statistics
            processing_time_ms = (time.perf_counter() - start_time) * 1000
            self._update_filter_stats(filter_type, processing_time_ms, quality_score)
            
            if processing_time_ms > 5.0:  # Target latency
                logger.warning(f"Filter {filter_type.value} processing took {processing_time_ms:.2f}ms, exceeding 5ms target")
            
            return FilteredData(
                original_value=data,
                filtered_value=filtered_value,
                filter_type=filter_type,
                quality_score=quality_score,
                processing_time_ms=processing_time_ms
            )
            
        except Exception as e:
            logger.error(f"Error applying filter {filter_type.value}: {e}")
            return FilteredData(
                original_value=data,
                filtered_value=data,
                filter_type=filter_type,
                quality_score=0.0,
                processing_time_ms=(time.perf_counter() - start_time) * 1000
            )

    async def _apply_filter_algorithm(self, data: float, filter_type: FilterType, 
                                    config: FilterConfig) -> float:
        """Apply specific filter algorithm"""
        
        if filter_type == FilterType.LOW_PASS:
            return await self._low_pass_filter(data, config)
        elif filter_type == FilterType.HIGH_PASS:
            return await self._high_pass_filter(data, config)
        elif filter_type == FilterType.MEDIAN:
            return await self._median_filter(data, config)
        elif filter_type == FilterType.MOVING_AVERAGE:
            return await self._moving_average_filter(data, config)
        elif filter_type == FilterType.OUTLIER_REMOVAL:
            return await self._outlier_removal_filter(data, config)
        elif filter_type == FilterType.NOISE_REDUCTION:
            return await self._noise_reduction_filter(data, config)
        elif filter_type == FilterType.TREND_FILTER:
            return await self._trend_filter(data, config)
        else:
            return data

    async def _low_pass_filter(self, data: float, config: FilterConfig) -> float:
        """Low-pass filter implementation"""
        history = self.filter_history[FilterType.LOW_PASS]
        
        if not history:
            return data
        
        # Simple exponential moving average
        filtered = config.alpha * data + (1 - config.alpha) * history[-1]
        return filtered

    async def _high_pass_filter(self, data: float, config: FilterConfig) -> float:
        """High-pass filter implementation"""
        history = self.filter_history[FilterType.HIGH_PASS]
        
        if len(history) < 2:
            return data
        
        # Simple high-pass filter
        filtered = config.alpha * (data - history[-1]) + (1 - config.alpha) * history[-1]
        return filtered

    async def _median_filter(self, data: float, config: FilterConfig) -> float:
        """Median filter implementation"""
        history = self.filter_history[FilterType.MEDIAN]
        
        if len(history) < config.window_size:
            return data
        
        # Get median of recent values
        recent_values = history[-config.window_size:] + [data]
        return statistics.median(recent_values)

    async def _moving_average_filter(self, data: float, config: FilterConfig) -> float:
        """Moving average filter implementation"""
        history = self.filter_history[FilterType.MOVING_AVERAGE]
        
        if len(history) < config.window_size:
            return data
        
        # Calculate moving average
        recent_values = history[-config.window_size:] + [data]
        return statistics.mean(recent_values)

    async def _outlier_removal_filter(self, data: float, config: FilterConfig) -> float:
        """Outlier removal filter implementation"""
        history = self.filter_history[FilterType.OUTLIER_REMOVAL]
        
        if len(history) < config.window_size:
            return data
        
        # Calculate mean and standard deviation
        recent_values = history[-config.window_size:]
        mean_val = statistics.mean(recent_values)
        std_val = statistics.stdev(recent_values) if len(recent_values) > 1 else 0
        
        # Check if data point is outlier
        if std_val > 0 and abs(data - mean_val) > config.threshold * std_val:
            return mean_val  # Replace outlier with mean
        
        return data

    async def _noise_reduction_filter(self, data: float, config: FilterConfig) -> float:
        """Noise reduction filter implementation"""
        history = self.filter_history[FilterType.NOISE_REDUCTION]
        
        if not history:
            return data
        
        # Weighted average with noise reduction
        filtered = config.alpha * data + (1 - config.alpha) * history[-1]
        return filtered

    async def _trend_filter(self, data: float, config: FilterConfig) -> float:
        """Trend filter implementation"""
        history = self.filter_history[FilterType.TREND_FILTER]
        
        if len(history) < config.window_size:
            return data
        
        # Calculate trend and apply smoothing
        recent_values = history[-config.window_size:]
        trend = (recent_values[-1] - recent_values[0]) / len(recent_values)
        filtered = data + config.alpha * trend
        return filtered

    def _calculate_quality_score(self, original: float, filtered: float, 
                               filter_type: FilterType) -> float:
        """Calculate quality score for filtered data"""
        if original == 0:
            return 1.0 if filtered == 0 else 0.0
        
        # Calculate relative error
        relative_error = abs(original - filtered) / abs(original)
        
        # Convert to quality score (0-1, higher is better)
        quality_score = max(0.0, 1.0 - relative_error)
        
        return quality_score

    def _update_filter_history(self, filter_type: FilterType, original: float, filtered: float):
        """Update filter history"""
        history = self.filter_history[filter_type]
        config = self.filter_configs[filter_type]
        
        # Add filtered value to history
        history.append(filtered)
        
        # Maintain window size
        if len(history) > config.window_size * 2:
            history.pop(0)

    def _update_filter_stats(self, filter_type: FilterType, processing_time_ms: float, 
                           quality_score: float):
        """Update filter statistics"""
        stats = self.filter_stats[filter_type]
        
        stats["total_processed"] += 1
        stats["quality_scores"].append(quality_score)
        stats["last_updated"] = datetime.now(timezone.utc)
        
        # Update average processing time
        self.processing_times.append(processing_time_ms)
        if len(self.processing_times) > 1000:
            self.processing_times.pop(0)
        
        stats["avg_processing_time_ms"] = statistics.mean(self.processing_times)

    def configure_filter(self, filter_type: FilterType, config: FilterConfig):
        """Configure filter parameters"""
        self.filter_configs[filter_type] = config
        logger.info(f"Configured filter {filter_type.value} with window_size={config.window_size}")

    def get_filter_stats(self, filter_type: Optional[FilterType] = None) -> Dict[str, Any]:
        """Get filter statistics"""
        if filter_type:
            return self.filter_stats.get(filter_type, {})
        
        return {
            "all_filters": self.filter_stats,
            "total_processing_times": len(self.processing_times),
            "avg_processing_time_ms": statistics.mean(self.processing_times) if self.processing_times else 0.0
        }

    def get_filter_quality_summary(self) -> Dict[str, Any]:
        """Get filter quality summary"""
        quality_summary = {}
        
        for filter_type, stats in self.filter_stats.items():
            if stats["quality_scores"]:
                avg_quality = statistics.mean(stats["quality_scores"])
                quality_summary[filter_type.value] = {
                    "average_quality": avg_quality,
                    "quality_level": self._get_quality_level(avg_quality),
                    "total_processed": stats["total_processed"]
                }
        
        return quality_summary

    def _get_quality_level(self, quality_score: float) -> FilterQuality:
        """Get quality level from score"""
        if quality_score >= 0.9:
            return FilterQuality.EXCELLENT
        elif quality_score >= 0.7:
            return FilterQuality.GOOD
        elif quality_score >= 0.5:
            return FilterQuality.FAIR
        else:
            return FilterQuality.POOR

    async def batch_filter(self, data_points: List[float], filter_type: FilterType,
                          custom_config: Optional[FilterConfig] = None) -> List[FilteredData]:
        """Apply filter to batch of data points"""
        results = []
        
        for data_point in data_points:
            filtered_data = await self.apply_filter(data_point, filter_type, custom_config)
            results.append(filtered_data)
        
        return results

    def reset_filter_history(self, filter_type: Optional[FilterType] = None):
        """Reset filter history"""
        if filter_type:
            self.filter_history[filter_type] = []
            logger.info(f"Reset history for filter {filter_type.value}")
        else:
            for ft in self.filter_history:
                self.filter_history[ft] = []
            logger.info("Reset history for all filters")


# Global instance
data_filter = DataFilter()
