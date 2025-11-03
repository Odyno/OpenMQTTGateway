#pragma once

#include <gmock/gmock.h>
#include <storage/IStorage.h>

/**
 * @brief Mock implementation of IStorage for unit testing
 */
class MockStorage : public IStorage {
public:
  MOCK_METHOD(bool, begin, (const char* name, bool readOnly), (override));
  MOCK_METHOD(void, end, (), (override));
  MOCK_METHOD(bool, isKey, (const char* key), (override));
  MOCK_METHOD(std::string, getString, (const char* key, const char* defaultValue), (override));
  MOCK_METHOD(size_t, putString, (const char* key, const std::string& value), (override));
  MOCK_METHOD(int, remove, (const char* key), (override));
};
