# FIRMWARE MIGRATION EXECUTION GUIDE - OHT-50

**Phiên bản:** v1.0  
**Ngày tạo:** 2024-12-19  
**Chịu trách nhiệm:** FW Team  
**Trạng thái:** Ready for Execution

---

## 🚀 **QUICK START - MIGRATION EXECUTION**

### **Bước 1: Chuẩn bị môi trường**
```bash
# Cài đặt dependencies
sudo apt-get update
sudo apt-get install -y cmake build-essential clang-tidy cppcheck lcov pmccabe

# Kiểm tra firmware hiện tại
ls -la firmware/
```

### **Bước 2: Chạy migration script**
```bash
# Chạy migration helper
cd firmware/
./scripts/migration_helper.sh

# Hoặc nếu chưa có quyền execute
bash scripts/migration_helper.sh
```

### **Bước 3: Kiểm tra kết quả**
```bash
# Xem cấu trúc mới
ls -la firmware_new/

# Chạy analysis
cd firmware_new/
./scripts/code_analysis.sh
```

---

## 📋 **DETAILED EXECUTION STEPS**

### **Phase 1: Foundation Setup (Week 1-2)**

#### **Day 1-2: Toolchain Setup**
```bash
# 1. Backup current firmware
cp -r firmware firmware_backup_$(date +%Y%m%d)

# 2. Create new structure
mkdir -p firmware_iso/{src,include,tests,docs,scripts,third_party,config}

# 3. Setup CMake
cd firmware_iso
# Copy CMakeLists.txt đã tạo
```

#### **Day 3-4: Static Analysis Setup**
```bash
# 1. Install analysis tools
sudo apt-get install -y clang-tidy cppcheck lcov pmccabe

# 2. Configure tools
# .clang-format và .clang-tidy đã được tạo

# 3. Test analysis
./scripts/code_analysis.sh
```

#### **Day 5: CI/CD Setup**
```bash
# 1. Create GitHub Actions workflow
mkdir -p .github/workflows
# Copy firmware-ci.yml template

# 2. Test CI locally
act -j build
```

### **Phase 2: Code Migration (Week 3-4)**

#### **Week 3: HAL Layer Refactoring**
```bash
# 1. Migrate HAL files
cp firmware/src/hal/* firmware_iso/src/hal/common/
cp firmware/include/hal_*.h firmware_iso/src/hal/common/

# 2. Organize by category
mv firmware_iso/src/hal/common/hal_gpio.* firmware_iso/src/hal/gpio/
mv firmware_iso/src/hal/common/hal_rs485.* firmware_iso/src/hal/communication/
mv firmware_iso/src/hal/common/hal_estop.* firmware_iso/src/hal/safety/
# ... continue for other HAL modules

# 3. Update include paths
find firmware_iso/src -name "*.c" -exec sed -i 's|#include "hal_|#include "hal/common/hal_|g' {} \;
```

#### **Week 4: Application Layer Refactoring**
```bash
# 1. Migrate application files
cp firmware/src/app/* firmware_iso/src/app/core/
cp firmware/include/*.h firmware_iso/src/app/core/

# 2. Organize by category
mv firmware_iso/src/app/core/safety_*.h firmware_iso/src/app/managers/
mv firmware_iso/src/app/core/communication_*.h firmware_iso/src/app/managers/
mv firmware_iso/src/app/core/*_module_*.h firmware_iso/src/app/modules/
mv firmware_iso/src/app/core/api_*.h firmware_iso/src/app/api/

# 3. Update include paths
find firmware_iso/src -name "*.c" -exec sed -i 's|#include "|#include "app/core/|g' {} \;
```

### **Phase 3: Testing và Validation (Week 5-6)**

#### **Week 5: Test Framework Setup**
```bash
# 1. Setup Unity test framework
cd firmware_iso/third_party
git clone https://github.com/ThrowTheSwitch/Unity.git unity

# 2. Create test structure
mkdir -p tests/unit/{hal,app,utils}
mkdir -p tests/integration/{hal_integration,app_integration,system_integration}
mkdir -p tests/system/{safety_tests,performance_tests,stress_tests}

# 3. Write unit tests
# Example: tests/unit/hal/test_hal_gpio.c
```

#### **Week 6: Integration Testing**
```bash
# 1. Setup integration test environment
# Create test fixtures và mock objects

# 2. Write integration tests
# Test HAL ↔ APP integration

# 3. Performance testing
# Benchmark critical functions
```

### **Phase 4: Documentation và Deployment (Week 7)**

#### **Week 7: Finalization**
```bash
# 1. Generate API documentation
doxygen docs/doxygen.conf

# 2. Create user guides
# Write user manual và developer guide

# 3. Deploy và validate
./scripts/build.sh Release
./scripts/test.sh
```

---

## 🔧 **TOOLS AND COMMANDS**

### **Build Commands**
```bash
# Build firmware
cd firmware_iso
./scripts/build.sh Release

# Build with debug info
./scripts/build.sh Debug

# Clean build
rm -rf build/
./scripts/build.sh Release
```

### **Test Commands**
```bash
# Run all tests
./scripts/test.sh

# Run specific test categories
cd build
ctest -R unit_tests
ctest -R integration_tests
ctest -R system_tests
```

### **Analysis Commands**
```bash
# Run code analysis
./scripts/code_analysis.sh

# Run specific analysis
clang-tidy -p build src/**/*.c
cppcheck --enable=all src/ include/
```

### **Documentation Commands**
```bash
# Generate documentation
doxygen docs/doxygen.conf

# View documentation
firefox docs/html/index.html
```

---

## 📊 **QUALITY GATES**

### **Build Quality Gates**
- [ ] CMake configuration successful
- [ ] No compilation errors
- [ ] No linking errors
- [ ] All warnings addressed

### **Code Quality Gates**
- [ ] MISRA C:2012 compliance > 95%
- [ ] Code coverage > 90%
- [ ] Cyclomatic complexity < 10
- [ ] No high-severity static analysis issues

### **Test Quality Gates**
- [ ] All unit tests pass
- [ ] All integration tests pass
- [ ] All system tests pass
- [ ] Performance benchmarks met

### **Documentation Quality Gates**
- [ ] API documentation complete
- [ ] User guide complete
- [ ] Developer guide complete
- [ ] All diagrams updated

---

## 🚨 **TROUBLESHOOTING**

### **Common Issues**

#### **1. Build Failures**
```bash
# Problem: CMake configuration fails
# Solution: Check CMakeLists.txt syntax
cmake -B build -S . --debug-output

# Problem: Compilation errors
# Solution: Check include paths
find . -name "*.h" | grep -i "missing_header"
```

#### **2. Test Failures**
```bash
# Problem: Unit tests fail
# Solution: Check test dependencies
cd build
ctest --output-on-failure --verbose

# Problem: Integration tests fail
# Solution: Check mock objects
ls tests/mocks/
```

#### **3. Analysis Issues**
```bash
# Problem: clang-tidy errors
# Solution: Update compilation database
rm build/compile_commands.json
cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Problem: cppcheck false positives
# Solution: Add suppressions
# Add to .cppcheck-suppressions
```

#### **4. Performance Issues**
```bash
# Problem: Build time too long
# Solution: Use parallel builds
make -j$(nproc)

# Problem: Memory usage high
# Solution: Check for memory leaks
valgrind --leak-check=full ./build/bin/oht50_main
```

---

## 📈 **PROGRESS TRACKING**

### **Daily Progress Template**
```markdown
# Migration Progress - Day X

## Completed Today
- [ ] Task 1
- [ ] Task 2
- [ ] Task 3

## Issues Encountered
- Issue 1: Description and resolution
- Issue 2: Description and resolution

## Next Steps
- [ ] Tomorrow's task 1
- [ ] Tomorrow's task 2

## Metrics
- Files migrated: X/Y
- Tests passing: X/Y
- Code coverage: X%
- MISRA compliance: X%
```

### **Weekly Review Template**
```markdown
# Weekly Migration Review - Week X

## Achievements
- Major milestone 1
- Major milestone 2

## Challenges
- Challenge 1: Impact and mitigation
- Challenge 2: Impact and mitigation

## Next Week Plan
- Priority 1
- Priority 2
- Priority 3

## Risk Assessment
- Risk 1: Probability and impact
- Risk 2: Probability and impact
```

---

## ✅ **ACCEPTANCE CHECKLIST**

### **Phase 1 Acceptance**
- [ ] CMake build system working
- [ ] Static analysis tools configured
- [ ] Basic project structure in place
- [ ] Backup of original firmware created

### **Phase 2 Acceptance**
- [ ] All source files migrated
- [ ] HAL interfaces standardized
- [ ] Error handling implemented
- [ ] Include paths updated

### **Phase 3 Acceptance**
- [ ] > 90% code coverage
- [ ] All tests passing
- [ ] Performance benchmarks met
- [ ] Integration tests working

### **Phase 4 Acceptance**
- [ ] Documentation complete
- [ ] Deployment successful
- [ ] Team training completed
- [ ] Quality gates passed

---

## 📞 **SUPPORT AND ESCALATION**

### **Technical Support**
- **FW Team Lead:** [Contact information]
- **CTO:** [Contact information]
- **Embedded Expert:** [Contact information]

### **Escalation Matrix**
1. **Level 1:** FW Team internal resolution
2. **Level 2:** CTO review and decision
3. **Level 3:** Executive escalation

### **Emergency Procedures**
- **Build Failure:** Revert to backup, investigate root cause
- **Test Failure:** Stop migration, fix issues before continuing
- **Data Loss:** Restore from backup, verify integrity

---

**Changelog:**
- v1.0: Initial execution guide creation
- Added detailed step-by-step instructions
- Included troubleshooting section
- Added progress tracking templates
- Defined quality gates and acceptance criteria
