"""
Production RBAC (Role-Based Access Control) System

This module provides production-grade role-based access control.
It includes permission matrix, role management, and access validation.
"""

import logging
from typing import Dict, List, Optional, Callable, Any
from functools import wraps
from fastapi import Depends, HTTPException, status
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials

from app.models.user import User
from app.core.security import get_current_user

logger = logging.getLogger(__name__)

# Security scheme
security = HTTPBearer()


class ProductionRBAC:
    """
    Production-grade Role-Based Access Control system
    
    Features:
    - Configurable permission matrix
    - Role-based access control
    - Resource-level permissions
    - Action-level permissions
    - Dynamic permission checking
    """
    
    def __init__(self):
        self.permission_matrix = self._load_permission_matrix()
        self.role_hierarchy = self._load_role_hierarchy()
        
        logger.info("🔐 ProductionRBAC initialized")
        
    def _load_permission_matrix(self) -> Dict[str, Dict[str, List[str]]]:
        """Load permission matrix configuration"""
        return {
            "admin": {
                "robot": ["read", "write", "delete", "control"],
                "telemetry": ["read", "write", "delete"],
                "safety": ["read", "write", "delete", "override", "acknowledge"],
                "configuration": ["read", "write", "delete"],
                "users": ["read", "write", "delete", "manage"],
                "system": ["read", "write", "delete", "admin"]
            },
            "administrator": {
                "robot": ["read", "write", "delete", "control"],
                "telemetry": ["read", "write", "delete"],
                "safety": ["read", "write", "delete", "override", "acknowledge"],
                "configuration": ["read", "write", "delete"],
                "users": ["read", "write", "delete", "manage"],
                "system": ["read", "write", "delete", "admin"],
                "map": ["read", "write", "delete", "control"],
                "sensors": ["read", "write", "delete", "control"],
                "localization": ["read", "write", "delete", "control"],
                "monitoring": ["read", "write", "delete", "admin"]
            },
            "operator": {
                "robot": ["read", "control"],
                "telemetry": ["read"],
                "safety": ["read", "override"],
                "configuration": ["read"],
                "users": ["read"],
                "system": ["read"]
            },
            "viewer": {
                "robot": ["read"],
                "telemetry": ["read"],
                "safety": ["read"],
                "configuration": ["read"],
                "users": ["read"],
                "system": ["read"]
            },
            "guest": {
                "robot": ["read"],
                "telemetry": ["read"],
                "safety": ["read"],
                "configuration": ["read"],
                "users": [],
                "system": []
            }
        }
        
    def _load_role_hierarchy(self) -> Dict[str, List[str]]:
        """Load role hierarchy configuration"""
        return {
            "admin": ["admin", "operator", "viewer", "guest"],
            "operator": ["operator", "viewer", "guest"],
            "viewer": ["viewer", "guest"],
            "guest": ["guest"]
        }
        
    def has_permission(self, user_role: str, resource: str, permission: str) -> bool:
        """
        Check if user role has permission for resource and action
        
        Args:
            user_role: User's role
            resource: Resource to access
            permission: Permission required
            
        Returns:
            True if permission granted, False otherwise
        """
        try:
            # Get role permissions
            role_permissions = self.permission_matrix.get(user_role, {})
            resource_permissions = role_permissions.get(resource, [])
            
            # Check direct permission
            if permission in resource_permissions:
                return True
                
            # Check role hierarchy
            if user_role in self.role_hierarchy:
                for inherited_role in self.role_hierarchy[user_role]:
                    inherited_permissions = self.permission_matrix.get(inherited_role, {})
                    inherited_resource_permissions = inherited_permissions.get(resource, [])
                    
                    if permission in inherited_resource_permissions:
                        return True
                        
            return False
            
        except Exception as e:
            logger.error(f"❌ Permission check failed: {e}")
            return False
            
    def require_permission(self, resource: str, permission: str):
        """
        Create permission checker dependency
        
        Args:
            resource: Resource to access
            permission: Permission required
            
        Returns:
            FastAPI dependency function
        """
        async def permission_checker(current_user: User = Depends(get_current_user)):
            if not self.has_permission(current_user.role, resource, permission):
                logger.warning(
                    f"🚫 Access denied: User {current_user.username} (role: {current_user.role}) "
                    f"tried to access {resource}:{permission}"
                )
                raise HTTPException(
                    status_code=status.HTTP_403_FORBIDDEN,
                    detail=f"Access denied: {resource}:{permission}"
                )
            return current_user
            
        return permission_checker
        
    def require_role(self, required_role: str):
        """
        Create role checker dependency
        
        Args:
            required_role: Minimum role required
            
        Returns:
            FastAPI dependency function
        """
        async def role_checker(current_user: User = Depends(get_current_user)):
            if not self._has_minimum_role(current_user.role, required_role):
                logger.warning(
                    f"🚫 Role insufficient: User {current_user.username} (role: {current_user.role}) "
                    f"requires minimum role: {required_role}"
                )
                raise HTTPException(
                    status_code=status.HTTP_403_FORBIDDEN,
                    detail=f"Insufficient role: requires {required_role} or higher"
                )
            return current_user
            
        return role_checker
        
    def _has_minimum_role(self, user_role: str, required_role: str) -> bool:
        """Check if user has minimum required role"""
        if user_role == required_role:
            return True
            
        # Check role hierarchy
        if user_role in self.role_hierarchy:
            return required_role in self.role_hierarchy[user_role]
            
        return False
        
    def get_user_permissions(self, user_role: str) -> Dict[str, List[str]]:
        """Get all permissions for a user role"""
        permissions = {}
        
        # Get direct permissions
        role_permissions = self.permission_matrix.get(user_role, {})
        for resource, perms in role_permissions.items():
            permissions[resource] = perms.copy()
            
        # Get inherited permissions
        if user_role in self.role_hierarchy:
            for inherited_role in self.role_hierarchy[user_role]:
                inherited_permissions = self.permission_matrix.get(inherited_role, {})
                for resource, perms in inherited_permissions.items():
                    if resource not in permissions:
                        permissions[resource] = []
                    permissions[resource].extend(perms)
                    
        # Remove duplicates
        for resource in permissions:
            permissions[resource] = list(set(permissions[resource]))
            
        return permissions
        
    def add_permission(self, role: str, resource: str, permission: str):
        """Add permission to role"""
        if role not in self.permission_matrix:
            self.permission_matrix[role] = {}
            
        if resource not in self.permission_matrix[role]:
            self.permission_matrix[role][resource] = []
            
        if permission not in self.permission_matrix[role][resource]:
            self.permission_matrix[role][resource].append(permission)
            logger.info(f"🔐 Added permission: {role}:{resource}:{permission}")
            
    def remove_permission(self, role: str, resource: str, permission: str):
        """Remove permission from role"""
        if (role in self.permission_matrix and 
            resource in self.permission_matrix[role] and
            permission in self.permission_matrix[role][resource]):
            
            self.permission_matrix[role][resource].remove(permission)
            logger.info(f"🔐 Removed permission: {role}:{resource}:{permission}")
            
    def create_role(self, role_name: str, inherit_from: Optional[str] = None):
        """Create new role"""
        if role_name in self.permission_matrix:
            logger.warning(f"⚠️ Role {role_name} already exists")
            return
            
        self.permission_matrix[role_name] = {}
        
        if inherit_from and inherit_from in self.role_hierarchy:
            self.role_hierarchy[role_name] = [inherit_from] + self.role_hierarchy[inherit_from]
        else:
            self.role_hierarchy[role_name] = [role_name]
            
        logger.info(f"🔐 Created role: {role_name}")
        
    def delete_role(self, role_name: str):
        """Delete role"""
        if role_name in self.permission_matrix:
            del self.permission_matrix[role_name]
            
        if role_name in self.role_hierarchy:
            del self.role_hierarchy[role_name]
            
        # Remove from other roles' inheritance
        for role in self.role_hierarchy:
            if role_name in self.role_hierarchy[role]:
                self.role_hierarchy[role].remove(role_name)
                
        logger.info(f"🔐 Deleted role: {role_name}")
        
    def get_all_roles(self) -> List[str]:
        """Get all available roles"""
        return list(self.permission_matrix.keys())
        
    def get_role_info(self, role_name: str) -> Dict[str, Any]:
        """Get comprehensive role information"""
        if role_name not in self.permission_matrix:
            return {"error": f"Role {role_name} not found"}
            
        return {
            "role": role_name,
            "permissions": self.permission_matrix[role_name],
            "inheritance": self.role_hierarchy.get(role_name, []),
            "effective_permissions": self.get_user_permissions(role_name)
        }
        
    def validate_permission_matrix(self) -> Dict[str, Any]:
        """Validate permission matrix configuration"""
        errors = []
        warnings = []
        
        # Check for orphaned roles
        for role in self.permission_matrix:
            if role not in self.role_hierarchy:
                errors.append(f"Role {role} missing from hierarchy")
                
        # Check for invalid inheritance
        for role, inherited_roles in self.role_hierarchy.items():
            for inherited_role in inherited_roles:
                if inherited_role not in self.permission_matrix:
                    errors.append(f"Role {role} inherits from non-existent role {inherited_role}")
                    
        # Check for empty permissions
        for role, resources in self.permission_matrix.items():
            for resource, permissions in resources.items():
                if not permissions:
                    warnings.append(f"Role {role} has no permissions for {resource}")
                    
        return {
            "valid": len(errors) == 0,
            "errors": errors,
            "warnings": warnings,
            "total_roles": len(self.permission_matrix),
            "total_resources": len(set().union(*[set(r.keys()) for r in self.permission_matrix.values()]))
        }


# Global RBAC instance
rbac = ProductionRBAC()


# Convenience functions
def require_permission(resource: str, permission: str):
    """Convenience function for permission checking"""
    return rbac.require_permission(resource, permission)


def require_role(role: str):
    """Convenience function for role checking"""
    return rbac.require_role(role)


def has_permission(user_role: str, resource: str, permission: str) -> bool:
    """Convenience function for permission checking"""
    return rbac.has_permission(user_role, resource, permission)
