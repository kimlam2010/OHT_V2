from fastapi.testclient import TestClient


def test_telemetry_current(client: TestClient):
    resp = client.get("/api/v1/telemetry/current")
    assert resp.status_code == 200
    body = resp.json()
    assert body.get("success") is True
    assert isinstance(body.get("data"), dict)


def test_telemetry_history_and_limits(client: TestClient):
    # default
    resp = client.get("/api/v1/telemetry/history")
    assert resp.status_code == 200
    data = resp.json()["data"]
    assert isinstance(data, list)
    # limit within range
    resp = client.get("/api/v1/telemetry/history?limit=5")
    assert resp.status_code == 200
    # limit out of range rejected by validation
    resp = client.get("/api/v1/telemetry/history?limit=0")
    assert resp.status_code == 422


def test_telemetry_stats(client: TestClient):
    resp = client.get("/api/v1/telemetry/stats")
    assert resp.status_code == 200
    body = resp.json()
    assert body.get("success") is True
    assert isinstance(body.get("data"), dict)


def test_telemetry_rate_set_and_get(client: TestClient):
    # set valid rate
    resp = client.post("/api/v1/telemetry/rate?hz=5.0")
    assert resp.status_code == 200
    body = resp.json()
    assert body.get("success") is True
    assert abs(body.get("hz") - 5.0) < 1e-6
    # out of range
    resp = client.post("/api/v1/telemetry/rate?hz=0.05")
    assert resp.status_code == 422


def test_telemetry_reset(client: TestClient):
    resp = client.post("/api/v1/telemetry/reset")
    assert resp.status_code == 200
    assert resp.json().get("success") is True


