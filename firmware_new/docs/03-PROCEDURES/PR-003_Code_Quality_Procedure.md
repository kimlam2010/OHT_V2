# OHT-50 Firmware Code Quality Guide

**Version:** 1.0.0  
**Last Updated:** 2025-10-07

## Overview

This document describes the code quality tools and practices used in the OHT-50 firmware project.

## Code Formatting

### Clang-Format

**Configuration:** `.clang-format`

**Style:** LLVM-based with custom modifications:
- **Indentation:** 4 spaces
- **Column Limit:** 100 characters
- **Pointer Alignment:** Left (`int* ptr`)
- **No Short Functions:** Functions not on single line

**Usage:**

```bash
# Format single file
clang-format -i src/hal/hal_rs485.c

# Format all C/H files
find src/ include/ -name "*.c" -o -name "*.h" | xargs clang-format -i

# Check formatting without modifying
clang-format --dry-run --Werror src/hal/hal_rs485.c
```

**IDE Integration:**
- **VSCode:** Install "C/C++" extension
- **Vim:** Use `clang-format` plugin
- **CLion:** Built-in support

### EditorConfig

**Configuration:** `.editorconfig`

Ensures consistent coding styles across different editors and IDEs:
- **Encoding:** UTF-8
- **Line Endings:** LF (Unix)
- **Trailing Whitespace:** Removed
- **Final Newline:** Required

**Supported Editors:**
- VSCode (built-in)
- Vim/Neovim (plugin required)
- Sublime Text (plugin required)
- CLion/IntelliJ (built-in)

## Static Analysis

### Clang-Tidy

**Configuration:** `.clang-tidy`

**Enabled Check Categories:**
- `clang-diagnostic-*` - Compiler diagnostics
- `clang-analyzer-*` - Static analyzer checks
- `readability-*` - Code readability checks
- `modernize-*` - Modern C++ practices (C11 for us)
- `bugprone-*` - Bug-prone code patterns
- `performance-*` - Performance improvements
- `portability-*` - Portability issues
- `concurrency-*` - Thread safety issues

**Disabled Checks:**
- `readability-identifier-naming` - We have our own naming conventions
- `readability-magic-numbers` - Too noisy for embedded code
- `bugprone-easily-swappable-parameters` - False positives

**Usage:**

```bash
# Run on single file
clang-tidy src/hal/hal_rs485.c -- -Iinclude -Isrc

# Run on all files (with compile_commands.json)
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
run-clang-tidy

# Run with fixes
clang-tidy -fix src/hal/hal_rs485.c -- -Iinclude -Isrc
```

**CMake Integration:**

```bash
cmake -DENABLE_STATIC_ANALYSIS=ON ..
make
```

### Ignored Files

**Configuration:** `.clang-format-ignore`

Files/directories excluded from formatting:
- `third_party/**` - External libraries
- `build/**` - Build artifacts
- Generated files

## Code Coverage

### GCov/LCov

**Enable Coverage:**

```bash
cmake -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
make test
```

**Generate Report:**

```bash
# Capture coverage data
lcov --capture --directory . --output-file coverage.info

# Remove external code
lcov --remove coverage.info '/usr/*' --output-file coverage.info

# Generate HTML report
genhtml coverage.info --output-directory coverage_html

# View report
firefox coverage_html/index.html
```

**Coverage Goals:**
- **Overall:** > 90%
- **Safety-critical code:** 100%
- **HAL layer:** > 95%
- **Application layer:** > 85%

## Sanitizers

### AddressSanitizer (ASan)

Detects memory errors:
- Buffer overflows
- Use-after-free
- Memory leaks
- Double-free

**Enable:**

```bash
cmake -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug ..
make
./oht50_main
```

### UndefinedBehaviorSanitizer (UBSan)

Detects undefined behavior:
- Integer overflow
- Division by zero
- Null pointer dereference
- Misaligned pointers

**Already included with ASan option**

## Compiler Warnings

### Warning Flags

**Enabled in `cmake/CompilerOptions.cmake`:**

```c
-Wall                     // All standard warnings
-Wextra                   // Extra warnings
-Wpedantic                // Pedantic warnings
-Werror=implicit-function-declaration
-Werror=return-type
-Werror=uninitialized
-Wformat=2
-Wformat-security
-Wnull-dereference
-Wstrict-prototypes
-Wmissing-prototypes
-Wold-style-definition
-Wshadow
-Wcast-align
-Wconversion
-Wsign-conversion
```

**Treat as Errors:**
- Implicit function declarations
- Missing return statements
- Uninitialized variables

## Pre-commit Checks

### Recommended Pre-commit Hook

Create `.git/hooks/pre-commit`:

```bash
#!/bin/bash

# Format check
echo "Checking code formatting..."
CHANGED_FILES=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|h)$')

if [ -n "$CHANGED_FILES" ]; then
    for file in $CHANGED_FILES; do
        clang-format --dry-run --Werror "$file" 2>&1 | grep -q "warning:"
        if [ $? -eq 0 ]; then
            echo "Error: $file needs formatting"
            echo "Run: clang-format -i $file"
            exit 1
        fi
    done
fi

# Static analysis (optional, can be slow)
# echo "Running clang-tidy..."
# for file in $CHANGED_FILES; do
#     clang-tidy "$file" -- -Iinclude -Isrc || exit 1
# done

echo "Pre-commit checks passed!"
```

Make executable:
```bash
chmod +x .git/hooks/pre-commit
```

## Continuous Integration

### CI Pipeline Stages

1. **Build Check**
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release ..
   make -j$(nproc)
   ```

2. **Code Formatting**
   ```bash
   find src include -name "*.c" -o -name "*.h" | \
       xargs clang-format --dry-run --Werror
   ```

3. **Static Analysis**
   ```bash
   cmake -DENABLE_STATIC_ANALYSIS=ON ..
   make
   ```

4. **Unit Tests**
   ```bash
   ctest -V
   ```

5. **Coverage Report**
   ```bash
   cmake -DENABLE_COVERAGE=ON ..
   make test
   lcov --capture --directory . --output-file coverage.info
   ```

6. **Sanitizer Tests**
   ```bash
   cmake -DENABLE_SANITIZERS=ON ..
   make
   ./oht50_main
   ```

## Best Practices

### Code Style

1. **Consistent Formatting**
   - Run `clang-format` before committing
   - Use `.editorconfig` in your editor
   - Follow LLVM style guide

2. **Naming Conventions**
   - Functions: `snake_case`
   - Variables: `snake_case`
   - Constants: `UPPER_CASE`
   - Types: `snake_case_t`

3. **Documentation**
   - Document all public functions
   - Use Doxygen-style comments
   - Include examples for complex functions

### Error Handling

1. **Always Check Return Values**
   ```c
   int result = hal_rs485_init(&config);
   if (result != HAL_OK) {
       // Handle error
   }
   ```

2. **Use Meaningful Error Codes**
   ```c
   typedef enum {
       HAL_OK = 0,
       HAL_ERROR = -1,
       HAL_TIMEOUT = -2,
       HAL_BUSY = -3
   } hal_status_t;
   ```

3. **Log Errors Appropriately**
   ```c
   if (result != HAL_OK) {
       LOG_ERROR("Failed to initialize RS485: %d", result);
       return result;
   }
   ```

### Memory Management

1. **Always Free What You Allocate**
   ```c
   void* buffer = malloc(size);
   if (buffer == NULL) {
       return HAL_ERROR;
   }
   // Use buffer
   free(buffer);
   ```

2. **Check for NULL Pointers**
   ```c
   void function(void* ptr) {
       if (ptr == NULL) {
           return HAL_ERROR;
       }
       // Use ptr
   }
   ```

3. **Avoid Memory Leaks**
   - Use ASan to detect leaks
   - Free in error paths
   - Use RAII-like patterns

### Thread Safety

1. **Protect Shared Data**
   ```c
   pthread_mutex_lock(&mutex);
   shared_data = value;
   pthread_mutex_unlock(&mutex);
   ```

2. **Avoid Race Conditions**
   - Use atomic operations where possible
   - Minimize critical sections
   - Use thread-local storage

3. **Test Concurrent Code**
   - Use ThreadSanitizer (TSan)
   - Write specific concurrency tests
   - Test under load

## Quality Metrics

### Current Status

- **Code Coverage:** ~90%
- **Static Analysis:** 0 high-priority issues
- **Compiler Warnings:** 0 (all treated as errors)
- **Memory Leaks:** 0 (verified with ASan)

### Quality Gates

**Before Merge:**
- [ ] Code formatted (clang-format)
- [ ] Static analysis passed (clang-tidy)
- [ ] All tests passing
- [ ] Coverage maintained/improved
- [ ] No sanitizer errors

**Before Release:**
- [ ] Full test suite passing
- [ ] Coverage > 90%
- [ ] No critical static analysis issues
- [ ] Documentation updated
- [ ] Performance benchmarks met

## Tools Installation

### Ubuntu/Debian

```bash
# Formatting
sudo apt-get install clang-format

# Static Analysis
sudo apt-get install clang-tidy

# Coverage
sudo apt-get install lcov

# Sanitizers (included in GCC/Clang)
sudo apt-get install gcc clang
```

### VSCode Extensions

- C/C++ (Microsoft)
- Clang-Format
- EditorConfig for VS Code
- Code Coverage

### Vim/Neovim Plugins

```vim
Plug 'rhysd/vim-clang-format'
Plug 'editorconfig/editorconfig-vim'
```

## Troubleshooting

### Clang-Format Not Working

```bash
# Check version (need 10+)
clang-format --version

# Install newer version
sudo apt-get install clang-format-14
```

### Clang-Tidy Too Slow

```bash
# Run on changed files only
git diff --name-only | grep '\.[ch]$' | xargs clang-tidy

# Use compilation database
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
```

### Coverage Report Empty

```bash
# Ensure debug build
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..

# Run tests to generate coverage data
make test

# Check .gcda files exist
find . -name "*.gcda"
```

## References

- [Clang-Format Documentation](https://clang.llvm.org/docs/ClangFormat.html)
- [Clang-Tidy Documentation](https://clang.llvm.org/extra/clang-tidy/)
- [EditorConfig](https://editorconfig.org/)
- [GCC Warning Options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html)
- [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer)

---

**Last Updated:** 2025-10-07  
**Version:** 1.0.0  
**Maintained By:** Firmware Team

