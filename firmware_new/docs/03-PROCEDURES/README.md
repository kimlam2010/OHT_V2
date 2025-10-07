# 🔧 03-PROCEDURES (Thủ Tục)

**Theo ISO 9001:2015 Section 8 - Operation**

---

## 🎯 **MỤC ĐÍCH**

Thư mục này chứa các **thủ tục chuẩn** cho quy trình phát triển, build, và quality control.

---

## 📄 **DANH SÁCH TÀI LIỆU**

| **Mã** | **Tên tài liệu** | **Mô tả** | **Version** |
|--------|------------------|-----------|-------------|
| **PR-001** | Build_Procedure.md | Quy trình build firmware (CMake, options) | 1.0 |
| **PR-002** | Contributing_Procedure.md | Quy trình đóng góp (Git workflow, PR) | 1.0.1 |
| **PR-003** | Code_Quality_Procedure.md | Quy trình quality control (format, lint) | 1.0 |

---

## 🏗️ **PR-001: Build Procedure**

**Nội dung:**
- Prerequisites (system + software requirements)
- Quick start (4 bước: clone → configure → build → run)
- Build configurations (Debug, Release, RelWithDebInfo)
- Build options (Coverage, Sanitizers, Static Analysis)
- Cross-compilation cho ARM
- Installation procedures
- Troubleshooting build issues

**Khi nào dùng:** Khi build firmware lần đầu hoặc build với config mới

---

## 🤝 **PR-002: Contributing Procedure**

**Nội dung:**
- Code of Conduct
- Code style (LLVM, naming conventions)
- Git workflow (Fork → Branch → Commit → PR)
- Commit message convention
- PR process (template, review, approval)
- Bug report và feature request templates
- Testing requirements (>80% coverage)
- Documentation standards

**Khi nào dùng:** Trước khi contribute code, tạo PR

---

## ✨ **PR-003: Code Quality Procedure**

**Nội dung:**
- Code formatting (clang-format)
- Static analysis (clang-tidy)
- Code coverage (gcov/lcov)
- Sanitizers (ASan/UBSan)
- Compiler warnings (15+ flags)
- Pre-commit checks
- CI/CD pipeline
- Quality metrics

**Khi nào dùng:** Development, code review, CI/CD setup

---

## ✅ **TUÂN THỦ**

Procedures phải được tuân thủ **BẮT BUỘC** cho:
- ✅ Pull requests
- ✅ Code reviews
- ✅ Quality gates
- ✅ Release process

---

**Version:** 1.0  
**Last Updated:** 2025-10-07  
**Maintained By:** Process Team

