# 🛠️ Hướng Dẫn Phát Triển - OHT-50 Firmware

**Version:** 1.0.1  
**Last Updated:** 2025-10-07

---

## 📋 **MỤC LỤC**

1. [Development Environment](#development-environment)
2. [Architecture Overview](#architecture-overview)
3. [Development Workflow](#development-workflow)
4. [Debugging](#debugging)
5. [Testing Strategy](#testing-strategy)
6. [Performance Profiling](#performance-profiling)
7. [Best Practices](#best-practices)

---

## 💻 **DEVELOPMENT ENVIRONMENT**

### **1. IDE Setup**

#### **VS Code (Khuyến nghị)**

```bash
# Install VS Code
# Extensions cần thiết:
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cmake-tools
code --install-extension llvm-vs-code-extensions.vscode-clangd
code --install-extension twxs.cmake
code --install-extension ms-vscode.makefile-tools
```

**.vscode/settings.json:**

```json
{
  "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
  "cmake.configureOnOpen": true,
  "clang-format.executable": "/usr/bin/clang-format",
  "clang-format.style": "file",
  "editor.formatOnSave": true,
  "files.associations": {
    "*.h": "c",
    "*.c": "c"
  }
}
```

---

## 🏗️ **ARCHITECTURE OVERVIEW**

### **Layer Architecture**

```
Application (Orchestration)
     ↓
Domain (Business Logic)
     ↓
Infrastructure (Technical Services)
     ↓
Core (Business Core - Independent)
     ↓
HAL (Hardware Abstraction)
```

**Dependency Rules:**
- ✅ Lower layers CAN'T depend on upper layers
- ✅ Upper layers CAN depend on lower layers
- ✅ Same layer: minimize coupling

---

## 🔄 **DEVELOPMENT WORKFLOW**

### **1. Feature Development**

```bash
# 1. Create feature branch
git checkout -b feature/my-feature

# 2. Implement feature
# Edit files...

# 3. Build và test
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
make test

# 4. Commit và push
git add .
git commit -m "feat: add my feature"
git push origin feature/my-feature

# 5. Create Pull Request
```

---

## 🐛 **DEBUGGING**

### **GDB Debugging**

```bash
# Build với debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run với GDB
gdb ./oht50_main

# GDB commands:
(gdb) break main
(gdb) run --debug
(gdb) next
(gdb) print variable
(gdb) backtrace
```

### **Valgrind (Memory Leaks)**

```bash
# Check memory leaks
valgrind --leak-check=full \
         --show-leak-kinds=all \
         ./oht50_main --dry-run
```

---

## 🧪 **TESTING STRATEGY**

### **Unit Tests**

```bash
# Run all tests
make test

# Run specific test
./tests/unit/test_hal_rs485

# With verbose
ctest -V
```

### **Integration Tests**

```bash
cd tests/integration
./test_module_discovery.sh
```

---

## ⚡ **PERFORMANCE PROFILING**

### **Using perf**

```bash
# Profile CPU
perf record -g ./oht50_main
perf report

# Analyze hotspots
perf top
```

---

## ✅ **BEST PRACTICES**

### **Code Quality**

```bash
# Format code
clang-format -i src/**/*.c

# Static analysis
clang-tidy src/**/*.c -- -I include/

# Build warnings
cmake -DCMAKE_C_FLAGS="-Wall -Wextra -Werror" ..
```

---

**Version:** 1.0.1  
**Maintained By:** OHT-50 Dev Team

