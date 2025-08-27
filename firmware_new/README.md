# OHT-50 Firmware

ISO/IEC 12207 Compliant Firmware for OHT-50 Master Module

## Overview

This firmware implements the Master Module functionality for the OHT-50 system, following ISO/IEC 12207 software lifecycle processes and MISRA C:2012 coding standards.

## Features

- Hardware Abstraction Layer (HAL)
- Safety-critical systems
- Real-time communication
- Module management
- Configuration persistence
- API interface

## Building

```bash
# Build in Release mode
./scripts/build.sh Release

# Build in Debug mode
./scripts/build.sh Debug
```

## Testing

```bash
# Run all tests
./scripts/test.sh
```

## Documentation

- [API Documentation](docs/API_DOCUMENTATION.md)
- [Design Documentation](docs/design/)
- [User Guide](docs/user_guide/)
- [Developer Guide](docs/developer_guide/)
- [Migration Guide](docs/MIGRATION_GUIDE.md)
- [Reports](docs/reports/)
- [Archive](docs/archive/)

## License

[License information here]
