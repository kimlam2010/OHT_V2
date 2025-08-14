"""
Tests cho health endpoints
"""

import pytest
from fastapi.testclient import TestClient


def test_health_basic(client: TestClient):
    """Test health endpoint cơ bản"""
    response = client.get("/health")
    assert response.status_code == 200
    data = response.json()
    assert data["status"] == "healthy"
    assert "timestamp" in data
    assert "version" in data


def test_health_detailed(client: TestClient):
    """Test health endpoint chi tiết"""
    response = client.get("/health/detailed")
    assert response.status_code == 200
    data = response.json()

    # Kiểm tra các trường cơ bản
    assert data["status"] == "healthy"
    assert "timestamp" in data
    assert "version" in data

    # Kiểm tra system metrics
    assert "system" in data
    system = data["system"]
    assert "cpu_percent" in system
    assert "memory_percent" in system
    assert "disk_percent" in system
    assert "disk_free_gb" in system

    # Kiểm tra service status
    assert "services" in data
    services = data["services"]
    assert "database" in services
    assert "center_communication" in services


def test_health_ready(client: TestClient):
    """Test readiness probe"""
    response = client.get("/health/ready")
    assert response.status_code == 200
    data = response.json()
    assert data["status"] == "ready"


def test_health_live(client: TestClient):
    """Test liveness probe"""
    response = client.get("/health/live")
    assert response.status_code == 200
    data = response.json()
    assert data["status"] == "alive"


def test_health_metrics_format(client: TestClient):
    """Test format của metrics"""
    response = client.get("/health/detailed")
    assert response.status_code == 200
    data = response.json()

    # Kiểm tra CPU metrics
    cpu = data["system"]["cpu_percent"]
    assert isinstance(cpu, (int, float))
    assert 0 <= cpu <= 100

    # Kiểm tra memory metrics
    memory = data["system"]["memory_percent"]
    assert isinstance(memory, (int, float))
    assert 0 <= memory <= 100

    # Kiểm tra disk metrics
    disk_percent = data["system"]["disk_percent"]
    assert isinstance(disk_percent, (int, float))
    assert 0 <= disk_percent <= 100

    disk_free = data["system"]["disk_free_gb"]
    assert isinstance(disk_free, (int, float))
    assert disk_free >= 0


def test_health_service_status(client: TestClient):
    """Test status của các services"""
    response = client.get("/health/detailed")
    assert response.status_code == 200
    data = response.json()

    services = data["services"]

    # Kiểm tra database status
    db_status = services["database"]
    assert isinstance(db_status, str)
    assert db_status in ["healthy", "unhealthy"]

    # Kiểm tra center communication status
    comm_status = services["center_communication"]
    assert isinstance(comm_status, str)
    assert comm_status in ["healthy", "unhealthy"]


def test_health_error_handling(client: TestClient):
    """Test error handling của health endpoints"""
    # Test với method không hợp lệ
    response = client.post("/health")
    assert response.status_code == 405

    # Test với endpoint không tồn tại
    response = client.get("/health/invalid")
    assert response.status_code == 404
