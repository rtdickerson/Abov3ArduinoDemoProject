# Testing Guide

## Overview
This directory contains unit and integration tests for the ESP32 Mesh LED Controller project.

## Test Framework
- **Framework**: Unity (embedded-friendly C testing)
- **Execution**: PlatformIO Test Runner
- **Target**: ESP32 hardware (native tests)

## Running Tests

### All Tests
```bash
pio test
```

### Specific Test File
```bash
pio test -f test_main
```

### Verbose Output
```bash
pio test -v
```

## Test Structure

### Unit Tests (`test_main.cpp`)
- Configuration validation
- Timing parameter sanity checks
- Message type definitions
- Payload size constraints
- Node type verification

### Future Test Files
- `test_mesh.cpp`: Mesh network functionality
- `test_led.cpp`: LED control and color rendering
- `test_protocol.cpp`: Message encoding/decoding

## Writing New Tests

### Test Function Template
```cpp
void test_feature_name() {
    // Arrange: Set up test conditions
    int expected_value = 42;
    
    // Act: Execute function under test
    int actual_value = my_function();
    
    // Assert: Verify results
    TEST_ASSERT_EQUAL(expected_value, actual_value);
}
```

### Common Assertions
- `TEST_ASSERT_TRUE(condition)`
- `TEST_ASSERT_FALSE(condition)`
- `TEST_ASSERT_EQUAL(expected, actual)`
- `TEST_ASSERT_NOT_EQUAL(value1, value2)`
- `TEST_ASSERT_NULL(pointer)`
- `TEST_ASSERT_NOT_NULL(pointer)`
- `TEST_ASSERT_EQUAL_STRING(expected, actual)`

### Register Test in setup()
```cpp
void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_feature_name);
    UNITY_END();
}
```

## Hardware-in-Loop Testing

Some tests require actual ESP32 hardware:
1. Connect ESP32 via USB
2. Run `pio test`
3. Tests compile, upload, execute on hardware
4. Results stream back via serial

## Test Coverage Goals

- [ ] Configuration validation (✓ implemented)
- [ ] Mesh network connection/disconnection
- [ ] Color message encoding/decoding
- [ ] LED update functionality
- [ ] Timeout handling
- [ ] Error recovery
- [ ] Memory leak detection
- [ ] Performance benchmarks

## Continuous Integration

For CI/CD pipelines:
```bash
# Install PlatformIO
pip install platformio

# Run tests
pio test --environment esp32

# Generate JUnit XML report
pio test --junit-xml=test-results.xml
```

## Debugging Failed Tests

1. **Enable verbose output**: `pio test -v`
2. **Check serial monitor**: Tests output to Serial @ 115200 baud
3. **Isolate test**: Comment out other tests in `setup()`
4. **Add debug prints**: Use `DEBUG_PRINTLN()` macros

## Simulation Testing

For tests without hardware:
- Use Wokwi simulator (see `/simulation` directory)
- Online: https://wokwi.com/
- Tests can run in simulated ESP32 environment

## Performance Benchmarks

Example benchmark test:
```cpp
void test_led_update_speed() {
    unsigned long start = micros();
    FastLED.show();
    unsigned long duration = micros() - start;
    
    // Should complete in < 10ms for 16 LEDs
    TEST_ASSERT_TRUE(duration < 10000);
}
```

## Memory Testing

Check for memory leaks:
```cpp
void test_memory_stability() {
    uint32_t free_heap_before = ESP.getFreeHeap();
    
    // Perform operations
    for(int i = 0; i < 100; i++) {
        // Your code here
    }
    
    uint32_t free_heap_after = ESP.getFreeHeap();
    
    // Heap should not decrease significantly
    TEST_ASSERT_TRUE(free_heap_after >= free_heap_before - 100);
}
```

## Best Practices

1. **Keep tests independent**: Each test should run in isolation
2. **Use descriptive names**: `test_mesh_reconnects_after_disconnect()`
3. **Test edge cases**: Null pointers, buffer overflows, boundary values
4. **Mock hardware when possible**: Use dependency injection
5. **Document assumptions**: Comment why thresholds are set

## Resources

- [Unity Documentation](https://github.com/ThrowTheSwitch/Unity)
- [PlatformIO Testing Guide](https://docs.platformio.org/en/latest/plus/unit-testing.html)
- [ESP32 Unit Testing](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/unit-tests.html)
