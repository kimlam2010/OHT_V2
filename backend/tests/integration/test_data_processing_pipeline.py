"""
Integration tests for Data Processing Pipeline
Tests the complete data processing pipeline including filtering, aggregation, and anomaly detection
"""

import pytest
import asyncio
from datetime import datetime, timezone
from typing import List, Dict, Any

from app.services.data_filter import data_filter, FilterType, FilterConfig
from app.services.data_aggregator import data_aggregator, AggregationType, AggregationConfig
from app.services.anomaly_detector import anomaly_detector, DetectionMethod, AnomalyConfig
from app.services.performance_monitor import performance_monitor, MetricType, PerformanceConfig


class TestDataProcessingPipelineIntegration:
    """Integration tests for the complete data processing pipeline"""

    @pytest.fixture
    def sample_data(self):
        """Sample telemetry data for testing"""
        return {
            "motor_speed": [100, 105, 110, 108, 112, 115, 120, 118, 125, 130],
            "battery_level": [85, 84, 83, 82, 81, 80, 79, 78, 77, 76],
            "temperature": [45, 46, 47, 48, 49, 50, 51, 52, 53, 54],
            "lidar_distance": [1.5, 1.4, 1.3, 1.2, 1.1, 1.0, 0.9, 0.8, 0.7, 0.6]
        }

    @pytest.mark.asyncio
    async def test_complete_data_processing_pipeline(self, sample_data):
        """Test complete data processing pipeline"""
        # Step 1: Data Filtering
        filtered_results = {}
        for source, data_points in sample_data.items():
            filtered_data = await data_filter.batch_filter(
                data_points, FilterType.MOVING_AVERAGE
            )
            filtered_results[source] = [fd.filtered_value for fd in filtered_data]
        
        # Step 2: Data Aggregation
        aggregated_results = {}
        for source, filtered_data in filtered_results.items():
            aggregated_data = await data_aggregator.aggregate_data(
                source, filtered_data, AggregationType.MEAN
            )
            aggregated_results[source] = aggregated_data.aggregated_value
        
        # Step 3: Anomaly Detection
        anomaly_results = {}
        for source, current_value in aggregated_results.items():
            anomaly_result = await anomaly_detector.detect_anomaly(
                source, current_value, DetectionMethod.Z_SCORE
            )
            anomaly_results[source] = anomaly_result
        
        # Verify results
        assert len(filtered_results) == 4
        assert len(aggregated_results) == 4
        assert len(anomaly_results) == 4
        
        # Check that all processing completed successfully
        for source in sample_data.keys():
            assert source in filtered_results
            assert source in aggregated_results
            assert source in anomaly_results

    @pytest.mark.asyncio
    async def test_performance_monitoring_integration(self):
        """Test performance monitoring integration"""
        # Start monitoring
        await performance_monitor.start_monitoring()
        
        # Wait for some metrics to be collected
        await asyncio.sleep(0.5)
        
        # Get current metrics
        current_metrics = performance_monitor.get_current_metrics()
        
        # Get performance summary
        summary = performance_monitor.get_performance_summary()
        
        # Stop monitoring
        await performance_monitor.stop_monitoring()
        
        # Wait a bit for monitoring to fully stop
        await asyncio.sleep(0.1)
        
        # Get updated summary after stopping
        final_summary = performance_monitor.get_performance_summary()
        
        # Verify monitoring worked
        assert final_summary["monitoring_active"] is False
        assert "total_metrics" in summary
        assert "enabled_metrics" in summary

    @pytest.mark.asyncio
    async def test_data_filtering_performance(self, sample_data):
        """Test data filtering performance"""
        motor_speeds = sample_data["motor_speed"]
        
        # Test different filter types
        filter_types = [
            FilterType.LOW_PASS,
            FilterType.MOVING_AVERAGE,
            FilterType.MEDIAN,
            FilterType.OUTLIER_REMOVAL
        ]
        
        for filter_type in filter_types:
            start_time = datetime.now(timezone.utc)
            
            filtered_data = await data_filter.batch_filter(motor_speeds, filter_type)
            
            end_time = datetime.now(timezone.utc)
            processing_time = (end_time - start_time).total_seconds() * 1000
            
            # Verify performance target (< 5ms per filter)
            assert processing_time < 50.0  # Allow some margin for batch processing
            assert len(filtered_data) == len(motor_speeds)
            
            # Verify all filtered values are valid
            for fd in filtered_data:
                assert isinstance(fd.filtered_value, (int, float))
                assert fd.processing_time_ms < 5.0  # Individual processing time

    @pytest.mark.asyncio
    async def test_data_aggregation_performance(self, sample_data):
        """Test data aggregation performance"""
        motor_speeds = sample_data["motor_speed"]
        
        # Test different aggregation types
        aggregation_types = [
            AggregationType.MEAN,
            AggregationType.MEDIAN,
            AggregationType.SUM,
            AggregationType.STD_DEV
        ]
        
        for agg_type in aggregation_types:
            start_time = datetime.now(timezone.utc)
            
            aggregated_data = await data_aggregator.aggregate_data(
                "motor_speed", motor_speeds, agg_type
            )
            
            end_time = datetime.now(timezone.utc)
            processing_time = (end_time - start_time).total_seconds() * 1000
            
            # Verify performance target (< 10ms per aggregation)
            assert processing_time < 100.0  # Allow some margin
            assert aggregated_data.processing_time_ms < 10.0
            
            # Verify aggregated value is valid
            assert isinstance(aggregated_data.aggregated_value, (int, float))

    @pytest.mark.asyncio
    async def test_anomaly_detection_performance(self, sample_data):
        """Test anomaly detection performance"""
        motor_speeds = sample_data["motor_speed"]
        
        # Test different detection methods
        detection_methods = [
            DetectionMethod.Z_SCORE,
            DetectionMethod.IQR,
            DetectionMethod.MOVING_AVERAGE
        ]
        
        for detection_method in detection_methods:
            start_time = datetime.now(timezone.utc)
            
            # Test anomaly detection on each data point
            anomaly_results = []
            for speed in motor_speeds:
                result = await anomaly_detector.detect_anomaly(
                    "motor_speed", speed, detection_method
                )
                anomaly_results.append(result)
            
            end_time = datetime.now(timezone.utc)
            processing_time = (end_time - start_time).total_seconds() * 1000
            
            # Verify performance target (< 15ms per detection)
            assert processing_time < 150.0  # Allow some margin for batch processing
            assert len(anomaly_results) == len(motor_speeds)
            
            # Verify all results are valid
            for result in anomaly_results:
                assert isinstance(result.anomaly_score, (int, float))
                assert result.processing_time_ms < 15.0

    @pytest.mark.asyncio
    async def test_end_to_end_data_flow(self, sample_data):
        """Test end-to-end data flow through the pipeline"""
        # Simulate real-time data processing
        results = []
        
        for i in range(len(sample_data["motor_speed"])):
            # Get current data point
            current_speed = sample_data["motor_speed"][i]
            current_battery = sample_data["battery_level"][i]
            
            # Step 1: Filter data
            filtered_speed = await data_filter.apply_filter(
                current_speed, FilterType.MOVING_AVERAGE
            )
            filtered_battery = await data_filter.apply_filter(
                current_battery, FilterType.LOW_PASS
            )
            
            # Step 2: Aggregate recent data
            recent_speeds = sample_data["motor_speed"][:i+1]
            recent_batteries = sample_data["battery_level"][:i+1]
            
            aggregated_speed = await data_aggregator.aggregate_data(
                "motor_speed", recent_speeds, AggregationType.MEAN
            )
            aggregated_battery = await data_aggregator.aggregate_data(
                "battery_level", recent_batteries, AggregationType.MEAN
            )
            
            # Step 3: Detect anomalies
            speed_anomaly = await anomaly_detector.detect_anomaly(
                "motor_speed", current_speed, DetectionMethod.Z_SCORE
            )
            battery_anomaly = await anomaly_detector.detect_anomaly(
                "battery_level", current_battery, DetectionMethod.IQR
            )
            
            # Store results
            results.append({
                "timestamp": datetime.now(timezone.utc),
                "original_speed": current_speed,
                "filtered_speed": filtered_speed.filtered_value,
                "aggregated_speed": aggregated_speed.aggregated_value,
                "speed_anomaly": speed_anomaly.is_anomaly,
                "speed_anomaly_score": speed_anomaly.anomaly_score,
                "original_battery": current_battery,
                "filtered_battery": filtered_battery.filtered_value,
                "aggregated_battery": aggregated_battery.aggregated_value,
                "battery_anomaly": battery_anomaly.is_anomaly,
                "battery_anomaly_score": battery_anomaly.anomaly_score
            })
        
        # Verify end-to-end processing
        assert len(results) == len(sample_data["motor_speed"])
        
        # Check that all processing steps completed
        for result in results:
            assert "original_speed" in result
            assert "filtered_speed" in result
            assert "aggregated_speed" in result
            assert "speed_anomaly" in result
            assert "speed_anomaly_score" in result

    @pytest.mark.asyncio
    async def test_error_handling_in_pipeline(self):
        """Test error handling in the data processing pipeline"""
        # Test with invalid data
        invalid_data = [None, "invalid", float('inf'), float('-inf')]
        
        # Test filtering with invalid data
        try:
            filtered_data = await data_filter.batch_filter(invalid_data, FilterType.MOVING_AVERAGE)
            # Should handle gracefully
            assert len(filtered_data) == len(invalid_data)
        except Exception as e:
            # Should not crash the system
            assert isinstance(e, (ValueError, TypeError))
        
        # Test aggregation with empty data
        try:
            aggregated_data = await data_aggregator.aggregate_data(
                "test_source", [], AggregationType.MEAN
            )
            # Should handle gracefully
            assert aggregated_data.aggregated_value == 0.0
        except Exception as e:
            # Should not crash the system
            assert isinstance(e, (ValueError, IndexError))
        
        # Test anomaly detection with insufficient data
        try:
            anomaly_result = await anomaly_detector.detect_anomaly(
                "new_source", 100.0, DetectionMethod.Z_SCORE
            )
            # Should handle gracefully
            assert anomaly_result.is_anomaly is False
        except Exception as e:
            # Should not crash the system
            assert isinstance(e, (ValueError, IndexError))

    @pytest.mark.asyncio
    async def test_pipeline_performance_under_load(self):
        """Test pipeline performance under load"""
        # Generate large dataset
        large_dataset = {
            "motor_speed": list(range(1000)),
            "battery_level": [100 - i * 0.01 for i in range(1000)],
            "temperature": [20 + i * 0.01 for i in range(1000)]
        }
        
        start_time = datetime.now(timezone.utc)
        
        # Process large dataset
        for source, data_points in large_dataset.items():
            # Filter
            filtered_data = await data_filter.batch_filter(
                data_points[:100], FilterType.MOVING_AVERAGE  # Process in batches
            )
            
            # Aggregate
            aggregated_data = await data_aggregator.aggregate_data(
                source, data_points[:100], AggregationType.MEAN
            )
            
            # Detect anomalies on sample points
            for i in range(0, min(100, len(data_points)), 10):
                await anomaly_detector.detect_anomaly(
                    source, data_points[i], DetectionMethod.Z_SCORE
                )
        
        end_time = datetime.now(timezone.utc)
        total_processing_time = (end_time - start_time).total_seconds() * 1000
        
        # Verify performance under load
        assert total_processing_time < 5000.0  # Should complete within 5 seconds
        
        # Verify all components are still functional
        filter_stats = data_filter.get_filter_stats()
        agg_stats = data_aggregator.get_aggregation_stats()
        anomaly_stats = anomaly_detector.get_detection_stats()
        
        assert "total_processing_times" in filter_stats
        assert "total_processing_times" in agg_stats
        assert "total_processing_times" in anomaly_stats
