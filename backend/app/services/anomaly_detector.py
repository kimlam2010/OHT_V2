"""
Anomaly Detection Service for OHT-50
Advanced anomaly detection algorithms for telemetry data monitoring
Target: < 15ms detection latency for real-time anomaly detection
"""

import asyncio
import logging
import statistics
import time
import math
from typing import Dict, Any, List, Optional, Tuple, Union
from datetime import datetime, timezone, timedelta
from dataclasses import dataclass, field
from enum import Enum

logger = logging.getLogger(__name__)


class AnomalyType(Enum):
    """Types of anomalies"""
    STATISTICAL = "statistical"
    TREND = "trend"
    PATTERN = "pattern"
    THRESHOLD = "threshold"
    SEASONAL = "seasonal"
    CLUSTER = "cluster"
    ISOLATION = "isolation"


class AnomalySeverity(Enum):
    """Anomaly severity levels"""
    LOW = "low"
    MEDIUM = "medium"
    HIGH = "high"
    CRITICAL = "critical"


class DetectionMethod(Enum):
    """Anomaly detection methods"""
    Z_SCORE = "z_score"
    IQR = "iqr"
    ISOLATION_FOREST = "isolation_forest"
    ONE_CLASS_SVM = "one_class_svm"
    LSTM = "lstm"
    MOVING_AVERAGE = "moving_average"
    EXPONENTIAL_SMOOTHING = "exponential_smoothing"


@dataclass
class AnomalyConfig:
    """Configuration for anomaly detection"""
    detection_method: DetectionMethod
    window_size: int = 20
    threshold: float = 2.0
    sensitivity: float = 0.1
    enabled: bool = True
    min_data_points: int = 10
    lookback_periods: int = 5


@dataclass
class AnomalyResult:
    """Anomaly detection result"""
    is_anomaly: bool
    anomaly_score: float
    anomaly_type: AnomalyType
    detection_method: DetectionMethod
    severity: AnomalySeverity
    confidence: float
    processing_time_ms: float
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))
    metadata: Dict[str, Any] = field(default_factory=dict)


class AnomalyDetector:
    """
    Advanced anomaly detection service for real-time telemetry monitoring
    Supports multiple detection algorithms with configurable parameters
    """

    def __init__(self):
        self.detection_configs: Dict[DetectionMethod, AnomalyConfig] = {}
        self.data_history: Dict[str, List[float]] = {}
        self.anomaly_history: Dict[str, List[AnomalyResult]] = {}
        self.processing_times: List[float] = []
        self.detection_stats: Dict[DetectionMethod, Dict[str, Any]] = {}
        
        # Initialize default configurations
        self._initialize_default_configs()
        
        logger.info("AnomalyDetector initialized with %d detection methods", len(self.detection_configs))

    def _initialize_default_configs(self):
        """Initialize default detection configurations"""
        self.detection_configs = {
            DetectionMethod.Z_SCORE: AnomalyConfig(
                detection_method=DetectionMethod.Z_SCORE,
                window_size=20,
                threshold=2.0,
                sensitivity=0.1
            ),
            DetectionMethod.IQR: AnomalyConfig(
                detection_method=DetectionMethod.IQR,
                window_size=20,
                threshold=1.5,
                sensitivity=0.1
            ),
            DetectionMethod.MOVING_AVERAGE: AnomalyConfig(
                detection_method=DetectionMethod.MOVING_AVERAGE,
                window_size=30,
                threshold=2.5,
                sensitivity=0.1
            ),
            DetectionMethod.EXPONENTIAL_SMOOTHING: AnomalyConfig(
                detection_method=DetectionMethod.EXPONENTIAL_SMOOTHING,
                window_size=20,
                threshold=2.0,
                sensitivity=0.1
            ),
            DetectionMethod.ISOLATION_FOREST: AnomalyConfig(
                detection_method=DetectionMethod.ISOLATION_FOREST,
                window_size=50,
                threshold=0.1,
                sensitivity=0.1
            )
        }
        
        # Initialize statistics
        for method in self.detection_configs:
            self.detection_stats[method] = {
                "total_processed": 0,
                "anomalies_detected": 0,
                "false_positives": 0,
                "avg_processing_time_ms": 0.0,
                "last_updated": datetime.now(timezone.utc)
            }

    async def detect_anomaly(self, data_source: str, current_value: float,
                           detection_method: DetectionMethod,
                           custom_config: Optional[AnomalyConfig] = None) -> AnomalyResult:
        """
        Detect anomaly in current data point
        """
        start_time = time.perf_counter()
        
        config = custom_config or self.detection_configs.get(detection_method)
        if not config or not config.enabled:
            return AnomalyResult(
                is_anomaly=False,
                anomaly_score=0.0,
                anomaly_type=AnomalyType.STATISTICAL,
                detection_method=detection_method,
                severity=AnomalySeverity.LOW,
                confidence=0.0,
                processing_time_ms=0.0
            )

        try:
            # Update data history
            self._update_data_history(data_source, current_value)
            
            # Check if we have enough data
            if len(self.data_history[data_source]) < config.min_data_points:
                return AnomalyResult(
                    is_anomaly=False,
                    anomaly_score=0.0,
                    anomaly_type=AnomalyType.STATISTICAL,
                    detection_method=detection_method,
                    severity=AnomalySeverity.LOW,
                    confidence=0.0,
                    processing_time_ms=(time.perf_counter() - start_time) * 1000,
                    metadata={"error": "Insufficient data points"}
                )

            # Apply detection algorithm
            anomaly_score, confidence = await self._apply_detection_algorithm(
                data_source, current_value, detection_method, config
            )
            
            # Determine if anomaly and severity
            is_anomaly, severity = self._evaluate_anomaly(anomaly_score, config)
            
            # Create result
            result = AnomalyResult(
                is_anomaly=is_anomaly,
                anomaly_score=anomaly_score,
                anomaly_type=self._get_anomaly_type(detection_method),
                detection_method=detection_method,
                severity=severity,
                confidence=confidence,
                processing_time_ms=(time.perf_counter() - start_time) * 1000,
                metadata={
                    "data_source": data_source,
                    "current_value": current_value,
                    "config": config.__dict__
                }
            )
            
            # Update history and statistics
            self._update_anomaly_history(data_source, result)
            self._update_detection_stats(detection_method, result)
            
            if result.processing_time_ms > 15.0:  # Target latency
                logger.warning(f"Anomaly detection {detection_method.value} took {result.processing_time_ms:.2f}ms, exceeding 15ms target")
            
            return result
            
        except Exception as e:
            logger.error(f"Error detecting anomaly with {detection_method.value}: {e}")
            return AnomalyResult(
                is_anomaly=False,
                anomaly_score=0.0,
                anomaly_type=AnomalyType.STATISTICAL,
                detection_method=detection_method,
                severity=AnomalySeverity.LOW,
                confidence=0.0,
                processing_time_ms=(time.perf_counter() - start_time) * 1000,
                metadata={"error": str(e)}
            )

    async def _apply_detection_algorithm(self, data_source: str, current_value: float,
                                       detection_method: DetectionMethod,
                                       config: AnomalyConfig) -> Tuple[float, float]:
        """Apply specific detection algorithm"""
        
        if detection_method == DetectionMethod.Z_SCORE:
            return self._z_score_detection(data_source, current_value, config)
        elif detection_method == DetectionMethod.IQR:
            return self._iqr_detection(data_source, current_value, config)
        elif detection_method == DetectionMethod.MOVING_AVERAGE:
            return self._moving_average_detection(data_source, current_value, config)
        elif detection_method == DetectionMethod.EXPONENTIAL_SMOOTHING:
            return self._exponential_smoothing_detection(data_source, current_value, config)
        elif detection_method == DetectionMethod.ISOLATION_FOREST:
            return self._isolation_forest_detection(data_source, current_value, config)
        else:
            return 0.0, 0.0

    def _z_score_detection(self, data_source: str, current_value: float, 
                          config: AnomalyConfig) -> Tuple[float, float]:
        """Z-score based anomaly detection"""
        data_points = self.data_history[data_source][-config.window_size:]
        
        if len(data_points) < 2:
            return 0.0, 0.0
        
        mean_val = statistics.mean(data_points)
        std_val = statistics.stdev(data_points)
        
        if std_val == 0:
            return 0.0, 0.0
        
        z_score = abs(current_value - mean_val) / std_val
        confidence = min(1.0, z_score / config.threshold)
        
        return z_score, confidence

    def _iqr_detection(self, data_source: str, current_value: float, 
                      config: AnomalyConfig) -> Tuple[float, float]:
        """Interquartile Range based anomaly detection"""
        data_points = self.data_history[data_source][-config.window_size:]
        
        if len(data_points) < 4:
            return 0.0, 0.0
        
        sorted_data = sorted(data_points)
        n = len(sorted_data)
        
        q1_index = n // 4
        q3_index = 3 * n // 4
        
        q1 = sorted_data[q1_index]
        q3 = sorted_data[q3_index]
        iqr = q3 - q1
        
        if iqr == 0:
            return 0.0, 0.0
        
        # Calculate IQR score
        if current_value < q1:
            iqr_score = (q1 - current_value) / iqr
        elif current_value > q3:
            iqr_score = (current_value - q3) / iqr
        else:
            iqr_score = 0.0
        
        confidence = min(1.0, iqr_score / config.threshold)
        
        return iqr_score, confidence

    def _moving_average_detection(self, data_source: str, current_value: float, 
                                config: AnomalyConfig) -> Tuple[float, float]:
        """Moving average based anomaly detection"""
        data_points = self.data_history[data_source][-config.window_size:]
        
        if len(data_points) < config.window_size // 2:
            return 0.0, 0.0
        
        # Calculate moving average
        moving_avg = statistics.mean(data_points)
        
        # Calculate deviation from moving average
        deviation = abs(current_value - moving_avg)
        avg_deviation = statistics.mean([abs(x - moving_avg) for x in data_points])
        
        if avg_deviation == 0:
            return 0.0, 0.0
        
        anomaly_score = deviation / avg_deviation
        confidence = min(1.0, anomaly_score / config.threshold)
        
        return anomaly_score, confidence

    def _exponential_smoothing_detection(self, data_source: str, current_value: float, 
                                       config: AnomalyConfig) -> Tuple[float, float]:
        """Exponential smoothing based anomaly detection"""
        data_points = self.data_history[data_source][-config.window_size:]
        
        if len(data_points) < 2:
            return 0.0, 0.0
        
        # Simple exponential smoothing
        alpha = config.sensitivity
        smoothed = data_points[0]
        
        for value in data_points[1:]:
            smoothed = alpha * value + (1 - alpha) * smoothed
        
        # Calculate deviation from smoothed value
        deviation = abs(current_value - smoothed)
        avg_deviation = statistics.mean([abs(x - smoothed) for x in data_points])
        
        if avg_deviation == 0:
            return 0.0, 0.0
        
        anomaly_score = deviation / avg_deviation
        confidence = min(1.0, anomaly_score / config.threshold)
        
        return anomaly_score, confidence

    def _isolation_forest_detection(self, data_source: str, current_value: float, 
                                  config: AnomalyConfig) -> Tuple[float, float]:
        """Simplified isolation forest based anomaly detection"""
        data_points = self.data_history[data_source][-config.window_size:]
        
        if len(data_points) < 10:
            return 0.0, 0.0
        
        # Simplified isolation score calculation
        # In a real implementation, this would use a proper isolation forest algorithm
        
        # Calculate distance to nearest neighbors
        distances = [abs(current_value - x) for x in data_points]
        min_distance = min(distances)
        avg_distance = statistics.mean(distances)
        
        if avg_distance == 0:
            return 0.0, 0.0
        
        # Isolation score (higher = more isolated = more anomalous)
        isolation_score = min_distance / avg_distance
        confidence = min(1.0, isolation_score / config.threshold)
        
        return isolation_score, confidence

    def _evaluate_anomaly(self, anomaly_score: float, config: AnomalyConfig) -> Tuple[bool, AnomalySeverity]:
        """Evaluate if anomaly score indicates an anomaly and determine severity"""
        
        if anomaly_score < config.threshold:
            return False, AnomalySeverity.LOW
        
        # Determine severity based on score
        if anomaly_score < config.threshold * 1.5:
            return True, AnomalySeverity.LOW
        elif anomaly_score < config.threshold * 2.0:
            return True, AnomalySeverity.MEDIUM
        elif anomaly_score < config.threshold * 3.0:
            return True, AnomalySeverity.HIGH
        else:
            return True, AnomalySeverity.CRITICAL

    def _get_anomaly_type(self, detection_method: DetectionMethod) -> AnomalyType:
        """Get anomaly type based on detection method"""
        type_mapping = {
            DetectionMethod.Z_SCORE: AnomalyType.STATISTICAL,
            DetectionMethod.IQR: AnomalyType.STATISTICAL,
            DetectionMethod.MOVING_AVERAGE: AnomalyType.TREND,
            DetectionMethod.EXPONENTIAL_SMOOTHING: AnomalyType.TREND,
            DetectionMethod.ISOLATION_FOREST: AnomalyType.ISOLATION
        }
        return type_mapping.get(detection_method, AnomalyType.STATISTICAL)

    def _update_data_history(self, data_source: str, current_value: float):
        """Update data history for anomaly detection"""
        if data_source not in self.data_history:
            self.data_history[data_source] = []
        
        self.data_history[data_source].append(current_value)
        
        # Keep only recent data points
        max_history = 1000
        if len(self.data_history[data_source]) > max_history:
            self.data_history[data_source] = self.data_history[data_source][-max_history:]

    def _update_anomaly_history(self, data_source: str, result: AnomalyResult):
        """Update anomaly detection history"""
        if data_source not in self.anomaly_history:
            self.anomaly_history[data_source] = []
        
        self.anomaly_history[data_source].append(result)
        
        # Keep only recent anomalies
        max_history = 100
        if len(self.anomaly_history[data_source]) > max_history:
            self.anomaly_history[data_source] = self.anomaly_history[data_source][-max_history:]

    def _update_detection_stats(self, detection_method: DetectionMethod, result: AnomalyResult):
        """Update detection statistics"""
        stats = self.detection_stats[detection_method]
        
        stats["total_processed"] += 1
        if result.is_anomaly:
            stats["anomalies_detected"] += 1
        stats["last_updated"] = datetime.now(timezone.utc)
        
        # Update average processing time
        self.processing_times.append(result.processing_time_ms)
        if len(self.processing_times) > 1000:
            self.processing_times.pop(0)
        
        stats["avg_processing_time_ms"] = statistics.mean(self.processing_times)

    def configure_detection(self, detection_method: DetectionMethod, config: AnomalyConfig):
        """Configure anomaly detection parameters"""
        self.detection_configs[detection_method] = config
        logger.info(f"Configured detection {detection_method.value} with threshold={config.threshold}")

    def get_detection_stats(self, detection_method: Optional[DetectionMethod] = None) -> Dict[str, Any]:
        """Get detection statistics"""
        if detection_method:
            return self.detection_stats.get(detection_method, {})
        
        return {
            "all_detections": self.detection_stats,
            "total_processing_times": len(self.processing_times),
            "avg_processing_time_ms": statistics.mean(self.processing_times) if self.processing_times else 0.0,
            "data_sources": list(self.data_history.keys()),
            "total_data_sources": len(self.data_history)
        }

    def get_anomaly_history(self, data_source: str, limit: int = 10) -> List[AnomalyResult]:
        """Get anomaly detection history for data source"""
        return self.anomaly_history.get(data_source, [])[-limit:]

    def get_anomaly_summary(self) -> Dict[str, Any]:
        """Get anomaly detection summary"""
        summary = {
            "total_anomalies": 0,
            "anomalies_by_severity": {severity.value: 0 for severity in AnomalySeverity},
            "anomalies_by_type": {anomaly_type.value: 0 for anomaly_type in AnomalyType},
            "data_sources_with_anomalies": []
        }
        
        for source, anomalies in self.anomaly_history.items():
            source_anomalies = [a for a in anomalies if a.is_anomaly]
            if source_anomalies:
                summary["data_sources_with_anomalies"].append(source)
                summary["total_anomalies"] += len(source_anomalies)
                
                for anomaly in source_anomalies:
                    summary["anomalies_by_severity"][anomaly.severity.value] += 1
                    summary["anomalies_by_type"][anomaly.anomaly_type.value] += 1
        
        return summary

    async def batch_detect_anomalies(self, data_sources: List[str], 
                                   current_values: Dict[str, float],
                                   detection_method: DetectionMethod,
                                   custom_config: Optional[AnomalyConfig] = None) -> Dict[str, AnomalyResult]:
        """Detect anomalies for multiple data sources"""
        results = {}
        
        for source in data_sources:
            if source in current_values:
                result = await self.detect_anomaly(
                    source, 
                    current_values[source], 
                    detection_method, 
                    custom_config
                )
                results[source] = result
        
        return results

    def reset_detection_data(self, data_source: Optional[str] = None):
        """Reset detection data"""
        if data_source:
            if data_source in self.data_history:
                self.data_history[data_source] = []
            if data_source in self.anomaly_history:
                self.anomaly_history[data_source] = []
            logger.info(f"Reset detection data for source {data_source}")
        else:
            self.data_history.clear()
            self.anomaly_history.clear()
            logger.info("Reset all detection data")


# Global instance
anomaly_detector = AnomalyDetector()
