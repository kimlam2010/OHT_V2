import os
import pytest
os.environ["TESTING"] = "true"


@pytest.mark.asyncio
async def test_dashboard_safety_mapping_normal():
    # Ensure mock is enabled for deterministic test
    os.environ["USE_MOCK_FIRMWARE"] = "true"

    from app.main import app
    from app.core.security import get_current_user
    from app.models.user import User
    import httpx

    async def override_get_current_user():
        return User(id=1, username="admin", email="admin@test.com", role="administrator", is_active=True)

    app.dependency_overrides[get_current_user] = override_get_current_user

    headers = {"Authorization": "Bearer mock_token"}
    async with httpx.AsyncClient(app=app, base_url="http://test") as ac:
        resp = await ac.get("/api/v1/dashboard/safety", headers=headers)
    assert resp.status_code == 200
    data = resp.json()

    # Schema keys
    assert set(data.keys()) == {"safety_state", "emergency_status", "obstacles_present", "active_alerts_count"}

    # Values mapping
    assert data["safety_state"] in ["SAFE", "WARNING", "EMERGENCY", "UNAVAILABLE"]
    assert data["emergency_status"] in ["Normal", "E‑STOP", "Unknown (FW offline)"]
    assert data["obstacles_present"] in [True, False, None]
    assert isinstance(data["active_alerts_count"], int)


@pytest.mark.asyncio
async def test_dashboard_safety_fw_offline(monkeypatch):

    # Force firmware call to raise to simulate FW offline
    from app.services import safety as safety_module

    async def mock_get_robot_status():
        raise RuntimeError("FW offline")

    monkeypatch.setattr(safety_module.safety_service.firmware_service, "get_robot_status", mock_get_robot_status)

    from app.main import app
    from app.core.security import get_current_user
    from app.models.user import User
    import httpx

    async def override_get_current_user():
        return User(id=1, username="admin", email="admin@test.com", role="administrator", is_active=True)

    app.dependency_overrides[get_current_user] = override_get_current_user

    headers = {"Authorization": "Bearer mock_token"}
    async with httpx.AsyncClient(app=app, base_url="http://test") as ac:
        resp = await ac.get("/api/v1/dashboard/safety", headers=headers)
    assert resp.status_code == 200
    data = resp.json()

    assert data["safety_state"] == "UNAVAILABLE"
    assert data["emergency_status"] == "Unknown (FW offline)"
    assert data["obstacles_present"] is None
    assert data["active_alerts_count"] == 0


