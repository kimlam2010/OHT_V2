# REQ_BE_17 - Security Implementation

**Document ID:** REQ_BE_17  
**Version:** 1.0  
**Date:** 2025-01-28  
**Status:** Draft  
**Classification:** Internal  
**Author:** CTO Team  
**Standard Reference:** ISO/IEC/IEEE 29148:2018

---

## 1. DOCUMENT INFORMATION

### 1.1 Document Control
- **Document Title:** Security Implementation
- **Document Type:** Security Implementation Specification
- **Standard Reference:** ISO/IEC/IEEE 29148:2018, ISO/IEC 27001:2013
- **Project:** OHT-50 Industrial Robot Control System
- **Component:** Backend Security Implementation

### 1.2 Revision History
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-01-28 | CTO Team | Initial creation |

---

## 2. SECURITY IMPLEMENTATION OVERVIEW

### 2.1 Security Strategy
```yaml
Security Strategy:
  Approach: "Defense-in-depth security"
  Framework: "Zero Trust Architecture"
  Authentication: "Multi-factor authentication"
  Authorization: "Role-based access control"
  Encryption: "End-to-end encryption"
  Monitoring: "Real-time security monitoring"
  Compliance: "ISO 27001, IEC 62443"
```

### 2.2 Security Layers
```
Security Layers:
├── Network Security
│   ├── Firewall protection
│   ├── VPN access
│   ├── Network segmentation
│   └── DDoS protection
├── Application Security
│   ├── Input validation
│   ├── Output encoding
│   ├── Session management
│   └── Error handling
├── Data Security
│   ├── Encryption at rest
│   ├── Encryption in transit
│   ├── Data classification
│   └── Access controls
└── Infrastructure Security
    ├── Hardened systems
    ├── Security patches
    ├── Vulnerability scanning
    └── Security monitoring
```

---

## 3. AUTHENTICATION IMPLEMENTATION

### 3.1 JWT Authentication
```python
# app/security/authentication.py
from datetime import datetime, timedelta
from jose import JWTError, jwt
from passlib.context import CryptContext
from fastapi import HTTPException, status, Depends
from fastapi.security import HTTPBearer

pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")
security = HTTPBearer()

class AuthenticationService:
    def __init__(self):
        self.secret_key = settings.SECRET_KEY
        self.algorithm = settings.JWT_ALGORITHM
        self.access_token_expire_minutes = settings.JWT_EXPIRATION
    
    def verify_password(self, plain_password: str, hashed_password: str) -> bool:
        return pwd_context.verify(plain_password, hashed_password)
    
    def get_password_hash(self, password: str) -> str:
        return pwd_context.hash(password)
    
    def create_access_token(self, data: dict, expires_delta: timedelta = None) -> str:
        to_encode = data.copy()
        if expires_delta:
            expire = datetime.utcnow() + expires_delta
        else:
            expire = datetime.utcnow() + timedelta(minutes=self.access_token_expire_minutes)
        
        to_encode.update({"exp": expire})
        return jwt.encode(to_encode, self.secret_key, algorithm=self.algorithm)
    
    def verify_token(self, token: str) -> dict:
        try:
            payload = jwt.decode(token, self.secret_key, algorithms=[self.algorithm])
            return payload
        except JWTError:
            raise HTTPException(status_code=401, detail="Invalid token")

async def get_current_user(credentials: HTTPAuthorizationCredentials = Depends(security)) -> User:
    auth_service = AuthenticationService()
    payload = auth_service.verify_token(credentials.credentials)
    username = payload.get("sub")
    user = get_user_by_username(username)
    if not user:
        raise HTTPException(status_code=401, detail="User not found")
    return user
```

### 3.2 Multi-Factor Authentication
```python
# app/security/mfa.py
import pyotp
import qrcode

class MFAService:
    def generate_secret(self) -> str:
        return pyotp.random_base32()
    
    def generate_qr_code(self, username: str, secret: str) -> str:
        totp_uri = pyotp.totp.TOTP(secret).provisioning_uri(
            name=username,
            issuer_name="OHT-50 System"
        )
        qr = qrcode.QRCode(version=1, box_size=10, border=5)
        qr.add_data(totp_uri)
        qr.make(fit=True)
        img = qr.make_image(fill_color="black", back_color="white")
        
        import io
        import base64
        buffer = io.BytesIO()
        img.save(buffer, format='PNG')
        return base64.b64encode(buffer.getvalue()).decode()
    
    def verify_totp(self, secret: str, token: str) -> bool:
        totp = pyotp.TOTP(secret)
        return totp.verify(token)
```

---

## 4. AUTHORIZATION IMPLEMENTATION

### 4.1 Role-Based Access Control
```python
# app/security/authorization.py
from enum import Enum
from fastapi import HTTPException, status, Depends

class Permission(str, Enum):
    ROBOT_READ = "robot:read"
    ROBOT_CONTROL = "robot:control"
    ROBOT_CONFIG = "robot:config"
    LIDAR_READ = "lidar:read"
    LIDAR_CONTROL = "lidar:control"
    SAFETY_READ = "safety:read"
    SAFETY_CONTROL = "safety:control"
    SYSTEM_ADMIN = "system:admin"

class Role(str, Enum):
    ADMIN = "admin"
    OPERATOR = "operator"
    VIEWER = "viewer"
    MAINTENANCE = "maintenance"

ROLE_PERMISSIONS = {
    Role.ADMIN: [p for p in Permission],
    Role.OPERATOR: [
        Permission.ROBOT_READ, Permission.ROBOT_CONTROL,
        Permission.LIDAR_READ, Permission.LIDAR_CONTROL,
        Permission.SAFETY_READ, Permission.SAFETY_CONTROL
    ],
    Role.VIEWER: [
        Permission.ROBOT_READ, Permission.LIDAR_READ, Permission.SAFETY_READ
    ],
    Role.MAINTENANCE: [
        Permission.ROBOT_READ, Permission.ROBOT_CONFIG,
        Permission.LIDAR_READ, Permission.LIDAR_CONTROL,
        Permission.SAFETY_READ, Permission.SAFETY_CONTROL
    ]
}

class AuthorizationService:
    def has_permission(self, user: User, permission: Permission) -> bool:
        user_role = Role(user.role)
        user_permissions = ROLE_PERMISSIONS.get(user_role, [])
        return permission in user_permissions
    
    def require_permission(self, permission: Permission):
        def permission_dependency(current_user: User = Depends(get_current_user)):
            if not self.has_permission(current_user, permission):
                raise HTTPException(status_code=403, detail="Insufficient permissions")
            return current_user
        return permission_dependency

require_robot_control = AuthorizationService().require_permission(Permission.ROBOT_CONTROL)
require_admin = AuthorizationService().require_role(Role.ADMIN)
```

---

## 5. DATA ENCRYPTION

### 5.1 Encryption Service
```python
# app/security/encryption.py
from cryptography.fernet import Fernet
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
import base64

class EncryptionService:
    def __init__(self):
        self.symmetric_key = settings.ENCRYPTION_KEY.encode()
        self.fernet = Fernet(self.symmetric_key)
    
    def encrypt_symmetric(self, data: str) -> str:
        return self.fernet.encrypt(data.encode()).decode()
    
    def decrypt_symmetric(self, encrypted_data: str) -> str:
        return self.fernet.decrypt(encrypted_data.encode()).decode()
    
    def hash_password(self, password: str, salt: str = None) -> tuple:
        if salt is None:
            salt = os.urandom(16)
        
        kdf = PBKDF2HMAC(
            algorithm=hashes.SHA256(),
            length=32,
            salt=salt,
            iterations=100000,
        )
        key = base64.urlsafe_b64encode(kdf.derive(password.encode()))
        return key.decode(), base64.b64encode(salt).decode()
    
    def verify_password(self, password: str, hashed_password: str, salt: str) -> bool:
        try:
            salt_bytes = base64.b64decode(salt.encode())
            kdf = PBKDF2HMAC(
                algorithm=hashes.SHA256(),
                length=32,
                salt=salt_bytes,
                iterations=100000,
            )
            key = base64.urlsafe_b64encode(kdf.derive(password.encode()))
            return key.decode() == hashed_password
        except Exception:
            return False
```

---

## 6. INPUT VALIDATION

### 6.1 Input Validation Service
```python
# app/security/validation.py
import re
import html
from pydantic import BaseModel, validator

class InputValidator:
    def __init__(self):
        self.sql_patterns = [
            r"(\b(SELECT|INSERT|UPDATE|DELETE|DROP|CREATE|ALTER|EXEC|UNION)\b)",
            r"(\b(OR|AND)\b\s+\d+\s*=\s*\d+)",
            r"(--|#|/\*|\*/)",
        ]
        
        self.xss_patterns = [
            r"<script[^>]*>.*?</script>",
            r"<iframe[^>]*>.*?</iframe>",
            r"javascript:",
            r"onload\s*=",
            r"onerror\s*=",
        ]
    
    def validate_sql_injection(self, value: str) -> bool:
        if not value:
            return True
        
        value_upper = value.upper()
        for pattern in self.sql_patterns:
            if re.search(pattern, value_upper, re.IGNORECASE):
                return False
        return True
    
    def validate_xss(self, value: str) -> bool:
        if not value:
            return True
        
        for pattern in self.xss_patterns:
            if re.search(pattern, value, re.IGNORECASE):
                return False
        return True
    
    def sanitize_input(self, value: str) -> str:
        if not value:
            return value
        
        value = html.escape(value)
        value = value.replace('\x00', '')
        value = ''.join(char for char in value if ord(char) >= 32)
        return value

class RobotControlRequest(BaseModel):
    command: str
    speed: Optional[float] = None
    duration: Optional[float] = None
    
    @validator('command')
    def validate_command(cls, v):
        valid_commands = ["move_forward", "move_backward", "turn_left", "turn_right", "stop"]
        if v not in valid_commands:
            raise ValueError('Invalid robot command')
        return v
    
    @validator('speed')
    def validate_speed(cls, v):
        if v is not None and (v < 0 or v > 10):
            raise ValueError('Speed must be between 0 and 10')
        return v
```

---

## 7. SECURITY MONITORING

### 7.1 Security Event Logging
```python
# app/security/monitoring.py
import logging
import json
from datetime import datetime
from app.models.security_event import SecurityEvent

class SecurityMonitor:
    def __init__(self):
        self.logger = logging.getLogger("security")
        self.logger.setLevel(logging.INFO)
        
        handler = logging.FileHandler("security.log")
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
    
    def log_security_event(self, event_type: str, severity: str, description: str, 
                          user_id: int = None, ip_address: str = None):
        log_entry = {
            "timestamp": datetime.utcnow().isoformat(),
            "event_type": event_type,
            "severity": severity,
            "description": description,
            "user_id": user_id,
            "ip_address": ip_address
        }
        
        self.logger.warning(json.dumps(log_entry))
        self.store_security_event(log_entry)
    
    def monitor_authentication_attempts(self, username: str, success: bool, ip_address: str):
        if not success:
            self.log_security_event(
                event_type="failed_login",
                severity="warning",
                description=f"Failed login attempt for user: {username}",
                ip_address=ip_address
            )
    
    def monitor_authorization_violations(self, user_id: int, resource: str, action: str):
        self.log_security_event(
            event_type="authorization_violation",
            severity="warning",
            description=f"Authorization violation: User {user_id} attempted {action} on {resource}",
            user_id=user_id
        )
```

---

## 8. SECURITY MIDDLEWARE

### 8.1 Security Headers Middleware
```python
# app/security/middleware.py
from fastapi import Request
from fastapi.responses import JSONResponse
import time

class SecurityMiddleware:
    def __init__(self):
        self.rate_limit_window = 60
        self.rate_limit_max_requests = 100
        self.request_counts = {}
    
    async def __call__(self, request: Request, call_next):
        # Rate limiting
        client_ip = request.client.host
        if not self.check_rate_limit(client_ip):
            return JSONResponse(
                status_code=429,
                content={"detail": "Rate limit exceeded"}
            )
        
        # Process request
        response = await call_next(request)
        
        # Add security headers
        self.add_security_headers(response)
        
        return response
    
    def check_rate_limit(self, client_ip: str) -> bool:
        current_time = time.time()
        window_start = current_time - self.rate_limit_window
        
        # Clean old entries
        self.request_counts = {
            ip: count for ip, count in self.request_counts.items()
            if count['timestamp'] > window_start
        }
        
        # Check current count
        if client_ip in self.request_counts:
            count_info = self.request_counts[client_ip]
            if count_info['timestamp'] > window_start:
                if count_info['count'] >= self.rate_limit_max_requests:
                    return False
                count_info['count'] += 1
            else:
                self.request_counts[client_ip] = {'count': 1, 'timestamp': current_time}
        else:
            self.request_counts[client_ip] = {'count': 1, 'timestamp': current_time}
        
        return True
    
    def add_security_headers(self, response):
        response.headers["X-Content-Type-Options"] = "nosniff"
        response.headers["X-Frame-Options"] = "DENY"
        response.headers["X-XSS-Protection"] = "1; mode=block"
        response.headers["Strict-Transport-Security"] = "max-age=31536000"
        response.headers["Content-Security-Policy"] = "default-src 'self'"
```

---

## 9. REFERENCES

### 9.1 Related Documents
- REQ_BE_01: System Overview
- REQ_BE_02: API Specifications
- REQ_BE_03: Database Design
- REQ_BE_04: Security Requirements
- REQ_BE_05: Integration Specifications
- REQ_BE_06: LiDAR Integration

### 9.2 Standards References
- ISO/IEC/IEEE 29148:2018 - Requirements Engineering
- ISO/IEC 27001:2013 - Information Security Management
- ISO/IEC 62443:2018 - Industrial Automation Security
- OWASP Top 10 - Web Application Security
- NIST Cybersecurity Framework

---

## 10. APPROVAL

| Role | Name | Date | Signature |
|------|------|------|-----------|
| CTO | [Name] | [Date] | [Signature] |
| Technical Lead | [Name] | [Date] | [Signature] |
| Security Engineer | [Name] | [Date] | [Signature] |

---

**Document End**
