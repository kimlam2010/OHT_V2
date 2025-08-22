---
title: "Adr 001 System Architecture"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "CTO Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['system-architecture']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# ADR-001: System Architecture Decision

## Status
**APPROVED** - 2024-08-21

## Context
OHT-50 Master Module cần một kiến trúc backend có khả năng:
- Xử lý real-time communication với hardware (RS485/Modbus)
- Quản lý mission và path planning
- Đảm bảo safety compliance (SIL2)
- Tích hợp với Center system
- Cung cấp real-time telemetry và monitoring
- Hỗ trợ multiple clients (Web UI, Mobile App)

## Decision
**Chọn kiến trúc Monolithic với Microservices-ready design**

### Core Architecture Components:
1. **FastAPI-based Backend** với modular service design
2. **Event-driven communication** giữa các services
3. **WebSocket real-time streaming** cho telemetry
4. **JWT-based authentication** với role-based access control
5. **PostgreSQL database** với Redis caching
6. **Docker containerization** cho deployment

### Service Architecture:
```
┌─────────────────────────────────────┐
│           API Gateway               │
│        (FastAPI + Middleware)       │
└─────────────────┬───────────────────┘
                  │
    ┌─────────────┼─────────────┐
    │             │             │
┌───▼───┐   ┌────▼────┐   ┌────▼────┐
│ Auth  │   │ Config  │   │Telemetry│
│Service│   │Service  │   │Service  │
└───────┘   └─────────┘   └─────────┘
    │             │             │
┌───▼───┐   ┌────▼────┐   ┌────▼────┐
│Mission│   │ Safety  │   │ Center  │
│Service│   │Service  │   │Service  │
└───────┘   └─────────┘   └─────────┘
    │             │             │
    └─────────────┼─────────────┘
                  │
            ┌─────▼─────┐
            │ Database  │
            │(PostgreSQL)│
            └───────────┘
```

## Consequences

### Positive:
- **FastAPI Performance**: High-performance async framework
- **Modular Design**: Easy to maintain và extend
- **Real-time Capability**: WebSocket support cho telemetry
- **Security**: JWT authentication với RBAC
- **Scalability**: Ready for microservices migration
- **Development Speed**: Rich ecosystem và tooling

### Negative:
- **Monolithic Risk**: Single point of failure
- **Complexity**: Multiple services trong single codebase
- **Testing Overhead**: Integration testing complexity
- **Deployment Complexity**: Container orchestration needed

### Risks:
- **Performance Bottleneck**: Database connection limits
- **Memory Usage**: Single process memory consumption
- **Deployment Risk**: Full system deployment required

## Mitigation Strategies:
1. **Database Optimization**: Connection pooling, indexing
2. **Caching Strategy**: Redis for frequently accessed data
3. **Monitoring**: Comprehensive metrics và alerting
4. **Gradual Migration**: Microservices migration path
5. **Load Testing**: Performance validation

## Performance Impact Assessment:
- **Response Time**: < 100ms cho API calls
- **Throughput**: 1000+ requests/second
- **Memory Usage**: < 512MB cho production
- **Database Connections**: Pool size 10-20
- **WebSocket Connections**: 100+ concurrent

## Alternatives Considered:
1. **Pure Microservices**: Too complex cho initial development
2. **Traditional REST API**: Lacks real-time capabilities
3. **GraphQL**: Overkill cho current requirements
4. **Event Sourcing**: Complex cho safety-critical systems

## Implementation Timeline:
- **Phase 1**: Core services (Auth, Config, Telemetry)
- **Phase 2**: Mission và Safety services
- **Phase 3**: Center integration
- **Phase 4**: Performance optimization

## Review Schedule:
- **Monthly**: Architecture performance review
- **Quarterly**: Scalability assessment
- **Annually**: Technology stack evaluation

---

**Approved by:** CTO  
**Date:** 2024-08-21  
**Next Review:** 2024-09-21
