# OpenMQTTGateway Test Infrastructure

This directory contains the comprehensive testing infrastructure for OpenMQTTGateway, following PlatformIO testing best practices and using GoogleTest framework.

## 📁 Directory Structure

```
test/
├── test_runner.cpp          # Main test runner with GoogleTest setup
├── README.md               # This file
├── helpers/                # Reusable test utilities and fixtures
│   ├── README.md          # Helper documentation
│   ├── test_helpers.h     # Common test utilities
│   └── test_helpers.cpp   # Helper implementations
├── mocks/                  # Mock implementations for dependencies
│   ├── README.md          # Mock documentation
│   └── mock_arduino.h     # Arduino framework mocks
├── unit/                   # Isolated unit tests
│   ├── README.md          # Unit test documentation
│   └── test_*/            # Individual component test directories
└── integration/            # Integration and end-to-end tests
    ├── README.md          # Integration test documentation
    └── test_*/            # Integration test directories
```

## 🚀 Running Tests

### All Tests
```bash
pio test -e test
```

### With Verbose Output
```bash
pio test -e test -v
```

### Specific Test Filters
```bash
# Run sensor tests only
pio test -e test --filter "*sensor*"

# Run RF configuration tests
pio test -e test --filter "*rf_configuration*"

# Run MQTT tests
pio test -e test --filter "*mqtt*"
```

## 🎯 Test Environments

### `[env:test]`
- **Platform**: Native (runs on development machine)
- **Framework**: GoogleTest
- **Purpose**: Fast unit testing with minimal dependencies

### `[env:test_embedded]` (Commented)
- **Platform**: ESP32 hardware
- **Framework**: GoogleTest  
- **Purpose**: Hardware-in-the-loop testing (future enhancement)

## 🏛️ Architecture Principles

For test infrastructure follows the SOLID principles and ESP32/Arduino best practices:

### **Single Responsibility**
- Each test file focuses on one component or functionality
- Helper functions have clear, single purposes
- Mock objects represent specific dependencies

### **Open/Closed Principle**
- Easy to extend with new test categories
- Test helpers are extensible without modification
- Mock interfaces allow for different implementations

### **Dependency Inversion**
- Tests depend on abstractions (mocks) not concrete implementations
- Hardware dependencies are abstracted away for unit testing
- Configuration dependencies are injectable

### **Performance Optimization**
- Native testing for fast feedback cycles
- Minimal memory usage in embedded test scenarios
- Parallel test execution support

## 📊 VS Code Integration

Pre-configured tasks are available in VS Code:

- **🧪 Run All Tests**: Execute complete test suite
- **🎯 Run Single Test**: Execute specific test with filter
- **📊 Test with Verbose Output**: Detailed test execution logging
- **🔍 Run Sensor Tests**: Sensor module specific tests
- **📡 Run MQTT Tests**: MQTT functionality tests
- **⚙️ Run Configuration Tests**: Configuration management tests

## 🔄 GitHub Actions

Automated testing runs on:
- **Push**: To `main`, `development`, and `feature/*` branches
- **Pull Requests**: To `main` and `development` branches

The workflow:
1. Sets up Ubuntu environment with Python 3.11
2. Installs PlatformIO
3. Runs native tests with `pio test -e test`
4. Reports test results

## 📝 Writing Tests

### Naming Conventions

- Test files: `test_[ComponentName].cpp`
- Test suites: `[ComponentName]Test`
- Test cases: descriptive names using GoogleTest conventions

### Unit Test Example

```cpp
#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"
#include "../mocks/mock_arduino.h"

class RFConfigurationTest : public ::testing::Test {
protected:
    void SetUp() override {
        ArduinoMock::reset();
    }
};

TEST_F(RFConfigurationTest, ShouldInitializeWithDefaults) {
    // Arrange
    RFConfiguration config;
    
    // Act
    config.init();
    
    // Assert  
    ASSERT_EQ(config.getFrequency(), 433.92);
    ASSERT_TRUE(config.isEnabled());
}
```

### Integration Test Example

```cpp
#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"

TEST(MQTTIntegration, ShouldPublishSensorDataCorrectly) {
    // Arrange
    std::string expected_topic = TestHelpers::createMQTTTopic(
        "OMG_Test", "BT", "toMQTT", "A4C138F2D736");
    
    // Act & Assert
    ASSERT_EQ(expected_topic, "home/OMG_Test/BTtoMQTT/A4C138F2D736");
}
```

## 🔧 Best Practices

1. **Test Naming**: Use descriptive names that explain behavior
2. **Arrange-Act-Assert**: Structure tests clearly 
3. **Mock External Dependencies**: Isolate units under test
4. **Test Both Success and Failure**: Include edge cases
5. **Use Test Helpers**: Reduce duplication with common utilities
6. **Follow PlatformIO Guidelines**: Align with framework conventions

## 📚 References

- [PlatformIO Unit Testing](https://docs.platformio.org/en/latest/advanced/unit-testing/)
- [GoogleTest Documentation](https://google.github.io/googletest/)
- [OpenMQTTGateway Architecture](../docs/)
- [Test Best Practices](https://docs.platformio.org/en/stable/advanced/unit-testing/structure/best-practices.html)