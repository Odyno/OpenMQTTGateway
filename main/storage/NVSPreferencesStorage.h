#pragma once

#ifdef ESP32
#  include <Preferences.h>
#  include <storage/IStorage.h>

/**
 * @brief Concrete implementation of IStorage using ESP32 Preferences (NVS)
 */
class NVSPreferencesStorage : public IStorage {
public:
  NVSPreferencesStorage();
  ~NVSPreferencesStorage() override;

  bool begin(const char* name, bool readOnly) override;
  void end() override;
  bool isKey(const char* key) override;
  std::string getString(const char* key, const char* defaultValue = "") override;
  size_t putString(const char* key, const std::string& value) override;
  int remove(const char* key) override;

private:
  Preferences preferences;
};
#endif