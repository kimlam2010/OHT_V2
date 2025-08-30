"""
Auth API: đăng nhập + quản trị người dùng tối giản
"""

from fastapi import APIRouter, Depends, HTTPException

from app.core.logging import get_logger
from app.models.auth import LoginRequest, TokenResponse
from app.services.auth_service import AuthService


logger = get_logger(__name__)
router = APIRouter(tags=["Authentication"])

_svc = None


def get_auth_service() -> AuthService:
    global _svc
    if _svc is None:
        _svc = AuthService()
    return _svc


@router.post("/login", response_model=TokenResponse)
async def login(payload: LoginRequest, svc: AuthService = Depends(get_auth_service)):
    if not svc.verify_user(payload.username, payload.password):
        logger.warning("Login failed", username=payload.username)
        raise HTTPException(status_code=401, detail="Invalid credentials")
    token = svc.issue_token(payload.username)
    logger.info("Login success", username=payload.username)
    return TokenResponse(token=token)


@router.get("/users")
async def list_users(svc: AuthService = Depends(get_auth_service)):
    return {"users": svc.list_users()}


@router.post("/users")
async def create_user(user: dict, svc: AuthService = Depends(get_auth_service)):
    username = (user.get("username") or "").strip()
    password = (user.get("password") or "").strip()
    role = user.get("role") or "operator"
    if not username or not password:
        raise HTTPException(status_code=400, detail="username/password required")
    try:
        svc.add_user(username, password, role)
    except ValueError as e:
        raise HTTPException(status_code=409, detail=str(e))
    return {"users": svc.list_users()}


@router.post("/users/{username}/password")
async def reset_password(username: str, body: dict, svc: AuthService = Depends(get_auth_service)):
    new_password = (body.get("password") or "").strip()
    if not new_password:
        raise HTTPException(status_code=400, detail="password required")
    try:
        svc.set_password(username, new_password)
    except ValueError as e:
        raise HTTPException(status_code=404, detail=str(e))
    return {"success": True}


@router.delete("/users/{username}")
async def delete_user(username: str, svc: AuthService = Depends(get_auth_service)):
    svc.delete_user(username)
    return {"users": svc.list_users()}


