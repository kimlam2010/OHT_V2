# 🔗 BACKEND FRONTEND INTEGRATION PLAN - OHT-50

**Phiên bản:** 1.0  
**Ngày tạo:** 2025-01-28  
**Mục tiêu:** Khắc phục API gaps và tích hợp hoàn chỉnh với frontend Login & Dashboard

---

## 🚨 **CRITICAL API GAPS IDENTIFIED**

### **1. LOGIN PAGE (1-1-auth-login.html) - MISSING APIs**

#### **❌ Critical Missing APIs:**
- **POST /api/v1/auth/logout** - User logout functionality
- **GET /api/v1/auth/me** - Get current user info
- **POST /api/v1/auth/refresh** - Refresh JWT token
- **POST /api/v1/auth/forgot-password** - Password reset
- **POST /api/v1/auth/change-password** - Change password

#### **✅ Existing APIs:**
- **POST /api/v1/auth/login** - User login ✅
- **POST /api/v1/auth/register** - User registration ✅

---

### **2. DASHBOARD PAGE (2-1-dashboard-main.html) - MISSING APIs**

#### **❌ Critical Missing APIs:**

##### **Dashboard Summary:**
- **GET /api/v1/dashboard/summary** - Dashboard overview data
- **GET /api/v1/dashboard/alerts** - Active alerts list
- **GET /api/v1/dashboard/logs** - System logs
- **GET /api/v1/dashboard/performance** - Performance charts

##### **Robot Mode Control:**
- **POST /api/v1/robot/mode** - Set robot mode (AUTO/MANUAL/SEMI)
- **GET /api/v1/robot/mode** - Get current robot mode
- **POST /api/v1/robot/mode/validate** - Validate mode change

##### **Manual Control:**
- **POST /api/v1/robot/move/forward** - Move forward
- **POST /api/v1/robot/move/backward** - Move backward
- **POST /api/v1/robot/move/left** - Turn left
- **POST /api/v1/robot/move/right** - Turn right
- **POST /api/v1/robot/move/stop** - Stop movement

##### **Speed Control:**
- **POST /api/v1/robot/speed** - Set robot speed
- **GET /api/v1/robot/speed** - Get current speed
- **POST /api/v1/robot/speed/preset** - Set speed preset

##### **Emergency Controls:**
- **POST /api/v1/robot/emergency-stop** - Emergency stop
- **POST /api/v1/robot/pause** - Pause system
- **POST /api/v1/robot/reset** - Reset system

#### **✅ Existing APIs:**
- **GET /api/v1/robot/status** - Robot status ✅
- **GET /api/v1/robot/position** - Robot position ✅
- **POST /api/v1/robot/control** - Generic robot control ✅
- **GET /api/v1/monitoring/health** - System health ✅
- **GET /api/v1/monitoring/metrics** - Performance metrics ✅
- **WebSocket /ws/telemetry** - Real-time telemetry ✅
- **WebSocket /ws/alerts** - Real-time alerts ✅

---

## 📋 **IMPLEMENTATION PLAN**

### **PHASE 1: AUTHENTICATION API COMPLETION (Priority 1)**

#### **Sprint 1.1: Auth API Enhancement (2-3 days)**

##### **Day 1: Logout & User Info APIs**
```python
# app/api/v1/auth.py - ADD THESE ENDPOINTS:

@router.post("/logout")
async def logout(
    current_user: User = Depends(get_current_user)
):
    """User logout endpoint"""
    # Implementation: Invalidate token, clear session
    pass

@router.get("/me", response_model=UserResponse)
async def get_current_user_info(
    current_user: User = Depends(get_current_user)
):
    """Get current user information"""
    # Implementation: Return user profile
    pass

@router.post("/refresh")
async def refresh_token(
    refresh_data: RefreshTokenRequest,
    db: AsyncSession = Depends(get_db)
):
    """Refresh JWT token"""
    # Implementation: Validate refresh token, issue new access token
    pass
```

##### **Day 2: Password Management APIs**
```python
@router.post("/forgot-password")
async def forgot_password(
    email: str,
    db: AsyncSession = Depends(get_db)
):
    """Send password reset email"""
    # Implementation: Generate reset token, send email
    pass

@router.post("/change-password")
async def change_password(
    password_data: ChangePasswordRequest,
    current_user: User = Depends(get_current_user),
    db: AsyncSession = Depends(get_db)
):
    """Change user password"""
    # Implementation: Validate old password, set new password
    pass
```

##### **Day 3: Testing & Validation**
- Unit tests for all new auth endpoints
- Integration tests for auth flow
- Performance testing (< 50ms response time)

---

### **PHASE 2: DASHBOARD API IMPLEMENTATION (Priority 1)**

#### **Sprint 2.1: Dashboard Summary APIs (2-3 days)**

##### **Day 1: Dashboard Summary Endpoint**
```python
# app/api/v1/dashboard.py - CREATE NEW FILE:

@router.get("/summary")
async def get_dashboard_summary(
    current_user: User = Depends(require_permission("dashboard", "read"))
):
    """Get dashboard summary data"""
    # Implementation: Aggregate robot status, alerts, performance
    pass

@router.get("/alerts")
async def get_active_alerts(
    current_user: User = Depends(require_permission("dashboard", "read"))
):
    """Get active alerts for dashboard"""
    # Implementation: Return active alerts with severity
    pass

@router.get("/logs")
async def get_system_logs(
    limit: int = 50,
    level: str = "all",
    current_user: User = Depends(require_permission("dashboard", "read"))
):
    """Get system logs for dashboard"""
    # Implementation: Return filtered system logs
    pass
```

##### **Day 2: Performance & Metrics APIs**
```python
@router.get("/performance")
async def get_performance_data(
    hours: int = 24,
    current_user: User = Depends(require_permission("dashboard", "read"))
):
    """Get performance data for charts"""
    # Implementation: Return performance metrics for charts
    pass

@router.get("/metrics/realtime")
async def get_realtime_metrics(
    current_user: User = Depends(require_permission("dashboard", "read"))
):
    """Get real-time metrics for dashboard"""
    # Implementation: Return current system metrics
    pass
```

##### **Day 3: Testing & Integration**
- Unit tests for dashboard endpoints
- Integration tests with frontend
- Performance validation

---

### **PHASE 3: ROBOT CONTROL API ENHANCEMENT (Priority 1)**

#### **Sprint 3.1: Mode Control APIs (2 days)**

##### **Day 1: Robot Mode Management**
```python
# app/api/v1/robot.py - ADD THESE ENDPOINTS:

@router.post("/mode")
async def set_robot_mode(
    mode_data: RobotModeRequest,
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Set robot operating mode (AUTO/MANUAL/SEMI)"""
    # Implementation: Validate mode, update robot state
    pass

@router.get("/mode")
async def get_robot_mode(
    current_user: User = Depends(require_permission("robot", "read"))
):
    """Get current robot mode"""
    # Implementation: Return current robot mode
    pass

@router.post("/mode/validate")
async def validate_mode_change(
    mode_data: RobotModeRequest,
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Validate if mode change is allowed"""
    # Implementation: Check safety conditions, return validation result
    pass
```

##### **Day 2: Manual Control APIs**
```python
@router.post("/move/forward")
async def move_forward(
    speed: float = 1.0,
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Move robot forward"""
    # Implementation: Send forward movement command
    pass

@router.post("/move/backward")
async def move_backward(
    speed: float = 1.0,
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Move robot backward"""
    # Implementation: Send backward movement command
    pass

@router.post("/move/left")
async def turn_left(
    angle: float = 90.0,
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Turn robot left"""
    # Implementation: Send left turn command
    pass

@router.post("/move/right")
async def turn_right(
    angle: float = 90.0,
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Turn robot right"""
    # Implementation: Send right turn command
    pass

@router.post("/move/stop")
async def stop_movement(
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Stop robot movement"""
    # Implementation: Send stop command
    pass
```

---

### **PHASE 4: SPEED CONTROL & EMERGENCY APIs (Priority 1)**

#### **Sprint 4.1: Speed Control APIs (1 day)**

##### **Day 1: Speed Management**
```python
# app/api/v1/robot.py - ADD THESE ENDPOINTS:

@router.post("/speed")
async def set_robot_speed(
    speed_data: SpeedRequest,
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Set robot speed"""
    # Implementation: Validate speed, update robot speed
    pass

@router.get("/speed")
async def get_robot_speed(
    current_user: User = Depends(require_permission("robot", "read"))
):
    """Get current robot speed"""
    # Implementation: Return current robot speed
    pass

@router.post("/speed/preset")
async def set_speed_preset(
    preset: str,  # "slow", "normal", "fast", "max"
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Set speed preset"""
    # Implementation: Set predefined speed values
    pass
```

#### **Sprint 4.2: Emergency Control APIs (1 day)**

##### **Day 1: Emergency Controls**
```python
@router.post("/emergency-stop")
async def emergency_stop(
    current_user: User = Depends(require_permission("robot", "emergency"))
):
    """Emergency stop robot"""
    # Implementation: Immediate stop, safety procedures
    pass

@router.post("/pause")
async def pause_system(
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Pause robot system"""
    # Implementation: Pause operations, maintain state
    pass

@router.post("/reset")
async def reset_system(
    current_user: User = Depends(require_permission("robot", "control"))
):
    """Reset robot system"""
    # Implementation: Reset to safe state
    pass
```

---

## 📊 **IMPLEMENTATION TIMELINE**

### **Week 1: Authentication & Dashboard APIs**
- **Day 1-2:** Auth API completion (logout, user info, password management)
- **Day 3-4:** Dashboard summary APIs
- **Day 5:** Testing & integration

### **Week 2: Robot Control APIs**
- **Day 1-2:** Mode control APIs
- **Day 3-4:** Manual control APIs
- **Day 5:** Speed control & emergency APIs

### **Week 3: Testing & Integration**
- **Day 1-2:** Unit testing
- **Day 3-4:** Integration testing
- **Day 5:** Performance testing & validation

---

## 🎯 **SUCCESS CRITERIA**

### **API Completeness:**
- ✅ **100%** Login page APIs implemented
- ✅ **100%** Dashboard page APIs implemented
- ✅ **100%** Robot control APIs implemented
- ✅ **100%** Emergency control APIs implemented

### **Performance Targets:**
- ✅ **Response Time:** < 50ms for all new endpoints
- ✅ **Throughput:** 1000+ requests/second
- ✅ **Availability:** 99.9% uptime
- ✅ **Error Rate:** < 0.1%

### **Quality Standards:**
- ✅ **Test Coverage:** > 80% for new code
- ✅ **Code Review:** 100% mandatory
- ✅ **Documentation:** Complete API documentation
- ✅ **Security:** All endpoints properly secured

---

## 🔧 **TECHNICAL REQUIREMENTS**

### **New Files to Create:**
```
backend/app/api/v1/dashboard.py          # Dashboard APIs
backend/app/schemas/dashboard.py         # Dashboard schemas
backend/app/schemas/robot_control.py     # Robot control schemas
backend/tests/api/v1/test_dashboard.py   # Dashboard tests
backend/tests/api/v1/test_robot_control.py # Robot control tests
```

### **Files to Modify:**
```
backend/app/api/v1/auth.py               # Add missing auth endpoints
backend/app/api/v1/robot.py              # Add robot control endpoints
backend/app/main.py                      # Register new routers
backend/app/schemas/auth.py              # Add auth schemas
```

### **Database Changes:**
- No database schema changes required
- Use existing models and tables

---

## 🚨 **RISK MITIGATION**

### **Technical Risks:**
- **Risk:** API performance degradation
  - **Mitigation:** Performance testing, caching, optimization
- **Risk:** Security vulnerabilities
  - **Mitigation:** Security review, input validation, authentication
- **Risk:** Integration failures
  - **Mitigation:** Comprehensive testing, error handling

### **Schedule Risks:**
- **Risk:** Scope creep
  - **Mitigation:** Strict change control, weekly reviews
- **Risk:** Resource constraints
  - **Mitigation:** Cross-training, backup resources

---

## 📞 **COMMUNICATION PLAN**

### **Daily Standups:**
- **Time:** 9:00 AM daily
- **Duration:** 15 minutes
- **Focus:** Progress, blockers, next steps

### **Weekly Reviews:**
- **Time:** Friday 2:00 PM
- **Duration:** 1 hour
- **Focus:** Sprint review, planning, retrospective

### **Integration Testing:**
- **Time:** End of each sprint
- **Duration:** 2 hours
- **Focus:** Frontend-backend integration validation

---

## 📚 **RESOURCES & REFERENCES**

### **Documentation:**
- [Frontend Login Page](../superdesign/design_iterations/core_pages/1-1-auth-login.html)
- [Frontend Dashboard Page](../superdesign/design_iterations/core_pages/2-1-dashboard-main.html)
- [Backend API Documentation](../01-API-DOCUMENTATION/INDEX.md)
- [Backend Development Rules](../02-REQUIREMENTS/04-BACKEND-REQUIREMENTS/BACKEND_DEVELOPMENT_RULES.md)

### **Tools & Technologies:**
- **Framework:** FastAPI 0.104+
- **Database:** SQLite (development), PostgreSQL (production)
- **Testing:** pytest, pytest-asyncio
- **Documentation:** OpenAPI/Swagger

---

**🎯 Mục tiêu cuối cùng:** Hoàn thành 100% API gaps để frontend Login & Dashboard hoạt động hoàn chỉnh!

**📅 Timeline:** 3 tuần (21 ngày)  
**👥 Team:** Backend developers (2 người)  
**🎯 Success Criteria:** 100% API completeness, < 50ms response time, 99.9% uptime

---

**Changelog v1.0:**
- ✅ Created comprehensive API gap analysis
- ✅ Defined implementation plan with 4 phases
- ✅ Set clear success criteria and timeline
- ✅ Identified technical requirements and risks
- ✅ Established communication plan

**🚨 Lưu ý:** Plan này cần được thực hiện ngay lập tức để đảm bảo frontend hoạt động đầy đủ!
