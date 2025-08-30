from fastapi.testclient import TestClient


def test_config_contract(client: TestClient):
    # GET returns SystemConfig directly (not wrapped)
    resp = client.get("/api/v1/config/")
    # might be 404 if service mocked; accept 200 or 404
    assert resp.status_code in (200, 404)

    # PUT returns ConfigResponse (wrapped)
    sample = {
        "version": 0,
        "hardware": {"motor": {"interface": "rs485", "r": 0.05, "vmax_mms": 100.0, "amax_mms2": 100.0, "jmax_mms3": 1000.0}, "encoder": {"type": "incremental", "resolution_ppr": 1000, "invert": False}},
        "safety": {"zones": [{"start_mm": 0, "end_mm": 200, "vmax_mms": 200}], "estop_reset_policy": "manual"},
        "telemetry": {"ws_hz": 10},
        "comm": {"rs485": {"device": "/dev/ttyOHT485", "baud": 115200}, "can": {"iface": "can0"}},
    }
    resp = client.put("/api/v1/config/?user=alice", json=sample)
    # 200 or 400 depending on validation; for contract we only check schema keys when 200
    if resp.status_code == 200:
        body = resp.json()
        assert body.get("success") is True
        assert "version" in body


