# OHT-50 Firmware Release Notes

**Version:** f53505c  
**Date:** 2025-08-24 05:56:28  
**Build:** f53505c9012da1d4217a86d7b134d55c95ebeb22

## Changes in this Release

### New Features
- ISO/IEC 12207 compliant firmware structure
- CMake build system
- MISRA C:2012 compliance
- Unity test framework
- Automated deployment scripts

### Improvements
- Modular HAL layer
- Improved error handling
- Better logging system
- Enhanced safety mechanisms

### Bug Fixes
- Fixed include path issues
- Resolved compilation warnings
- Improved build reliability

## Installation

1. Extract the release package
2. Run the deployment script as root:
   ```bash
   sudo ./scripts/deploy.sh
   ```

## Rollback

If issues occur, use the rollback script:
```bash
sudo ./scripts/rollback.sh
```

## Testing

Run unit tests:
```bash
./tests/test_hal_common
```

## Support

For issues or questions, contact the FW Team.
