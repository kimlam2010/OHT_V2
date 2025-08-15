from fastapi.testclient import TestClient
from app.main import app


def main() -> None:
    client = TestClient(app)

    def show(name: str, method: str, url: str, **kwargs):
        try:
            resp = getattr(client, method)(url, **kwargs)
            print(f"{name}: {resp.status_code}")
            try:
                print(resp.json())
            except Exception:
                print(resp.text)
        except Exception as e:
            print(f"{name}: ERROR {e}")

    show("HEALTH", "get", "/health/")
    show("TELEM_CURRENT", "get", "/api/v1/telemetry/current")
    show(
        "CENTER_MISSION",
        "post",
        "/api/v1/center/mission",
        json={
            "mission_id": "m1",
            "mission_type": "move_to",
            "target_position": {"x": 1, "y": 2, "z": 0, "theta": 0},
        },
    )
    show("CONFIG_GET", "get", "/api/v1/config/")
    show("USER_SETTINGS_GET", "get", "/api/v1/user/settings/admin")


if __name__ == "__main__":
    main()


