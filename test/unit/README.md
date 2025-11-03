# Unit Tests

This directory contains isolated unit tests for individual OpenMQTTGateway modules and components.

## Organization

Tests are organized by module/component:

```
unit/
├── test_gateway_rf/          # RF configuration module tests
├── test_sensors/             # Sensor module tests
├── test_gateways/            # Gateway module tests
├── test_mqtt/                # MQTT functionality tests
├── test_configuration/       # Configuration management tests
└── test_web_ui/              # Web UI tests (ESP32 only)
```

## Naming Conventions

- Test files: `test_[ComponentName].cpp`
- Test suites: `[ComponentName]Test`
- Test cases: descriptive names using GoogleTest conventions

## Example Test Structure

```cpp
#include <gtest/gtest.h>
#include "../mocks/mock_arduino.h"
#include "ComponentUnderTest.h"

class ComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test fixtures
    }
    
    void TearDown() override {
        // Cleanup
    }
};

TEST_F(ComponentTest, ShouldDoExpectedBehavior) {
    // Test implementation
    ASSERT_TRUE(condition);
}
```

## Guidelines

- Test one component in isolation
- Use mocks for dependencies
- Follow AAA pattern (Arrange, Act, Assert)
- Test both success and failure cases
- Include edge cases and boundary conditions