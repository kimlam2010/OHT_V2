# OHT-50 Backend Documentation

## 📁 Cấu trúc thư mục

```
docs/backend/
├── README.md                    # 📖 Tài liệu này
├── DEPLOYMENT.md               # 🚀 Hướng dẫn triển khai
├── RELEASE_NOTES.md            # 📝 Ghi chú phát hành
├── ERRORS_CONVENTION.md        # ⚠️ Quy ước xử lý lỗi
├── BE-02_COMPLETION_SUMMARY.md # ✅ Báo cáo hoàn thành CI/CD
│
├── api-specs/                  # 📋 Đặc tả API
│   ├── README.md              # Index cho API specs
│   ├── CONFIG_API.md          # API cấu hình hệ thống
│   ├── TELEMETRY_API.md       # API telemetry
│   ├── WEBSOCKET_EVENTS.md    # WebSocket events
│   └── INTEGRATION_CENTER.md  # Tích hợp với Center
│
├── operational/                # 🔧 Vận hành & Bảo mật
│   ├── README.md              # Index cho operational docs
│   ├── SECURITY_GUIDE.md      # Hướng dẫn bảo mật
│   ├── HARDENING_CHECKLIST.md # Checklist hardening
│   ├── RUNBOOK.md             # Runbook sự cố
│   ├── SLO_SLA.md             # Service level objectives
│   └── ERROR_MATRIX.md        # Ma trận lỗi & edge cases
│
├── guides/                     # 📚 Hướng dẫn phát triển
│   ├── README.md              # Index cho development guides
│   ├── OPENAPI_EXAMPLES_GUIDE.md # Hướng dẫn thêm examples
│   ├── OPENAPI_REVIEW.md      # Review OpenAPI schema
│   └── CI_DOCS_CHECK_GUIDE.md # CI/CD cho documentation
│
└── tools/                      # 🛠️ Công cụ testing
    ├── README.md              # Index cho testing tools
    ├── postman_collection.json # Postman collection
    ├── INSOMNIA_WORKSPACE.json # Insomnia workspace
    └── curl_suite.sh          # Script test nhanh
```

## 🚀 Quick Start

### Cho Developers
1. **API Reference**: Xem [api-specs/](./api-specs/) cho endpoint details
2. **Testing**: Import [tools/postman_collection.json](./tools/postman_collection.json) hoặc [tools/INSOMNIA_WORKSPACE.json](./tools/INSOMNIA_WORKSPACE.json)
3. **Examples**: Tham khảo [guides/OPENAPI_EXAMPLES_GUIDE.md](./guides/OPENAPI_EXAMPLES_GUIDE.md)
4. **Error Handling**: Xem [ERRORS_CONVENTION.md](./ERRORS_CONVENTION.md)

### Cho DevOps/Operations
1. **Deployment**: Xem [DEPLOYMENT.md](./DEPLOYMENT.md)
2. **Security**: Review [operational/SECURITY_GUIDE.md](./operational/SECURITY_GUIDE.md) và [operational/HARDENING_CHECKLIST.md](./operational/HARDENING_CHECKLIST.md)
3. **Incidents**: Sử dụng [operational/RUNBOOK.md](./operational/RUNBOOK.md)
4. **Monitoring**: Xem [operational/SLO_SLA.md](./operational/SLO_SLA.md)

### Cho QA/Testing
1. **Error Cases**: Xem [operational/ERROR_MATRIX.md](./operational/ERROR_MATRIX.md)
2. **API Testing**: Sử dụng tools trong [tools/](./tools/)
3. **Validation**: Follow [guides/CI_DOCS_CHECK_GUIDE.md](./guides/CI_DOCS_CHECK_GUIDE.md)
4. **Test Scenarios**: Xem [tools/README.md](./tools/README.md)

### Cho PM/CTO
1. **Overview**: Xem [BE-02_COMPLETION_SUMMARY.md](./BE-02_COMPLETION_SUMMARY.md)
2. **Release Info**: Xem [RELEASE_NOTES.md](./RELEASE_NOTES.md)
3. **Performance**: Xem [operational/SLO_SLA.md](./operational/SLO_SLA.md)
4. **Security**: Xem [operational/SECURITY_GUIDE.md](./operational/SECURITY_GUIDE.md)

## 📋 Maintenance

### Cập nhật tài liệu
- **API changes** → Update [api-specs/](./api-specs/)
- **Security updates** → Update [operational/](./operational/)
- **New tools** → Add to [tools/](./tools/)
- **Process changes** → Update [guides/](./guides/)

### Validation
- Chạy CI docs-check theo [guides/CI_DOCS_CHECK_GUIDE.md](./guides/CI_DOCS_CHECK_GUIDE.md)
- Verify links trong tất cả files
- Test examples với actual API
- Update cross-references khi cần

## 🔗 Liên kết quan trọng

### External Links
- **OpenAPI UI**: `http://localhost:8000/docs`
- **Health Check**: `http://localhost:8000/health/`
- **Telemetry Schema**: `../specs/telemetry_schema.md`

### Internal Navigation
- **API Specifications**: [api-specs/README.md](./api-specs/README.md)
- **Operational Docs**: [operational/README.md](./operational/README.md)
- **Development Guides**: [guides/README.md](./guides/README.md)
- **Testing Tools**: [tools/README.md](./tools/README.md)

## 📊 Documentation Health

### Coverage Status
- ✅ **API Specs**: 100% endpoints documented
- ✅ **Error Handling**: Complete error matrix
- ✅ **Security**: Comprehensive security guide
- ✅ **Testing**: Full test coverage
- ✅ **Operational**: Complete runbooks và procedures

### Quality Metrics
- **Cross-references**: All links verified
- **Examples**: All API examples tested
- **Validation**: CI pipeline green
- **Maintenance**: Up-to-date với current codebase

## 🚨 Emergency Contacts

### Technical Issues
- **API Problems**: Check [operational/RUNBOOK.md](./operational/RUNBOOK.md)
- **Security Issues**: Follow [operational/SECURITY_GUIDE.md](./operational/SECURITY_GUIDE.md)
- **Deployment Issues**: See [DEPLOYMENT.md](./DEPLOYMENT.md)

### Documentation Issues
- **Missing Docs**: Create theo [guides/OPENAPI_EXAMPLES_GUIDE.md](./guides/OPENAPI_EXAMPLES_GUIDE.md)
- **Broken Links**: Fix và validate với [guides/CI_DOCS_CHECK_GUIDE.md](./guides/CI_DOCS_CHECK_GUIDE.md)
- **Outdated Content**: Update và verify với actual API
