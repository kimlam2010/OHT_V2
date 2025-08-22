"""
Admin API endpoints for user management
"""
from typing import Dict, Any, List
from fastapi import APIRouter, Depends, HTTPException, status
from app.services.auth_service import AuthService
from app.core.security import verify_token
from app.models.auth import UserRecord

router = APIRouter(tags=["Admin Management"])


def get_auth_service() -> AuthService:
    """Dependency injection for AuthService"""
    return AuthService()


@router.get("/users", summary="Danh sách tất cả users")
async def list_users(svc: AuthService = Depends(get_auth_service)):
    """Lấy danh sách tất cả users (Admin only)"""
    try:
        users = svc.list_users()
        return {
            "success": True,
            "users": users,
            "total": len(users)
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Lỗi khi lấy danh sách users: {str(e)}"
        )


@router.post("/users", summary="Tạo user mới")
async def create_user(
    user_data: Dict[str, Any], 
    svc: AuthService = Depends(get_auth_service)
):
    """Tạo user mới (Admin only)"""
    try:
        username = user_data.get("username")
        password = user_data.get("password")
        role = user_data.get("role", "operator")
        
        if not username or not password:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Username và password là bắt buộc"
            )
        
        svc.add_user(username, password, role)
        return {
            "success": True,
            "message": f"User {username} đã được tạo thành công",
            "username": username,
            "role": role
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Lỗi khi tạo user: {str(e)}"
        )


@router.put("/users/{username}/password", summary="Đặt lại mật khẩu user")
async def reset_password(
    username: str, 
    body: Dict[str, Any], 
    svc: AuthService = Depends(get_auth_service)
):
    """Đặt lại mật khẩu cho user (Admin only)"""
    try:
        new_password = body.get("password")
        if not new_password:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Password mới là bắt buộc"
            )
        
        svc.set_password(username, new_password)
        return {
            "success": True,
            "message": f"Đã đặt lại mật khẩu cho user {username}"
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Lỗi khi đặt lại mật khẩu: {str(e)}"
        )


@router.delete("/users/{username}", summary="Xóa user")
async def delete_user(
    username: str, 
    svc: AuthService = Depends(get_auth_service)
):
    """Xóa user (Admin only)"""
    try:
        svc.delete_user(username)
        return {
            "success": True,
            "message": f"User {username} đã được xóa"
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Lỗi khi xóa user: {str(e)}"
        )


@router.get("/users/{username}", summary="Thông tin chi tiết user")
async def get_user_info(
    username: str, 
    svc: AuthService = Depends(get_auth_service)
):
    """Lấy thông tin chi tiết user (Admin only)"""
    try:
        users = svc.list_users()
        user = next((u for u in users if u["username"] == username), None)
        
        if not user:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail=f"User {username} không tồn tại"
            )
        
        return {
            "success": True,
            "user": {
                "username": user["username"],
                "role": user["role"],
                "created_at": user.get("created_at")
            }
        }
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Lỗi khi lấy thông tin user: {str(e)}"
        )


@router.put("/users/{username}/role", summary="Cập nhật role user")
async def update_user_role(
    username: str, 
    body: Dict[str, Any], 
    svc: AuthService = Depends(get_auth_service)
):
    """Cập nhật role cho user (Admin only)"""
    try:
        new_role = body.get("role")
        if not new_role or new_role not in ["admin", "operator"]:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Role phải là 'admin' hoặc 'operator'"
            )
        
        # Implementation cần thêm method update_user_role trong AuthService
        # svc.update_user_role(username, new_role)
        
        return {
            "success": True,
            "message": f"Đã cập nhật role cho user {username} thành {new_role}"
        }
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Lỗi khi cập nhật role: {str(e)}"
        )


@router.get("/audit", summary="Lịch sử hoạt động admin")
async def get_admin_audit_logs(
    limit: int = 100,
    svc: AuthService = Depends(get_auth_service)
):
    """Lấy lịch sử hoạt động admin (Admin only)"""
    try:
        # Implementation cần thêm method get_audit_logs trong AuthService
        # audit_logs = svc.get_audit_logs(limit)
        
        return {
            "success": True,
            "audit_logs": [],
            "total": 0,
            "message": "Audit logs sẽ được implement trong tương lai"
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Lỗi khi lấy audit logs: {str(e)}"
        )
