#ifdef ESP32
#  include "NVSPreferencesStorage.h"

NVSPreferencesStorage::NVSPreferencesStorage() {}
NVSPreferencesStorage::~NVSPreferencesStorage() { preferences.end(); }

bool NVSPreferencesStorage::begin(const char* name, bool readOnly) {
  return preferences.begin(name, readOnly);
}

void NVSPreferencesStorage::end() {
  preferences.end();
}

bool NVSPreferencesStorage::isKey(const char* key) {
  return preferences.isKey(key);
}

std::string NVSPreferencesStorage::getString(const char* key, const char* defaultValue) {
  String arduinoString = preferences.getString(key, defaultValue);
  return std::string(arduinoString.c_str());
}

size_t NVSPreferencesStorage::putString(const char* key, const std::string& value) {
  String arduinoString(value.c_str());
  return preferences.putString(key, arduinoString);
}

int NVSPreferencesStorage::remove(const char* key) {
  return preferences.remove(key);
}
#endif