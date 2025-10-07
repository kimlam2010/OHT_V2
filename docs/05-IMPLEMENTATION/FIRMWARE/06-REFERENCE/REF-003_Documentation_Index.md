# OHT-50 Firmware - Quick Navigation Index

**Version:** 1.0.0 | **Last Updated:** 2025-10-07 | **Status:** ✅ Production Ready

## 🚀 Quick Start

1. **New to the project?** → Start with [README.md](README.md)
2. **Want to build?** → See [BUILD_GUIDE.md](BUILD_GUIDE.md)
3. **Understanding code?** → Read [CODE_STRUCTURE.md](CODE_STRUCTURE.md)
4. **Finding documentation?** → Check [DOCUMENTATION.md](DOCUMENTATION.md)
5. **What changed?** → Review [CLEANUP_SUMMARY.md](CLEANUP_SUMMARY.md)

## 📚 Main Documentation

| Document | Purpose | When to Read |
|----------|---------|--------------|
| **[README.md](README.md)** | Project overview, features, quick start | First time, project introduction |
| **[BUILD_GUIDE.md](BUILD_GUIDE.md)** | Complete build instructions | Building the firmware |
| **[CODE_STRUCTURE.md](CODE_STRUCTURE.md)** | Code organization and architecture | Understanding codebase |
| **[CODE_QUALITY.md](CODE_QUALITY.md)** | Code quality tools and practices | Setting up development |
| **[DOCUMENTATION.md](DOCUMENTATION.md)** | Documentation index and navigation | Finding specific docs |
| **[CLEANUP_SUMMARY.md](CLEANUP_SUMMARY.md)** | Cleanup actions and improvements | Understanding recent changes |

## 📁 Directory-Specific Documentation

### Source Code
- **[src/README.md](src/README.md)** - Source code structure (TBD)
- **[include/README.md](include/README.md)** - Header files organization
- **[tests/README.md](tests/README.md)** - Test suite guide
- **[cmake/README.md](cmake/README.md)** - CMake configuration

### Scripts
- **[scripts/build/README.md](scripts/build/README.md)** - Build scripts
- **[scripts/deploy/README.md](scripts/deploy/README.md)** - Deploy scripts
- **[scripts/test/README.md](scripts/test/README.md)** - Test scripts
- **[scripts/rs485/README.md](scripts/rs485/README.md)** - RS485 utilities
- **[scripts/lidar/README.md](scripts/lidar/README.md)** - LiDAR utilities
- **[scripts/safety/README.md](scripts/safety/README.md)** - Safety tests

## 🎯 Common Tasks

### Building
```bash
# Quick build
mkdir build && cd build
cmake ..
make -j$(nproc)

# More info: BUILD_GUIDE.md
```

### Testing
```bash
# Run all tests
cd build
ctest -V

# More info: tests/README.md
```

### Running
```bash
# Start firmware
./build/oht50_main

# More info: README.md
```

## 🔍 Find Information By Topic

### Architecture & Design
- Architecture overview → [CODE_STRUCTURE.md](CODE_STRUCTURE.md)
- System layers → [CODE_STRUCTURE.md](CODE_STRUCTURE.md)
- Component design → [docs/01-ARCHITECTURE/](docs/01-ARCHITECTURE/)

### Implementation
- Source structure → [CODE_STRUCTURE.md](CODE_STRUCTURE.md)
- Header files → [include/README.md](include/README.md)
- Implementation guides → [docs/02-IMPLEMENTATION/](docs/02-IMPLEMENTATION/)

### Building & Configuration
- Build instructions → [BUILD_GUIDE.md](BUILD_GUIDE.md)
- CMake options → [cmake/README.md](cmake/README.md)
- Build scripts → [scripts/build/README.md](scripts/build/README.md)

### Testing
- Test suite → [tests/README.md](tests/README.md)
- Test scripts → [scripts/test/README.md](scripts/test/README.md)
- API tests → [tests/api/](tests/api/)

### Deployment
- Deploy guide → [docs/03-DEPLOYMENT/](docs/03-DEPLOYMENT/)
- Deploy scripts → [scripts/deploy/README.md](scripts/deploy/README.md)

### Operations
- Operations guide → [docs/04-OPERATIONS/](docs/04-OPERATIONS/)
- User guide → [docs/user_guide/](docs/user_guide/)

### Hardware & Interfaces
- RS485 → [scripts/rs485/README.md](scripts/rs485/README.md)
- LiDAR → [scripts/lidar/README.md](scripts/lidar/README.md)
- Safety → [scripts/safety/README.md](scripts/safety/README.md)

## 📊 Project Statistics

- **Documentation Files:** 5 (root level)
- **README Files:** 12 (throughout project)
- **Source Files:** 70 (C/H files)
- **Test Files:** 47 (C/Python files)
- **Total Code:** 58,265 lines

## 🏗️ Project Structure

```
firmware_new/
├── 📄 Documentation (5 main docs)
│   ├── README.md
│   ├── BUILD_GUIDE.md
│   ├── CODE_STRUCTURE.md
│   ├── DOCUMENTATION.md
│   └── CLEANUP_SUMMARY.md
│
├── 📂 Source Code
│   ├── src/              → Source files
│   ├── include/          → Header files
│   └── cmake/            → CMake modules
│
├── 🧪 Testing
│   └── tests/            → Test suite
│
├── 🔧 Utilities
│   └── scripts/          → Build/deploy/test scripts
│
├── ⚙️ Configuration
│   ├── config/           → Build configuration
│   └── CMakeLists.txt    → CMake config
│
└── 📚 Documentation
    └── docs/             → Full docs (symlink)
```

## 🎓 Learning Path

### For New Developers
1. Read [README.md](README.md) - Project overview
2. Review [CODE_STRUCTURE.md](CODE_STRUCTURE.md) - Understanding architecture
3. Read [BUILD_GUIDE.md](BUILD_GUIDE.md) - Building the project
4. Explore [tests/README.md](tests/README.md) - Testing approach
5. Check [docs/](docs/) - Detailed documentation

### For Contributors
1. Read [README.md](README.md) - Contributing guidelines
2. Review [CODE_STRUCTURE.md](CODE_STRUCTURE.md) - Code organization
3. Read [BUILD_GUIDE.md](BUILD_GUIDE.md) - Build system
4. Follow [tests/README.md](tests/README.md) - Testing standards
5. Update documentation as needed

### For Operators
1. Read [README.md](README.md) - System overview
2. Check [BUILD_GUIDE.md](BUILD_GUIDE.md) - Installation
3. Review [scripts/deploy/README.md](scripts/deploy/README.md) - Deployment
4. Read [docs/user_guide/](docs/user_guide/) - User guides
5. Check [docs/04-OPERATIONS/](docs/04-OPERATIONS/) - Operations

## 🔗 External Resources

### Main Project Docs
- **Location:** `/home/orangepi/Desktop/OHT_V2/docs/05-IMPLEMENTATION/FIRMWARE/`
- **Symlink:** `docs/` → Main documentation
- **Access:** [DOCUMENTATION.md](DOCUMENTATION.md)

### Online Resources
- Project repository
- Issue tracker
- Wiki/knowledge base
- Team communication channels

## 📝 Quick Reference Cards

### Build Commands
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build
cmake -DCMAKE_BUILD_TYPE=Release ..

# With tests
cmake -DBUILD_TESTING=ON ..

# With coverage
cmake -DENABLE_COVERAGE=ON ..
```

### Test Commands
```bash
# All tests
ctest -V

# Unit tests
cd tests/unit && ctest -V

# Integration tests
cd tests/integration && ctest -V

# API tests
python3 tests/api/test_http_api.py
```

### Common Scripts
```bash
# Build
./scripts/build/build.sh

# Deploy
./scripts/deploy/deploy.sh

# Start
./scripts/deploy/start_firmware.sh

# Test all
./scripts/test/run_all_tests.sh
```

## 🆘 Need Help?

### Common Issues
- **Build fails?** → [BUILD_GUIDE.md](BUILD_GUIDE.md) → Troubleshooting
- **Test fails?** → [tests/README.md](tests/README.md) → Troubleshooting
- **Can't find doc?** → [DOCUMENTATION.md](DOCUMENTATION.md)
- **Understand code?** → [CODE_STRUCTURE.md](CODE_STRUCTURE.md)

### Getting Support
1. Check documentation first
2. Review troubleshooting guides
3. Search issue tracker
4. Ask team for help

## ✅ Document Checklist

Before starting work:
- [ ] Read README.md
- [ ] Understand CODE_STRUCTURE.md
- [ ] Review BUILD_GUIDE.md
- [ ] Check relevant component docs

Before committing:
- [ ] Update relevant documentation
- [ ] Test changes
- [ ] Update changelog if needed
- [ ] Review code quality

## 🔄 Keeping Updated

### Documentation Maintenance
- Update docs when code changes
- Review quarterly for accuracy
- Mark deprecated content
- Add examples for new features

### Finding Updates
- Check CLEANUP_SUMMARY.md for recent changes
- Review git commit messages
- Read changelog in README.md
- Check team communications

---

**Navigation Help:**
- 📄 Main docs are in root directory
- 📂 Component docs are in subdirectories
- 🔗 External docs via `docs/` symlink
- 📚 Full docs index in DOCUMENTATION.md

**Quick Links:**
- [Project Home](README.md)
- [Build Guide](BUILD_GUIDE.md)
- [Code Structure](CODE_STRUCTURE.md)
- [Documentation Index](DOCUMENTATION.md)
- [Recent Changes](CLEANUP_SUMMARY.md)

---

**Last Updated:** 2025-10-07  
**Maintained By:** Firmware Team  
**Status:** ✅ Current


