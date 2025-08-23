# ADR-002: Authentication System Decision

## Status
**APPROVED** - 2024-08-21

## Context
OHT-50 Master Module cần hệ thống xác thực an toàn cho:
- API access control
- Role-based permissions
- Real-time WebSocket connections
- Hardware control access
- Audit logging
- Compliance với safety standards

## Decision
**Chọn JWT-based Authentication với Role-Based Access Control (RBAC)**

### Authentication Architecture:
```
┌─────────────────────────────────────┐
│           Client Request             │
└─────────────────┬───────────────────┘
                  │
            ┌─────▼─────┐
            │   JWT     │
            │  Token    │
            └─────┬─────┘
                  │
            ┌─────▼─────┐
            │  Token    │
            │Validation │
            └─────┬─────┘
                  │
            ┌─────▼─────┐
            │   RBAC    │
            │  Check    │
            └─────┬─────┘
                  │
            ┌─────▼─────┐
            │  Access   │
            │  Grant    │
            └───────────┘
```

### User Roles:
1. **super_admin**: Full system access
2. **admin**: System configuration và management
3. **operator**: Mission control và monitoring
4. **viewer**: Read-only access
5. **system**: Internal system access

### Token Structure:
```json
{
  "sub": "user_id",
  "role": "operator",
  "permissions": ["mission:read", "mission:write"],
  "exp": 1640995200,
  "iat": 1640908800
}
```

## Consequences

### Positive:
- **Stateless**: No server-side session storage
- **Scalable**: Works across multiple instances
- **Secure**: Signed tokens với expiration
- **Flexible**: Role-based permissions
- **Standard**: Industry-standard approach
- **Audit Trail**: Complete access logging

### Negative:
- **Token Size**: Larger than session IDs
- **Revocation**: Requires blacklist mechanism
- **Complexity**: Token management overhead
- **Security Risk**: Token exposure risk

### Risks:
- **Token Theft**: Stolen tokens remain valid
- **Clock Skew**: Time synchronization issues
- **Key Management**: Secret key rotation complexity

## Mitigation Strategies:
1. **Short Expiration**: 30-minute access tokens
2. **Refresh Tokens**: Long-lived refresh tokens
3. **Token Blacklist**: Redis-based revocation
4. **HTTPS Only**: Secure transmission
5. **Key Rotation**: Automated key management

## Performance Impact Assessment:
- **Token Validation**: < 1ms per request
- **Memory Usage**: Minimal (stateless)
- **Database Load**: Reduced (no session queries)
- **Network Overhead**: ~200 bytes per request
- **Concurrent Users**: 1000+ supported

## Security Considerations:
- **Token Storage**: Secure client-side storage
- **Token Transmission**: HTTPS mandatory
- **Token Expiration**: Automatic refresh
- **Audit Logging**: Complete access records
- **Rate Limiting**: Prevent brute force attacks

## Implementation Details:
```python
# Token Configuration
JWT_CONFIG = {
    "algorithm": "HS256",
    "access_token_expire_minutes": 30,
    "refresh_token_expire_days": 7,
    "secret_key": os.getenv("SECRET_KEY"),
    "blacklist_enabled": True
}

# Permission Matrix
PERMISSIONS = {
    "super_admin": ["*"],
    "admin": ["config:*", "mission:*", "telemetry:*"],
    "operator": ["mission:read", "mission:write", "telemetry:read"],
    "viewer": ["mission:read", "telemetry:read"],
    "system": ["internal:*"]
}
```

## Alternatives Considered:
1. **Session-based**: Requires server-side storage
2. **API Keys**: Limited flexibility
3. **OAuth 2.0**: Overkill cho internal system
4. **Certificate-based**: Complex management

## Compliance Requirements:
- **SIL2 Safety**: Authentication không ảnh hưởng safety
- **Audit Trail**: Complete access logging
- **Access Control**: Role-based permissions
- **Security Standards**: Industry best practices

## Review Schedule:
- **Monthly**: Security audit
- **Quarterly**: Permission matrix review
- **Annually**: Authentication system evaluation

---

**Approved by:** CTO  
**Date:** 2024-08-21  
**Next Review:** 2024-09-21
