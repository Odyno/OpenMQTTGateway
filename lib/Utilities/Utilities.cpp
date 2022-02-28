#include "Utilities.hpp"

#if defined(ESP32)
#  include "esp_system.h"
#endif
#include <stdio.h>

#include <string>

namespace OMG {

std::string getMacAddress() {
  uint8_t baseMac[6];
  char baseMacChr[13] = {0};
#if defined(ESP8266)
  WiFi.macAddress(baseMac);
  sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
#elif defined(ESP32)
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
#else
  sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#endif
  return std::string(baseMacChr).c_str();
}

std::string getUniqueId(const char* id, const char* name, const char* sufix) {
  std::string out = std::string(id);
  if (id[0]) {
    out = std::string(id);
  } else {
    out = OMG::getMacAddress();
  }
  if (name != NULL) {
    out = out.append("-").append(name);
  }
  if (sufix != NULL) {
    out = out.append("-").append(sufix);
  }
  return out;
}

std::string getOMGUniqueId(const char* name) {
  return getUniqueId(getMacAddress().c_str(), name, NULL);
}

std::string getOMGUniqueId(const char* name, const char* sufix) {
  return getUniqueId(getMacAddress().c_str(), name, sufix);
}

} // namespace OMG
