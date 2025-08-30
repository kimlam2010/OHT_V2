from fastapi.testclient import TestClient


def test_login_admin_default(client: TestClient):
    resp = client.post("/api/v1/auth/login", json={"username": "admin", "password": "admin"})
    assert resp.status_code == 200
    token = resp.json().get("token")
    assert isinstance(token, str) and len(token) > 0


def test_user_crud(client: TestClient):
    # list
    resp = client.get("/api/v1/auth/users")
    assert resp.status_code == 200
    assert any(u["username"] == "admin" for u in resp.json().get("users", []))

    # create
    resp = client.post("/api/v1/auth/users", json={"username": "u1", "password": "p1", "role": "operator"})
    assert resp.status_code == 200
    assert any(u["username"] == "u1" for u in resp.json().get("users", []))

    # reset password
    resp = client.post("/api/v1/auth/users/u1/password", json={"password": "new"})
    assert resp.status_code == 200
    assert resp.json().get("success") is True

    # delete
    resp = client.delete("/api/v1/auth/users/u1")
    assert resp.status_code == 200
    assert all(u["username"] != "u1" for u in resp.json().get("users", []))


