# HoneywellSecurityMQTT Unit Tests

This directory contains unit tests for the HoneywellSecurityMQTT project that can be run without requiring actual hardware (RTL-SDR adapter or MQTT broker).

## Overview

The test suite includes comprehensive tests for:

- **AnalogDecoder**: Tests for signal processing, decimation, threshold adaptation, OOK decoding, and filtering
- **DigitalDecoder**: Tests for CRC validation, payload parsing, Manchester decoding, state bit extraction, and timing

## Test Coverage

### AnalogDecoder Tests (9 tests)
- Decimation ratio verification (1-of-17 sampling)
- High/low magnitude signal detection
- Threshold adaptation with varying signal levels
- Signal saturation handling
- Smoothing filter behavior
- OOK (On-Off Keying) decay
- Minimum threshold enforcement
- Callback handling

### DigitalDecoder Tests (15 tests)
- CRC computation for Honeywell and 2GIG payloads
- Payload field parsing (SOF, serial, type, CRC)
- State bit extraction (alarm, tamper, battery low)
- Sync pattern detection
- Bit timing and sampling
- Edge detection
- Data validation
- Alarm state tracking
- Timeout constants
- MQTT topic generation

## Building and Running Tests

### Prerequisites

The tests use a lightweight custom testing framework (no external dependencies required). You only need:
- g++ compiler with C++11 support
- Standard C++ library

### Build Commands

```bash
cd tests

# Build the tests
make

# Run the tests
make test

# Or run directly
./run_tests

# Clean build artifacts
make clean
```

### Expected Output

When all tests pass, you should see:

```
Running 24 tests...
========================================
[ RUN      ] AnalogDecoderTest.DecimationRatio
[       OK ] AnalogDecoderTest.DecimationRatio
...
========================================
Tests passed: 24/24
All tests PASSED
```

## Test Architecture

### Files

- `simple_test.h` - Lightweight testing framework (header-only)
- `mock_mqtt.h` - Mock MQTT class for testing without a broker
- `test_analog_decoder.cpp` - Tests for AnalogDecoder class
- `test_digital_decoder.cpp` - Tests for DigitalDecoder logic
- `test_main.cpp` - Test runner entry point
- `Makefile` - Build configuration

### Design Principles

1. **No Hardware Dependencies**: Tests run entirely in software without requiring RTL-SDR hardware or MQTT broker
2. **Fast Execution**: All tests complete in under a second
3. **Comprehensive Coverage**: Tests cover signal processing, decoding logic, state management, and error handling
4. **Isolated Testing**: Each test is independent and can run in any order

## Adding New Tests

To add new tests, follow this pattern:

```cpp
#include "simple_test.h"

TEST(TestSuiteName, TestName) {
    // Arrange
    // ... setup code ...
    
    // Act
    // ... execute code under test ...
    
    // Assert
    EXPECT_EQ(actual, expected);
    EXPECT_TRUE(condition);
    EXPECT_FALSE(condition);
    EXPECT_GT(a, b);
    EXPECT_GE(a, b);
    EXPECT_NE(a, b);
}
```

Available assertion macros:
- `EXPECT_EQ(a, b)` - Assert equality
- `EXPECT_NE(a, b)` - Assert inequality
- `EXPECT_TRUE(condition)` - Assert true
- `EXPECT_FALSE(condition)` - Assert false
- `EXPECT_GT(a, b)` - Assert greater than
- `EXPECT_GE(a, b)` - Assert greater than or equal
- `ASSERT_*` variants - Same as EXPECT but stop test on failure

## Continuous Integration

These tests are designed to be easily integrated into CI/CD pipelines. They:
- Return exit code 0 on success, non-zero on failure
- Provide clear output for debugging
- Run quickly (suitable for pre-commit hooks)
- Have no external dependencies

## Limitations

These tests focus on the core decoding and signal processing logic. They do not test:
- Actual RTL-SDR hardware interaction
- Real MQTT broker communication
- System-level integration
- Performance under load

For full system testing, you'll need the actual hardware setup as described in the main README.
