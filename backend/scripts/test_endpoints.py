import json
import time
from urllib import request, error

BASE_URL = "http://127.0.0.1:8080"


def http_get(path: str, headers: dict | None = None):
    url = f"{BASE_URL}{path}"
    req = request.Request(url, headers=headers or {}, method="GET")
    started = time.time()
    try:
        with request.urlopen(req, timeout=5) as resp:
            body = resp.read()
            elapsed_ms = int((time.time() - started) * 1000)
            return {
                "ok": True,
                "status": resp.status,
                "ms": elapsed_ms,
                "json": json.loads(body.decode("utf-8")) if body else None,
            }
    except error.HTTPError as e:
        try:
            body = e.read()
            detail = body.decode("utf-8")
        except Exception:
            detail = str(e)
        return {"ok": False, "status": e.code, "error": detail}
    except Exception as e:
        return {"ok": False, "status": None, "error": str(e)}


def http_post(path: str, payload: dict, headers: dict | None = None):
    url = f"{BASE_URL}{path}"
    data = json.dumps(payload).encode("utf-8")
    hdrs = {"Content-Type": "application/json"}
    if headers:
        hdrs.update(headers)
    req = request.Request(url, data=data, headers=hdrs, method="POST")
    started = time.time()
    try:
        with request.urlopen(req, timeout=8) as resp:
            body = resp.read()
            elapsed_ms = int((time.time() - started) * 1000)
            return {
                "ok": True,
                "status": resp.status,
                "ms": elapsed_ms,
                "json": json.loads(body.decode("utf-8")) if body else None,
            }
    except error.HTTPError as e:
        try:
            body = e.read()
            detail = body.decode("utf-8")
        except Exception:
            detail = str(e)
        return {"ok": False, "status": e.code, "error": detail}
    except Exception as e:
        return {"ok": False, "status": None, "error": str(e)}


def main():
    report: dict[str, dict] = {}

    # Public endpoints
    report["health_fast"] = http_get("/health/fast")
    report["root"] = http_get("/")

    # Auth: try admin first
    token = None
    login_resp = http_post("/api/v1/auth/login", {"username": "admin", "password": "admin123"})
    report["auth_login_admin"] = login_resp

    if login_resp.get("ok") and login_resp.get("json"):
        token = login_resp["json"].get("access_token")
    else:
        # fallback: create temporary user then login
        uname = "admin1"
        register_resp = http_post(
            "/api/v1/auth/register",
            {
                "username": uname,
                "email": f"{uname}@example.com",
                "password": "StrongPass123!",
                "role": "admin",
                "full_name": "Admin One",
            },
        )
        report["auth_register_admin1"] = register_resp
        login2_resp = http_post(
            "/api/v1/auth/login", {"username": uname, "password": "StrongPass123!"}
        )
        report["auth_login_admin1"] = login2_resp
        if login2_resp.get("ok") and login2_resp.get("json"):
            token = login2_resp["json"].get("access_token")

    auth_headers = {"Authorization": f"Bearer {token}"} if token else {}

    # Protected endpoints
    report["telemetry_current"] = http_get("/api/v1/telemetry/current", headers=auth_headers)
    report["safety_status"] = http_get("/api/v1/safety/status", headers=auth_headers)
    report["config_system"] = http_get("/api/v1/config/system", headers=auth_headers)

    report["robot_status"] = http_get("/api/v1/robot/status", headers=auth_headers)

    move_cmd = {
        "type": "move",
        "parameters": {"speed": 0.5, "direction": "forward", "duration_ms": 1000},
    }
    report["robot_command_move"] = http_post(
        "/api/v1/robot/command", move_cmd, headers=auth_headers
    )

    # Print concise summary
    summary = {}
    for key, val in report.items():
        summary[key] = {
            "ok": val.get("ok"),
            "status": val.get("status"),
        }
        if not val.get("ok"):
            # shorten error
            err = str(val.get("error", ""))
            summary[key]["error"] = err[:180]
    print(json.dumps({"summary": summary, "details": report}, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
