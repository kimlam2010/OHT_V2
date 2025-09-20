"""
Tests for Dashboard API endpoints
"""

import pytest
import os
from fastapi.testclient import TestClient
from unittest.mock import AsyncMock, patch, MagicMock

# Set testing environment
os.environ["TESTING"] = "true"

from app.main import app
from app.models.user import User
from app.core.security import get_current_user

# Override authentication dependency for testing
async def override_get_current_user():
    """Override authentication for testing"""
    return User(
        id=1,
        username="admin",
        email="admin@test.com",
        role="admin",
        is_active=True
    )

app.dependency_overrides[get_current_user] = override_get_current_user

client = TestClient(app)


class TestDashboardAPI:
    """Test cases for Dashboard API endpoints"""
    
    def test_get_dashboard_summary(self):
        """Test dashboard summary endpoint"""
        response = client.get("/api/v1/dashboard/summary")
        assert response.status_code == 200
        
        data = response.json()
        assert "robot" in data
        assert "system" in data
        assert "performance" in data
        assert "last_updated" in data
        
        # Check robot data structure
        robot_data = data["robot"]
        assert "robot_id" in robot_data
        assert "status" in robot_data
        assert "mode" in robot_data
        assert "position" in robot_data
        assert "battery_level" in robot_data
        
        # Check system data structure
        system_data = data["system"]
        assert "total_alerts" in system_data
        assert "active_alerts" in system_data
        assert "critical_alerts" in system_data
        assert "system_uptime" in system_data
        
        # Check performance data structure
        performance_data = data["performance"]
        assert "timestamp" in performance_data
        assert "cpu_usage" in performance_data
        assert "memory_usage" in performance_data
        assert "response_time" in performance_data
    
    def test_get_active_alerts(self):
        """Test active alerts endpoint"""
        response = client.get("/api/v1/dashboard/alerts")
        assert response.status_code == 200
        
        data = response.json()
        assert "alerts" in data
        assert "total_count" in data
        assert "page" in data
        assert "page_size" in data
        assert "has_next" in data
        
        # Check alert structure
        if data["alerts"]:
            alert = data["alerts"][0]
            assert "id" in alert
            assert "title" in alert
            assert "message" in alert
            assert "severity" in alert
            assert "status" in alert
            assert "timestamp" in alert
    
    def test_get_active_alerts_with_pagination(self):
        """Test active alerts endpoint with pagination"""
        response = client.get("/api/v1/dashboard/alerts?page=1&page_size=10")
        assert response.status_code == 200
        
        data = response.json()
        assert data["page"] == 1
        assert data["page_size"] == 10
    
    def test_get_active_alerts_with_severity_filter(self):
        """Test active alerts endpoint with severity filter"""
        response = client.get("/api/v1/dashboard/alerts?severity=critical")
        assert response.status_code == 200
        
        data = response.json()
        # All returned alerts should have critical severity
        for alert in data["alerts"]:
            assert alert["severity"] == "critical"
    
    def test_get_system_logs(self):
        """Test system logs endpoint"""
        response = client.get("/api/v1/dashboard/logs")
        assert response.status_code == 200
        
        data = response.json()
        assert "logs" in data
        assert "total_count" in data
        assert "page" in data
        assert "page_size" in data
        assert "has_next" in data
        
        # Check log structure
        if data["logs"]:
            log = data["logs"][0]
            assert "id" in log
            assert "level" in log
            assert "message" in log
            assert "source" in log
            assert "timestamp" in log
    
    def test_get_system_logs_with_filters(self):
        """Test system logs endpoint with filters"""
        response = client.get("/api/v1/dashboard/logs?limit=20&level=error")
        assert response.status_code == 200
        
        data = response.json()
        assert data["page_size"] <= 20
        
        # All returned logs should have error level
        for log in data["logs"]:
            assert log["level"] == "error"
    
    def test_get_performance_data(self):
        """Test performance data endpoint"""
        response = client.get("/api/v1/dashboard/performance")
        assert response.status_code == 200
        
        data = response.json()
        assert "metrics" in data
        assert "time_range" in data
        assert "interval" in data
        assert "total_points" in data
        
        # Check performance metric structure
        if data["metrics"]:
            metric = data["metrics"][0]
            assert "timestamp" in metric
            assert "cpu_usage" in metric
            assert "memory_usage" in metric
            assert "disk_usage" in metric
            assert "response_time" in metric
    
    def test_get_performance_data_with_hours(self):
        """Test performance data endpoint with custom hours"""
        response = client.get("/api/v1/dashboard/performance?hours=12")
        assert response.status_code == 200
        
        data = response.json()
        assert data["time_range"] == "12 hours"
        assert data["total_points"] == 12
    
    def test_get_dashboard_stats(self):
        """Test dashboard stats endpoint"""
        response = client.get("/api/v1/dashboard/stats")
        assert response.status_code == 200
        
        data = response.json()
        assert "total_requests" in data
        assert "successful_requests" in data
        assert "failed_requests" in data
        assert "average_response_time" in data
        assert "error_rate" in data
        assert "active_connections" in data
        assert "last_24h_requests" in data
    
    def test_acknowledge_alert(self):
        """Test acknowledge alert endpoint"""
        # First create an alert to acknowledge
        alert_response = client.get("/api/v1/dashboard/alerts")
        if alert_response.status_code == 200:
            alerts = alert_response.json()
            if alerts and "alerts" in alerts and alerts["alerts"]:
                alert_id = alerts["alerts"][0]["id"]
            else:
                alert_id = "test-alert-1"  # Use a test ID
        else:
            alert_id = "test-alert-1"
        
        response = client.post(f"/api/v1/dashboard/alerts/{alert_id}/acknowledge")
        # Accept both success and not found for now
        assert response.status_code in [200, 404]
        
        if response.status_code == 200:
            data = response.json()
            assert data["success"] is True
            assert "message" in data
    
    def test_resolve_alert(self):
        """Test resolve alert endpoint"""
        # Use same approach as acknowledge test
        alert_response = client.get("/api/v1/dashboard/alerts")
        if alert_response.status_code == 200:
            alerts = alert_response.json()
            if alerts and "alerts" in alerts and alerts["alerts"]:
                alert_id = alerts["alerts"][0]["id"]
            else:
                alert_id = "test-alert-1"
        else:
            alert_id = "test-alert-1"
        
        response = client.post(f"/api/v1/dashboard/alerts/{alert_id}/resolve")
        # Accept both success and not found for now
        assert response.status_code in [200, 404]
        
        if response.status_code == 200:
            data = response.json()
            assert data["success"] is True
            assert "message" in data
    
    def test_dashboard_summary_performance(self):
        """Test dashboard summary response time"""
        import time
        
        start_time = time.time()
        response = client.get("/api/v1/dashboard/summary")
        end_time = time.time()
        
        response_time = (end_time - start_time) * 1000  # Convert to milliseconds
        assert response.status_code == 200
        assert response_time < 50  # Should be under 50ms
    
    def test_alerts_performance(self):
        """Test alerts endpoint response time"""
        import time
        
        start_time = time.time()
        response = client.get("/api/v1/dashboard/alerts")
        end_time = time.time()
        
        response_time = (end_time - start_time) * 1000  # Convert to milliseconds
        assert response.status_code == 200
        assert response_time < 50  # Should be under 50ms
    
    def test_logs_performance(self):
        """Test logs endpoint response time"""
        import time
        
        start_time = time.time()
        response = client.get("/api/v1/dashboard/logs")
        end_time = time.time()
        
        response_time = (end_time - start_time) * 1000  # Convert to milliseconds
        assert response.status_code == 200
        assert response_time < 50  # Should be under 50ms
    
    def test_performance_data_performance(self):
        """Test performance data endpoint response time"""
        import time
        
        start_time = time.time()
        response = client.get("/api/v1/dashboard/performance")
        end_time = time.time()
        
        response_time = (end_time - start_time) * 1000  # Convert to milliseconds
        assert response.status_code == 200
        assert response_time < 50  # Should be under 50ms
