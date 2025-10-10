# Unit Tests - Register Metadata API

## Compilation

```bash
cd /home/orangepi/Desktop/OHT_V2/firmware_new
gcc -o test_register_metadata tests/test_register_metadata_api.c \
    src/hal/register/register_info.c \
    src/app/utils/register_json_serializer.c \
    -I./include -I./src/hal/common \
    -lm
```

## Run Tests

```bash
./test_register_metadata
```

## Expected Output

```
========================================
🧪 REGISTER METADATA API - UNIT TESTS
========================================

🧪 TEST SECTION: get_module_registers_array()
  [✓] PASS: Power module registers should not be NULL
  [✓] PASS: Power module should have > 0 registers
  [✓] PASS: Power module should have >= 51 registers
  [✓] PASS: Safety module registers should not be NULL
  ...

========================================
📊 TEST RESULTS
========================================
Total tests:  40
✓ Passed:     40
✗ Failed:     0
Success rate: 100.0%
========================================

✅ ALL TESTS PASSED!
```

## Test Coverage

- ✅ Register Info API functions
- ✅ JSON Serializer functions
- ✅ Enum to String converters
- ✅ Error handling
- ✅ All 4 modules (Power, Safety, Motor, Dock)

