#include <config_RF.h>
#include <gtest/gtest.h>
#include <rf/RFConfiguration.h>

#include "../mocks/mock_IStorage.h"
#include "../mocks/mock_RFReceiver.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgReferee;

class RFConfigurationTest : public ::testing::Test {
protected:
  void SetUp() override {
  }
};

TEST_F(RFConfigurationTest, ShouldInitializeWithDefaults) {
  // Arrange
  MockRFReceiver mockReceiver;
  RFConfiguration config(mockReceiver);

  // Assert
  ASSERT_NEAR(config.getFrequency(), RF_FREQUENCY, 0.01);
  ASSERT_EQ(config.getActiveReceiver(), ACTIVE_RECEIVER);
  ASSERT_EQ(config.getRssiThreshold(), 0);
  ASSERT_EQ(config.getNewOokThreshold(), 0);
}

TEST_F(RFConfigurationTest, ShouldSetAndGetFrequency) {
  MockRFReceiver mockReceiver;
  RFConfiguration config(mockReceiver);

  config.setFrequency(433.92f);
  ASSERT_EQ(config.getFrequency(), 433.92f);
}

TEST_F(RFConfigurationTest, ShouldSetAndGetRssiThreshold) {
  MockRFReceiver mockReceiver;
  RFConfiguration config(mockReceiver);

  config.setRssiThreshold(42);
  ASSERT_EQ(config.getRssiThreshold(), 42);
}

TEST_F(RFConfigurationTest, ShouldSetAndGetNewOokThreshold) {
  MockRFReceiver mockReceiver;
  RFConfiguration config(mockReceiver);

  config.setNewOokThreshold(77);
  ASSERT_EQ(config.getNewOokThreshold(), 77);
}

TEST_F(RFConfigurationTest, ShouldSetAndGetActiveReceiver) {
  MockRFReceiver mockReceiver;
  RFConfiguration config(mockReceiver);

  config.setActiveReceiver(2);
  ASSERT_EQ(config.getActiveReceiver(), 2);
}

TEST_F(RFConfigurationTest, ShouldReInitRestoreDefaults) {
  MockRFReceiver mockReceiver;
  RFConfiguration config(mockReceiver);

  config.setFrequency(400.0f);
  config.setActiveReceiver(3);
  config.setRssiThreshold(99);
  config.setNewOokThreshold(88);

  config.reInit();

  ASSERT_NEAR(config.getFrequency(), RF_FREQUENCY, 0.01);
  ASSERT_EQ(config.getActiveReceiver(), ACTIVE_RECEIVER);
  ASSERT_EQ(config.getRssiThreshold(), 0);
  ASSERT_EQ(config.getNewOokThreshold(), 0);
}

TEST_F(RFConfigurationTest, ShouldValidateFrequencyRanges) {
  MockRFReceiver mockReceiver;
  RFConfiguration config(mockReceiver);

  // Valid ranges
  ASSERT_TRUE(config.validFrequency(300.0f));
  ASSERT_TRUE(config.validFrequency(348.0f));
  ASSERT_TRUE(config.validFrequency(387.0f));
  ASSERT_TRUE(config.validFrequency(464.0f));
  ASSERT_TRUE(config.validFrequency(779.0f));
  ASSERT_TRUE(config.validFrequency(928.0f));

  // Invalid ranges
  ASSERT_FALSE(config.validFrequency(299.9f));
  ASSERT_FALSE(config.validFrequency(348.1f));
  ASSERT_FALSE(config.validFrequency(386.9f));
  ASSERT_FALSE(config.validFrequency(464.1f));
  ASSERT_FALSE(config.validFrequency(778.9f));
  ASSERT_FALSE(config.validFrequency(928.1f));
}

TEST_F(RFConfigurationTest, ShouldUpdateValueIfDifferent) {
  MockRFReceiver mockReceiver;
  RFConfiguration config(mockReceiver);

  StaticJsonDocument<128> doc;
  JsonObject obj = doc.to<JsonObject>();
  obj["frequency"] = 315.0f;

  float freq = 433.92f;
  RFConfiguration::update(obj, "frequency", freq);
  ASSERT_EQ(freq, 315.0f);

  // Should not update if same
  obj["frequency"] = 315.0f;
  RFConfiguration::update(obj, "frequency", freq);
  ASSERT_EQ(freq, 315.0f);
}

TEST_F(RFConfigurationTest, ShouldNotUpdateIfKeyMissing) {
  MockRFReceiver mockReceiver;
  RFConfiguration config(mockReceiver);

  StaticJsonDocument<128> doc;
  JsonObject obj = doc.to<JsonObject>();

  float freq = 433.92f;
  RFConfiguration::update(obj, "frequency", freq);
  ASSERT_EQ(freq, 433.92f);
}

TEST_F(RFConfigurationTest, ShouldSerializeToJson) {
  MockRFReceiver mockReceiver;
  RFConfiguration config(mockReceiver);

  config.setFrequency(433.92f);
  config.setRssiThreshold(12);
  config.setNewOokThreshold(34);
  config.setActiveReceiver(1);

  StaticJsonDocument<256> doc;
  JsonObject obj = doc.to<JsonObject>();
  config.toJson(obj);

  ASSERT_EQ(obj["frequency"].as<float>(), 433.92f);
  ASSERT_EQ(obj["rssithreshold"].as<int>(), 12);
  ASSERT_EQ(obj["ookthreshold"].as<int>(), 34);
  ASSERT_EQ(obj["active"].as<int>(), 1);
  ASSERT_TRUE(obj.containsKey("white-list"));
  ASSERT_TRUE(obj.containsKey("black-list"));
}

TEST_F(RFConfigurationTest, ShouldLoadFromJson) {
  MockRFReceiver mockReceiver;
  RFConfiguration config(mockReceiver);

  StaticJsonDocument<256> doc;
  JsonObject obj = doc.to<JsonObject>();
  obj["frequency"] = 315.0f;
  obj["active"] = 2;

  config.fromJson(obj);

  ASSERT_EQ(config.getFrequency(), 315.0f);
  ASSERT_EQ(config.getActiveReceiver(), 2);
}

TEST_F(RFConfigurationTest, ShouldNotUpdateFrequencyIfInvalid) {
  MockRFReceiver mockReceiver;
  RFConfiguration config(mockReceiver);

  StaticJsonDocument<256> doc;
  JsonObject obj = doc.to<JsonObject>();
  obj["frequency"] = 100.0f; // Invalid

  float oldFreq = config.getFrequency();
  config.fromJson(obj);

  ASSERT_EQ(config.getFrequency(), oldFreq);
}

// Storage tests are skipped because saveOnStorage/loadFromStorage
// are protected by #ifdef ESP32 in the implementation
#ifdef ESP32
TEST_F(RFConfigurationTest, ShouldSaveToStorageWithStdString) {
  MockRFReceiver mockReceiver;
  MockStorage mockStorage;
  RFConfiguration config(mockReceiver, &mockStorage);

  config.setFrequency(315.0f);
  config.setActiveReceiver(2);

  // Verify that putString is called with std::string (not Arduino String)
  EXPECT_CALL(mockStorage, begin(_, false))
      .WillOnce(Return(true));
  EXPECT_CALL(mockStorage, putString("RFConfig", ::testing::An<const std::string&>()))
      .WillOnce(Return(100));
  EXPECT_CALL(mockStorage, end())
      .Times(1);

  config.saveOnStorage();
}

TEST_F(RFConfigurationTest, ShouldLoadFromStorageWithStdString) {
  MockRFReceiver mockReceiver;
  MockStorage mockStorage;
  RFConfiguration config(mockReceiver, &mockStorage);

  std::string storedConfig = R"({"frequency":315.0,"active":2})";

  EXPECT_CALL(mockStorage, begin(_, true))
      .WillOnce(Return(true));
  EXPECT_CALL(mockStorage, isKey("RFConfig"))
      .WillOnce(Return(true));
  EXPECT_CALL(mockStorage, getString("RFConfig", "{}"))
      .WillOnce(Return(storedConfig));
  EXPECT_CALL(mockStorage, end())
      .Times(1);

  config.loadFromStorage();

  ASSERT_EQ(config.getFrequency(), 315.0f);
  ASSERT_EQ(config.getActiveReceiver(), 2);
}

TEST_F(RFConfigurationTest, ShouldHandleEmptyStorageGracefully) {
  MockRFReceiver mockReceiver;
  MockStorage mockStorage;
  RFConfiguration config(mockReceiver, &mockStorage);

  EXPECT_CALL(mockStorage, begin(_, true))
      .WillOnce(Return(true));
  EXPECT_CALL(mockStorage, isKey("RFConfig"))
      .WillOnce(Return(false));
  EXPECT_CALL(mockStorage, end())
      .Times(1);
  EXPECT_CALL(mockReceiver, enable())
      .Times(1);

  config.loadFromStorage();
}
#endif // ESP32
