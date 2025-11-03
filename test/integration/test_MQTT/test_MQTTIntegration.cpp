#include <gtest/gtest.h>

#include "../helpers/test_helpers.cpp"
#include "../helpers/test_helpers.h"

TEST(MQTTIntegration, Demo_ShouldPublishSensorDataCorrectly) {
  // Arrange
  std::string expected_topic = TestHelpers::createMQTTTopic(
      "OMG_Test", "BT", "toMQTT", "A4C138F2D736");

  // Act & Assert
  ASSERT_EQ(expected_topic, "home/OMG_Test/BTtoMQTT/A4C138F2D736");
}