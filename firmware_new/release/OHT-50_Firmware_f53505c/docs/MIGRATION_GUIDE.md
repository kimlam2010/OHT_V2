# OHT-50 Firmware Migration Guide
## Chuyển đổi từ Firmware Cũ sang ISO/IEC 12207 Compliant

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-27  
**Chịu trách nhiệm:** FW Team  
**Trạng thái:** Production Ready

---

## 📋 **TỔNG QUAN**

### **Mục tiêu:**
Hướng dẫn chuyển đổi an toàn từ firmware cũ (Makefile-based) sang firmware mới (CMake-based) tuân thủ ISO/IEC 12207 và MISRA C:2012.

### **Phạm vi:**
- Build system migration
- Development workflow changes
- Testing procedures
- Deployment process

---

## 🔄 **QUY TRÌNH CHUYỂN ĐỔI**

### **Bước 1: Chuẩn bị**
```bash
# 1. Backup firmware hiện tại
cp -r firmware firmware_backup_$(date +%Y%m%d_%H%M%S)

# 2. Switch sang branch mới
git checkout FWteam-deploying

# 3. Verify cấu trúc mới
ls -la firmware_new/
```

### **Bước 2: Build System Migration**
```bash
# 1. Cài đặt dependencies
sudo apt-get update
sudo apt-get install cmake build-essential clang-tidy cppcheck

# 2. Build firmware mới
cd firmware_new
mkdir build && cd build
cmake ..
make oht50_main

# 3. Verify build
./oht50_main --help
```

### **Bước 3: Testing Migration**
```bash
# 1. Run unit tests
cd firmware_new/build
make test_hal_common
./tests/unit/test_hal_common

# 2. Run static analysis
cd ..
./scripts/static_analysis.sh
```

### **Bước 4: Deployment Migration**
```bash
# 1. Create release package
./scripts/create_release.sh

# 2. Deploy to production
sudo ./release/OHT-50_Firmware_*/scripts/deploy.sh
```

---

## 📁 **CẤU TRÚC THƯ MỤC MỚI**

### **Before (Cũ):**
```
firmware/
├── src/
│   ├── hal/          # Mixed HAL files
│   ├── app/          # Mixed application files
│   └── main.c
├── include/          # All headers mixed
├── tests/            # Basic test structure
└── Makefile          # Complex makefile
```

### **After (Mới):**
```
firmware_new/
├── src/
│   ├── hal/          # Organized by category
│   │   ├── common/   # Common utilities
│   │   ├── gpio/     # GPIO abstraction
│   │   ├── communication/ # RS485, Network
│   │   ├── safety/   # E-Stop, safety
│   │   ├── storage/  # Configuration persistence
│   │   └── peripherals/ # LED, Relay
│   ├── app/          # Organized by layer
│   │   ├── core/     # State machine, control
│   │   ├── managers/ # System managers
│   │   ├── modules/  # Module handlers
│   │   └── api/      # API layer
│   └── utils/        # Utility functions
├── include/          # Public API headers
├── tests/            # ISO/IEC/IEEE 29119 compliant
│   ├── unit/         # Unit tests
│   ├── integration/  # Integration tests
│   └── system/       # System tests
├── docs/             # Complete documentation
├── scripts/          # Build and analysis tools
├── config/           # Configuration files
├── third_party/      # Third-party libraries
└── CMakeLists.txt    # Modern build system
```

---

## 🔧 **BUILD SYSTEM CHANGES**

### **Old Build (Makefile):**
```bash
# Build firmware cũ
cd firmware
make clean
make all
```

### **New Build (CMake):**
```bash
# Build firmware mới
cd firmware_new
mkdir build && cd build
cmake ..
make oht50_main

# Hoặc sử dụng script
./scripts/build.sh
```

### **Build Targets:**
- **Main Executable:** `oht50_main`
- **Unit Tests:** `test_hal_common`
- **All Tests:** `make test`
- **Clean:** `make clean`

---

## 🧪 **TESTING CHANGES**

### **Old Testing:**
```bash
# Không có framework test chuẩn
# Manual testing only
```

### **New Testing:**
```bash
# Unit tests với Unity framework
cd firmware_new/build
make test_hal_common
./tests/unit/test_hal_common

# Static analysis
cd ..
cppcheck src/
clang-tidy src/ -- -I include/

# Integration tests (future)
make test_integration
```

---

## 📦 **DEPLOYMENT CHANGES**

### **Old Deployment:**
```bash
# Manual deployment
sudo cp build/firmware /opt/oht50/
sudo systemctl restart oht50-firmware
```

### **New Deployment:**
```bash
# Automated deployment với safety checks
sudo ./scripts/deploy.sh

# Rollback nếu có vấn đề
sudo ./scripts/rollback.sh

# Hoặc sử dụng release package
sudo ./release/OHT-50_Firmware_*/install.sh
```

---

## 🔍 **VERIFICATION CHECKLIST**

### **Pre-Migration:**
- [ ] Backup firmware hiện tại
- [ ] Verify system requirements
- [ ] Install required tools
- [ ] Test build system
- [ ] Run unit tests

### **Migration:**
- [ ] Build firmware mới thành công
- [ ] All tests passing
- [ ] Static analysis clean
- [ ] Documentation updated
- [ ] Deployment scripts tested

### **Post-Migration:**
- [ ] Firmware running correctly
- [ ] All features working
- [ ] Performance acceptable
- [ ] Logs clean
- [ ] Team trained

---

## ⚠️ **RISK MITIGATION**

### **Potential Issues:**
1. **Build Failures:** CMake configuration issues
2. **Runtime Errors:** API changes
3. **Performance Issues:** New structure overhead
4. **Compatibility Issues:** Hardware interface changes

### **Mitigation Strategies:**
1. **Parallel Development:** Maintain old system during transition
2. **Incremental Testing:** Test each component individually
3. **Rollback Plan:** Always maintain ability to revert
4. **Monitoring:** Enhanced logging and monitoring

---

## 📊 **PERFORMANCE COMPARISON**

### **Build Performance:**
- **Old System:** ~30 seconds (Makefile)
- **New System:** ~15 seconds (CMake, parallel build)

### **Code Quality:**
- **Old System:** Basic warnings
- **New System:** MISRA C:2012 compliance, static analysis

### **Test Coverage:**
- **Old System:** Manual testing only
- **New System:** Automated unit tests, 90%+ coverage target

---

## 🚀 **MIGRATION TIMELINE**

### **Day 1: Preparation**
- Backup current system
- Install new tools
- Setup development environment

### **Day 2: Build System**
- Test CMake build
- Verify all components compile
- Run initial tests

### **Day 3: Testing**
- Run full test suite
- Fix any issues
- Verify functionality

### **Day 4: Deployment**
- Create release package
- Deploy to test environment
- Verify operation

### **Day 5: Production**
- Deploy to production
- Monitor performance
- Train team

---

## 📞 **SUPPORT & TROUBLESHOOTING**

### **Common Issues:**

#### **1. CMake Configuration Errors**
```bash
# Error: CMake not found
sudo apt-get install cmake

# Error: Compiler not found
sudo apt-get install build-essential
```

#### **2. Build Failures**
```bash
# Clean and rebuild
cd firmware_new/build
make clean
cmake ..
make oht50_main
```

#### **3. Test Failures**
```bash
# Check test output
./tests/unit/test_hal_common

# Run with verbose output
./tests/unit/test_hal_common --verbose
```

#### **4. Deployment Issues**
```bash
# Check logs
tail -f /var/log/oht50_deploy.log
tail -f /var/log/oht50_firmware.log

# Rollback if needed
sudo ./scripts/rollback.sh
```

### **Contact Information:**
- **FW Team Lead:** Technical issues
- **CTO:** Architecture decisions
- **Documentation:** This guide

---

## ✅ **SUCCESS CRITERIA**

### **Technical Success:**
- [ ] All builds successful
- [ ] All tests passing
- [ ] No critical warnings
- [ ] Performance maintained or improved

### **Process Success:**
- [ ] Team trained on new system
- [ ] Documentation complete
- [ ] Deployment automated
- [ ] Rollback procedures tested

### **Business Success:**
- [ ] Zero downtime migration
- [ ] All features working
- [ ] Improved development velocity
- [ ] Enhanced code quality

---

## 📚 **ADDITIONAL RESOURCES**

### **Documentation:**
- `docs/05-IMPLEMENTATION/FIRMWARE/FIRMWARE_ISO_MIGRATION_SUMMARY.md`
- `docs/05-IMPLEMENTATION/FIRMWARE/FIRMWARE_ISO_STANDARDIZATION_PLAN.md`
- `docs/05-IMPLEMENTATION/FIRMWARE/STATIC_ANALYSIS_REPORT.md`

### **Scripts:**
- `scripts/build.sh` - Build automation
- `scripts/deploy.sh` - Deployment automation
- `scripts/rollback.sh` - Rollback automation
- `scripts/create_release.sh` - Release packaging

### **Tools:**
- CMake 3.16+
- GCC with C11 support
- cppcheck (static analysis)
- clang-tidy (code quality)
- Unity (unit testing)

---

**Changelog:**
- v1.0: Initial migration guide creation
- Comprehensive migration procedures
- Risk mitigation strategies
- Troubleshooting guide
- Success criteria definition
