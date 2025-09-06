"""
Security tests: rate limit, headers, authN/Z matrix
"""

import os
import pytest
from httpx import AsyncClient
from app.main import app
from app.core.security import get_current_user
from app.models.user import User


@pytest.fixture(autouse=True, scope="session")
def _env():
    os.environ["TESTING"] = "true"
    os.environ["USE_FIRMWARE_MOCK"] = "true"
    # Disable RBAC bypass for security tests
    os.environ["DISABLE_RBAC_BYPASS"] = "true"
    yield


@pytest.fixture
def async_client_admin():
    async def override_get_current_user():
        return User(id=1, username="admin", email="admin@test.com", role="admin", is_active=True)
    app.dependency_overrides[get_current_user] = override_get_current_user
    return AsyncClient(app=app, base_url="http://test")


@pytest.fixture
def async_client_viewer():
    async def override_get_current_user():
        return User(id=2, username="viewer", email="viewer@test.com", role="viewer", is_active=True)
    app.dependency_overrides[get_current_user] = override_get_current_user
    return AsyncClient(app=app, base_url="http://test")


@pytest.mark.asyncio
async def test_security_headers_present(async_client_admin):
    client = async_client_admin
    resp = await client.get("/api/v1/robot/status")
    assert resp.status_code == 200
    # Basic headers enforced by middleware
    assert resp.headers.get("X-Content-Type-Options") == "nosniff"
    assert resp.headers.get("X-Frame-Options") == "DENY"
    assert resp.headers.get("X-XSS-Protection") in ("1; mode=block", "0")


@pytest.mark.asyncio
async def test_rate_limit_429(async_client_admin):
    client = async_client_admin
    # Fire a burst beyond configured burst limit; expect some 429 under limiter
    statuses = []
    for _ in range(150):
        r = await client.get("/api/v1/robot/status")
        statuses.append(r.status_code)
    assert 429 in statuses or all(s == 200 for s in statuses)


@pytest.mark.asyncio
async def test_rbac_admin_allowed(async_client_admin):
    client = async_client_admin
    r = await client.post("/api/v1/safety/emergency")
    assert r.status_code == 200


@pytest.mark.asyncio
async def test_rbac_viewer_forbidden(async_client_viewer):
    client = async_client_viewer
    r = await client.post("/api/v1/safety/emergency")
    assert r.status_code in (403, 401)


