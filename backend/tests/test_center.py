from fastapi.testclient import TestClient


def test_center_mission_and_permission(client: TestClient):
    mission = {
        "mission_id": "m1",
        "mission_type": "move_to",
        "target_position": {"x": 1, "y": 2, "z": 0, "theta": 0},
    }
    resp = client.post("/api/v1/center/mission", json=mission)
    assert resp.status_code == 200
    body = resp.json()
    assert body.get("success") is True
    assert body.get("accepted") is True
    assert body.get("mission_id") == "m1"

    permission = {"action": "move", "resource": "robot", "context": {"zone": "A"}}
    resp = client.post("/api/v1/center/permission", json=permission)
    assert resp.status_code == 200
    body = resp.json()
    assert body.get("success") is True
    assert body.get("allowed") is True


