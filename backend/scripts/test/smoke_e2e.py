import os
os.environ.setdefault("TESTING", "true")
os.environ.setdefault("USE_FIRMWARE_MOCK", "true")

from fastapi.testclient import TestClient
from app.main import app


def main() -> None:
    client = TestClient(app)

    print("== Smoke E2E Tests ==")

    # Login
    login_resp = client.post(
        "/api/v1/auth/login",
        json={"username": "admin", "password": "admin123"},
    )
    print("LOGIN:", login_resp.status_code)
    token = None
    if login_resp.status_code == 200:
        token = login_resp.json().get("access_token")
    headers = {"Authorization": f"Bearer {token}"} if token else {}

    # Robot status
    r1 = client.get("/api/v1/robot/status", headers=headers)
    print("ROBOT_STATUS:", r1.status_code)
    try:
        print(r1.json())
    except Exception:
        print(r1.text)

    # Telemetry current
    r2 = client.get("/api/v1/telemetry/current", headers=headers)
    print("TELEMETRY_CURRENT:", r2.status_code)
    try:
        print(r2.json())
    except Exception:
        print(r2.text)

    # Safety emergency
    r3 = client.post("/api/v1/safety/emergency", headers=headers)
    print("SAFETY_EMERGENCY:", r3.status_code)
    try:
        print(r3.json())
    except Exception:
        print(r3.text)

    # Dashboard summary
    r4 = client.get("/api/v1/dashboard/summary", headers=headers)
    print("DASHBOARD_SUMMARY:", r4.status_code)
    try:
        print(r4.json())
    except Exception:
        print(r4.text)


if __name__ == "__main__":
    main()


