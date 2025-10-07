# 🤝 Hướng Dẫn Đóng Góp - OHT-50 Firmware

**Version:** 1.0.1  
**Last Updated:** 2025-10-07

---

## 📋 **MỤC LỤC**

1. [Quy Tắc Chung](#quy-tắc-chung)
2. [Code Style](#code-style)
3. [Quy Trình Phát Triển](#quy-trình-phát-triển)
4. [Pull Request Process](#pull-request-process)
5. [Báo Cáo Lỗi](#báo-cáo-lỗi)
6. [Đề Xuất Tính Năng](#đề-xuất-tính-năng)
7. [Testing Requirements](#testing-requirements)
8. [Documentation](#documentation)

---

## 📜 **QUY TẮC CHUNG**

### **Code of Conduct**

Khi đóng góp vào dự án, bạn đồng ý:

✅ Tôn trọng tất cả contributors  
✅ Chấp nhận constructive criticism  
✅ Focus vào điều tốt nhất cho cộng đồng  
✅ Thể hiện empathy với contributors khác  
✅ Tuân thủ các quy tắc coding standards

❌ Không sử dụng ngôn ngữ offensive hoặc imagery  
❌ Không trolling, insulting, hoặc derogatory comments  
❌ Không public hoặc private harassment  
❌ Không publish thông tin cá nhân của người khác

### **Ai Có Thể Đóng Góp?**

Tất cả mọi người! Chúng tôi welcome:

- 🐛 **Bug fixes** - Sửa lỗi, cải thiện stability
- ✨ **New features** - Thêm tính năng mới
- 📚 **Documentation** - Cải thiện docs, examples
- 🧪 **Tests** - Thêm test cases, coverage
- 🎨 **Code quality** - Refactoring, optimization
- 🌐 **Translations** - Dịch sang ngôn ngữ khác

---

## 🎨 **CODE STYLE**

### **1. C Code Style**

Chúng tôi sử dụng **LLVM style** với một số customization:

#### **1.1. File Structure**

```c
// Header guard
#ifndef HAL_MODULE_NAME_H
#define HAL_MODULE_NAME_H

// Includes (system → project)
#include <stdint.h>
#include <stdbool.h>
#include "hal_common.h"

// Constants
#define MODULE_CONSTANT 100

// Type definitions
typedef struct {
    uint32_t field1;
    bool field2;
} module_config_t;

// Function declarations
hal_status_t module_init(module_config_t *config);
hal_status_t module_update(void);
hal_status_t module_deinit(void);

#endif // HAL_MODULE_NAME_H
```

#### **1.2. Naming Conventions**

| **Type** | **Convention** | **Example** |
|----------|----------------|-------------|
| **Functions** | `snake_case` | `hal_rs485_init()` |
| **Variables** | `snake_case` | `module_count` |
| **Constants** | `UPPER_SNAKE_CASE` | `MAX_MODULES` |
| **Types** | `snake_case_t` | `module_config_t` |
| **Enums** | `UPPER_SNAKE_CASE` | `MODULE_TYPE_POWER` |
| **Structs** | `snake_case_t` | `safety_status_t` |

#### **1.3. Formatting Rules**

```c
// Indentation: 4 spaces (NO TABS)
void function_name(int param1, int param2) {
    if (condition) {
        // Code block
        int result = param1 + param2;
        return result;
    }
}

// Braces: Same line for functions, if/else, loops
if (condition) {
    // true case
} else {
    // false case
}

// Line length: Max 100 characters
// Break long lines:
int result = some_long_function_name(
    parameter1, parameter2, parameter3, parameter4);

// Comments: Use // for single line, /* */ for multi-line
// Single line comment

/*
 * Multi-line comment
 * Explains complex logic
 */

// Function comments: Doxygen style
/**
 * @brief Initialize RS485 communication
 * @param config Configuration parameters
 * @return HAL_STATUS_OK on success, error code otherwise
 */
hal_status_t hal_rs485_init(rs485_config_t *config);
```

#### **1.4. Auto-formatting**

```bash
# Format single file
clang-format -i src/hal/hal_module.c

# Format all files
find src -name "*.c" -o -name "*.h" | xargs clang-format -i

# Check formatting (CI)
clang-format --dry-run --Werror src/**/*.c
```

### **2. CMake Style**

```cmake
# Function names: lowercase with underscores
add_library(module_name STATIC
    file1.c
    file2.c
)

# Variables: UPPERCASE
set(MODULE_VERSION "1.0.0")

# Indentation: 4 spaces
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
```

### **3. YAML Style**

```yaml
# Indentation: 2 spaces
modules:
  - address: 0x02
    type: POWER
    enabled: true
```

---

## 🔄 **QUY TRÌNH PHÁT TRIỂN**

### **1. Fork và Clone**

```bash
# Fork trên GitHub UI

# Clone fork về local
git clone https://github.com/YOUR_USERNAME/OHT-50-firmware.git
cd OHT-50-firmware/firmware_new

# Add upstream remote
git remote add upstream https://github.com/ORIGINAL_ORG/OHT-50-firmware.git

# Verify remotes
git remote -v
```

### **2. Create Branch**

```bash
# Update main
git checkout main
git pull upstream main

# Create feature branch
git checkout -b feature/my-awesome-feature

# Hoặc bugfix branch
git checkout -b bugfix/fix-rs485-timeout
```

**Branch naming convention:**
- `feature/description` - Tính năng mới
- `bugfix/description` - Sửa lỗi
- `hotfix/description` - Sửa lỗi khẩn cấp
- `docs/description` - Cập nhật docs
- `refactor/description` - Refactoring code

### **3. Make Changes**

```bash
# Code your changes
vim src/hal/hal_module.c

# Test your changes
mkdir build && cd build
cmake ..
make -j$(nproc)
./oht50_main --dry-run

# Run tests
make test
```

### **4. Commit Changes**

#### **4.1. Commit Message Format**

```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types:**
- `feat` - Tính năng mới
- `fix` - Sửa lỗi
- `docs` - Cập nhật documentation
- `style` - Formatting, missing semicolons, etc.
- `refactor` - Code refactoring
- `test` - Adding tests
- `chore` - Build process, tools, etc.

**Examples:**

```bash
# Good commit message
git commit -m "feat(rs485): add CRC validation for Modbus frames

- Implement CRC16 algorithm
- Add unit tests for CRC validation
- Update documentation

Fixes #123"

# Another good example
git commit -m "fix(safety): correct E-Stop response time

Response time was 150ms, now reduced to 80ms by
optimizing interrupt handler.

Related to #456"
```

#### **4.2. Commit Guidelines**

✅ **DO:**
- Write clear, concise commit messages
- Use present tense ("add feature" not "added feature")
- Reference issue numbers
- Keep commits atomic (one logical change per commit)
- Test before committing

❌ **DON'T:**
- Commit broken code
- Mix multiple unrelated changes
- Use vague messages ("fix stuff", "update")
- Commit generated files (build artifacts)

### **5. Push và Create PR**

```bash
# Push to your fork
git push origin feature/my-awesome-feature

# Create Pull Request trên GitHub UI
```

---

## 📝 **PULL REQUEST PROCESS**

### **1. Before Creating PR**

**Checklist:**

```bash
# ✅ Code builds without errors
mkdir build && cd build
cmake .. && make -j$(nproc)

# ✅ All tests pass
make test

# ✅ Code formatted
clang-format -i src/**/*.c

# ✅ No lint errors
clang-tidy src/**/*.c

# ✅ Documentation updated
# - README.md
# - API_REFERENCE.md
# - CHANGELOG.md

# ✅ Commit messages follow convention
git log --oneline -n 5
```

### **2. PR Template**

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix (non-breaking change)
- [ ] New feature (non-breaking change)
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Unit tests added/updated
- [ ] Integration tests pass
- [ ] Manual testing completed

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] No new warnings
- [ ] Tests added/updated
- [ ] Passes all CI checks

## Related Issues
Fixes #123
Related to #456

## Screenshots (if applicable)
```

### **3. PR Review Process**

1. **Automated Checks** (CI/CD)
   - ✅ Build successful
   - ✅ All tests pass
   - ✅ Code coverage acceptable
   - ✅ No security vulnerabilities

2. **Manual Review** (Reviewers)
   - 👀 Code quality
   - 🔍 Logic correctness
   - 📚 Documentation completeness
   - 🧪 Test coverage

3. **Approval**
   - ✅ Minimum 1 approval required
   - ✅ All comments addressed
   - ✅ CI checks pass

4. **Merge**
   - Squash and merge (default)
   - Merge commit (for large features)
   - Rebase and merge (linear history)

### **4. After Merge**

```bash
# Update your local main
git checkout main
git pull upstream main

# Delete feature branch
git branch -d feature/my-awesome-feature
git push origin --delete feature/my-awesome-feature
```

---

## 🐛 **BÁO CÁO LỖI**

### **Bug Report Template**

```markdown
## Bug Description
Clear description of the bug

## Steps to Reproduce
1. Go to '...'
2. Click on '....'
3. Scroll down to '....'
4. See error

## Expected Behavior
What you expected to happen

## Actual Behavior
What actually happened

## Environment
- **Firmware Version:** 1.0.1
- **Platform:** Orange Pi 5B
- **OS:** Ubuntu 22.04
- **Build Type:** Debug/Release

## Logs
```
[Paste relevant logs here]
```

## Screenshots
If applicable, add screenshots

## Additional Context
Any other context about the problem
```

### **Bug Reporting Guidelines**

✅ **Good Bug Reports:**
- Clear, concise description
- Steps to reproduce
- Expected vs actual behavior
- Logs và screenshots
- Environment information

❌ **Bad Bug Reports:**
- "It doesn't work"
- "Fix this bug"
- No details, no logs
- Can't reproduce

---

## ✨ **ĐỀ XUẤT TÍNH NĂNG**

### **Feature Request Template**

```markdown
## Feature Description
Clear description of the proposed feature

## Problem Statement
What problem does this solve?

## Proposed Solution
How do you propose to implement this?

## Alternatives Considered
What other approaches did you consider?

## Additional Context
Any other context, screenshots, mockups

## Implementation Checklist
- [ ] Design document
- [ ] API design
- [ ] Implementation plan
- [ ] Test plan
- [ ] Documentation plan
```

---

## 🧪 **TESTING REQUIREMENTS**

### **1. Unit Tests**

```c
// Test file: tests/unit/test_module.c
#include "unity.h"
#include "hal_module.h"

void setUp(void) {
    // Setup before each test
}

void tearDown(void) {
    // Cleanup after each test
}

void test_module_init_success(void) {
    module_config_t config = {0};
    hal_status_t result = module_init(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
}

void test_module_init_null_config(void) {
    hal_status_t result = module_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_ARG, result);
}
```

### **2. Integration Tests**

```bash
# Run integration tests
cd tests/integration
./test_rs485_communication.sh
./test_module_discovery.sh
```

### **3. Test Coverage**

```bash
# Build with coverage
cmake -DENABLE_COVERAGE=ON ..
make

# Run tests
make test

# Generate coverage report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report

# Target: > 80% coverage
```

---

## 📚 **DOCUMENTATION**

### **1. Code Documentation**

```c
/**
 * @brief Initialize RS485 communication
 * 
 * This function initializes the RS485 hardware and configures
 * the UART with specified parameters.
 * 
 * @param config Pointer to configuration structure
 * @return HAL_STATUS_OK on success
 * @return HAL_STATUS_INVALID_ARG if config is NULL
 * @return HAL_STATUS_ERROR on hardware failure
 * 
 * @note This function must be called before any RS485 operations
 * @warning Config must remain valid during operation
 * 
 * @example
 * rs485_config_t config = {
 *     .baud_rate = 115200,
 *     .data_bits = 8
 * };
 * hal_status_t result = hal_rs485_init(&config);
 */
hal_status_t hal_rs485_init(rs485_config_t *config);
```

### **2. API Documentation**

Update `API_REFERENCE.md` khi thêm/sửa API:

```markdown
## POST /api/v1/robot/command

Execute robot command.

### Request

```json
{
  "command": "move",
  "parameters": {
    "velocity": 1000,
    "position": 5000
  }
}
```

### Response

```json
{
  "success": true,
  "message": "Command executed"
}
```

### Errors

- `400` - Invalid command
- `500` - Execution failed
```

---

## 🎯 **REVIEW CHECKLIST**

Khi review PR, check:

### **Code Quality**
- [ ] Follows coding style
- [ ] No duplicate code
- [ ] No magic numbers (use constants)
- [ ] Error handling comprehensive
- [ ] No memory leaks
- [ ] Thread-safe (if applicable)

### **Testing**
- [ ] Unit tests added
- [ ] Integration tests pass
- [ ] Manual testing done
- [ ] Edge cases covered

### **Documentation**
- [ ] Code comments clear
- [ ] API docs updated
- [ ] README updated (if needed)
- [ ] CHANGELOG updated

### **Performance**
- [ ] No performance regression
- [ ] Resource usage acceptable
- [ ] Scalability considered

### **Security**
- [ ] No security vulnerabilities
- [ ] Input validation added
- [ ] No hardcoded secrets

---

## 🏆 **RECOGNITION**

Contributors được ghi nhận trong:
- `CONTRIBUTORS.md` - Danh sách contributors
- Release notes
- GitHub contributors page

Top contributors có thể được:
- 🌟 Invited làm maintainer
- 🎁 Swag (stickers, t-shirts)
- 📣 Featured trên blog/social media

---

## 📞 **LIÊN HỆ**

Có câu hỏi về contribution?

- 💬 **Slack:** #oht50-contributors
- 📧 **Email:** contributors@oht50.com
- 🐛 **Issues:** GitHub Issues
- 💡 **Discussions:** GitHub Discussions

---

**Thank you for contributing to OHT-50! 🚀**

**Version:** 1.0.1  
**Last Updated:** 2025-10-07  
**Maintained By:** OHT-50 Core Team

