/*
  OpenMQTTGateway Addon  - ESP8266 or Arduino program for home automation

   Act as a gateway between your 433mhz, infrared IR, BLE, LoRa signal and one interface like an MQTT broker
   Send and receiving command by MQTT

   This is the Home Assistant MQTT Discovery addon.

    Copyright: (c) Rafal Herok / Florian Robert

    This file is part of OpenMQTTGateway.

    OpenMQTTGateway is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenMQTTGateway is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "User_config.h"

#ifdef ZmqttDiscovery2
#  include "TheengsCommon.h"
#  include "config_mqttDiscovery.h"

#  ifdef ESP8266
#    include <ESP8266WiFi.h>
#  elif defined(ESP32)
#    include <WiFi.h>

#    include "esp_mac.h"
#  endif
#  ifdef ESP32_ETHERNET
#    include <ETH.h>
#  endif

extern bool ethConnected;
extern JsonArray modules;
extern String getMacAddress();
char discovery_prefix[parameters_size + 1] = discovery_Prefix;

#  include <HassButton.h>
#  include <HassConstants.h>
#  include <HassDiscoveryManager.h>
#  include <HassSensor.h>
#  include <HassSwitch.h>
#  include <HassTemplates.h>

#  include <string>

//=============================================================================
// INTERFACE IMPLEMENTATIONS FOR LEGACY BRIDGE
// These implementations bridge modern C++ interfaces with legacy C system
// Only compiled when using HMD (ZmqttDiscovery2)
//=============================================================================
namespace omg {
namespace hass {

class LegacySettingsProvider : public ISettingsProvider {
public:
  std::string getDiscoveryPrefix() const override {
    return std::string(discovery_prefix);
  }

  std::string getMqttTopic() const override {
    return std::string(mqtt_topic);
  }

  std::string getGatewayName() const override {
    return std::string(gateway_name);
  }

  bool isEthConnected() const override {
    return ethConnected;
  }

  JsonArray getModules() const override {
    return modules;
  }
  std::string getNetworkMacAddress() const override {
    return getMacAddress().c_str();
  }

  std::string getNetworkIPAddress() const override {
    // Set configuration URL
    if (this->isEthConnected()) {
#  ifdef ESP32_ETHERNET
      return ETH.localIP().toString().c_str();
#  else
      return "0.0.0.0"; // Ethernet not supported
#  endif
    } else {
      return WiFi.localIP().toString().c_str();
    }
  }

  std::string getGatewayManufacturer() const override {
    return std::string(GATEWAY_MANUFACTURER);
  }
  std::string getGatewayVersion() const override {
    return std::string(OMG_VERSION);
  }
};

class LegacyMqttPublisher : public IMqttPublisher {
public:
  bool publishJson(JsonObject& json) override {
    try {
      enqueueJsonObject(json);
      return true;
    } catch (...) {
      return false;
    }
  }

  bool publishMessage(const std::string& topic, const std::string& payload, bool retain = false) override {
    try {
      return pubMQTT(topic.c_str(), payload.c_str(), retain);
    } catch (...) {
      return false;
    }
  }

  std::string getUId(const std::string& name, const std::string& suffix = "") override {
    // Use the legacy function defined later in this file
    String result = ::getUniqueId(String(name.c_str()), String(suffix.c_str()));
    return std::string(result.c_str());
  }
};

} // namespace hass
} // namespace omg

//=============================================================================
// HMD DISCOVERY FUNCTIONS (NEW ARCHITECTURE)
// Only compiled when ZmqttDiscovery2 is defined
//=============================================================================
static omg::hass::LegacySettingsProvider settingsProvider;
static omg::hass::LegacyMqttPublisher mqttPublisher;

std::unique_ptr<omg::hass::HassDiscoveryManager> g_discoveryManager;

/**
 * @brief Publish OpenMQTTGateway system entities
 * This function contains system-specific logic and belongs in main project
 */
void publishSystemEntities() {
  if (!g_discoveryManager) return;

  THEENGS_LOG_TRACE(F("Publishing OMG system entities" CR));

  // Access the static instances
  static omg::hass::LegacyMqttPublisher mqttPublisher;

  auto gatewayDevice = g_discoveryManager->getGatewayDevice();

  // System connectivity sensor
  auto connectivityConfig = omg::hass::HassEntity::EntityConfig::createSensor(
      "SYS: Connectivity",
      mqttPublisher.getUId("connectivity", "").c_str(),
      HASS_CLASS_CONNECTIVITY);
  connectivityConfig.componentType = "binary_sensor";
  connectivityConfig.stateTopic = will_Topic;
  connectivityConfig.availabilityTopic = will_Topic;
  auto connectivitySensor = std::make_unique<omg::hass::HassSensor>(connectivityConfig, gatewayDevice);
  g_discoveryManager->publishEntity(std::move(connectivitySensor));

  // System uptime sensor
  auto uptimeConfig = omg::hass::HassEntity::EntityConfig::createSensor(
      "SYS: Uptime",
      mqttPublisher.getUId("uptime", "").c_str(),
      HASS_CLASS_DURATION,
      HASS_UNIT_SECOND);
  uptimeConfig.valueTemplate = "{{ value_json.uptime }}";
  uptimeConfig.stateTopic = subjectSYStoMQTT;
  uptimeConfig.stateClass = stateClassMeasurement;
  auto uptimeSensor = std::make_unique<omg::hass::HassSensor>(uptimeConfig, gatewayDevice);
  g_discoveryManager->publishEntity(std::move(uptimeSensor));

  // System memory sensor
  auto memoryConfig = omg::hass::HassEntity::EntityConfig::createSensor(
      "SYS: Free memory",
      mqttPublisher.getUId("freemem", "").c_str(),
      HASS_CLASS_DATA_SIZE,
      HASS_UNIT_BYTE);
  memoryConfig.valueTemplate = "{{ value_json.freemem }}";
  memoryConfig.stateTopic = subjectSYStoMQTT;
  memoryConfig.stateClass = stateClassMeasurement;
  auto memorySensor = std::make_unique<omg::hass::HassSensor>(memoryConfig, gatewayDevice);
  g_discoveryManager->publishEntity(std::move(memorySensor));

  // System IP sensor
  auto ipConfig = omg::hass::HassEntity::EntityConfig::createSensor(
      "SYS: IP",
      mqttPublisher.getUId("ip", "").c_str());
  ipConfig.valueTemplate = "{{ value_json.ip }}";
  ipConfig.stateTopic = subjectSYStoMQTT;
  auto ipSensor = std::make_unique<omg::hass::HassSensor>(ipConfig, gatewayDevice);
  g_discoveryManager->publishEntity(std::move(ipSensor));

#  ifndef ESP32_ETHERNET
  // RSSI sensor (only for WiFi)
  auto rssiConfig = omg::hass::HassEntity::EntityConfig::createSensor(
      "SYS: RSSI",
      mqttPublisher.getUId("rssi", "").c_str(),
      HASS_CLASS_SIGNAL_STRENGTH,
      HASS_UNIT_DB);
  rssiConfig.valueTemplate = "{{ value_json.rssi }}";
  rssiConfig.stateTopic = subjectSYStoMQTT;
  auto rssiSensor = std::make_unique<omg::hass::HassSensor>(rssiConfig, gatewayDevice);
  g_discoveryManager->publishEntity(std::move(rssiSensor));
#  endif

  // Auto discovery switch
  auto discoveryConfig = omg::hass::HassEntity::EntityConfig::createSwitch(
      "SYS: Auto discovery",
      mqttPublisher.getUId("disc", "").c_str());
  discoveryConfig.valueTemplate = "{{ value_json.disc }}";
  discoveryConfig.stateTopic = subjectSYStoMQTT;
  discoveryConfig.commandTopic = subjectMQTTtoSYSset;

  auto discoverySwitchConfig = omg::hass::HassSwitch::SwitchConfig::createWithJsonPayloads(
      "{\"disc\":true,\"save\":true}",
      "{\"disc\":false,\"save\":true}");
  discoverySwitchConfig.stateOn = "true";
  discoverySwitchConfig.stateOff = "false";

  auto discoverySwitch = std::make_unique<omg::hass::HassSwitch>(discoveryConfig, discoverySwitchConfig, gatewayDevice);
  g_discoveryManager->publishEntity(std::move(discoverySwitch));

  // Restart button
  auto restartConfig = omg::hass::HassEntity::EntityConfig::createButton(
      "SYS: Restart gateway",
      mqttPublisher.getUId("restart", "").c_str());
  restartConfig.commandTopic = subjectMQTTtoSYSset;
  restartConfig.availabilityTopic = will_Topic;

  auto restartButtonConfig = omg::hass::HassButton::ButtonConfig::createRestart();
  auto restartButton = std::make_unique<omg::hass::HassButton>(restartConfig, restartButtonConfig, gatewayDevice);
  g_discoveryManager->publishEntity(std::move(restartButton));

  // Erase credentials button
  auto eraseConfig = omg::hass::HassEntity::EntityConfig::createButton(
      "SYS: Erase credentials",
      mqttPublisher.getUId("erase", "").c_str());
  eraseConfig.commandTopic = subjectMQTTtoSYSset;
  eraseConfig.availabilityTopic = will_Topic;

  auto eraseButtonConfig = omg::hass::HassButton::ButtonConfig::createGeneric("{\"cmd\":\"erase\"}");
  auto eraseButton = std::make_unique<omg::hass::HassButton>(eraseConfig, eraseButtonConfig, gatewayDevice);
  g_discoveryManager->publishEntity(std::move(eraseButton));

  THEENGS_LOG_NOTICE(F("OMG system entities published: %d total" CR), g_discoveryManager->getEntityCount());
}

//=============================================================================
// PUBLIC API FUNCTION
// Dispatches to appropriate implementation based on compilation flags
//=============================================================================
void pubMqttDiscovery() {
  THEENGS_LOG_TRACE(F("Starting refactored HA Discovery" CR));

  // Initialize discovery manager if not already done
  if (!g_discoveryManager) {
    g_discoveryManager = std::make_unique<omg::hass::HassDiscoveryManager>(
        settingsProvider,
        mqttPublisher);
  }

  // Publish system entities using new architecture
  publishSystemEntities();

  auto gatewayDevice = g_discoveryManager->getGatewayDevice();

#  if defined(ZgatewayBT) || defined(SecondaryModule)
  // BT Scan Parameters
  {
    // Interval between scans
    auto intervalConfig = omg::hass::HassEntity::EntityConfig::createNumber(
        "BT: Interval between scans",
        mqttPublisher.getUId("interval", "").c_str());
    intervalConfig.valueTemplate = "{{ value_json.interval/1000 }}";
    intervalConfig.stateTopic = subjectBTtoMQTT;
    intervalConfig.commandTopic = subjectMQTTtoBTset;
    intervalConfig.commandTemplate = "{\"interval\":{{value*1000}},\"save\":true}";
    intervalConfig.unitOfMeasurement = HASS_UNIT_SECOND;
    auto intervalNumber = std::make_unique<omg::hass::HassSensor>(intervalConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(intervalNumber));

    // Interval between active scans
    auto intervalActsConfig = omg::hass::HassEntity::EntityConfig::createNumber(
        "BT: Interval between active scans",
        mqttPublisher.getUId("intervalacts", "").c_str());
    intervalActsConfig.valueTemplate = "{{ value_json.intervalacts/1000 }}";
    intervalActsConfig.stateTopic = subjectBTtoMQTT;
    intervalActsConfig.commandTopic = subjectMQTTtoBTset;
    intervalActsConfig.commandTemplate = "{\"intervalacts\":{{value*1000}},\"save\":true}";
    intervalActsConfig.unitOfMeasurement = HASS_UNIT_SECOND;
    auto intervalActsNumber = std::make_unique<omg::hass::HassSensor>(intervalActsConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(intervalActsNumber));
  }
#  endif

#  ifdef SecondaryModule
  // Secondary module system sensors
  {
    String secondaryPrefix = String(SecondaryModule);

    // Secondary uptime sensor
    auto uptimeConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        ("SYS: Uptime " + secondaryPrefix).c_str(),
        mqttPublisher.getUId(("uptime-" + secondaryPrefix).c_str(), "").c_str(),
        HASS_CLASS_DURATION,
        HASS_UNIT_SECOND);
    uptimeConfig.valueTemplate = "{{ value_json.uptime }}";
    uptimeConfig.stateTopic = subjectSYStoMQTTSecondaryModule;
    uptimeConfig.stateClass = stateClassMeasurement;
    auto uptimeSensor = std::make_unique<omg::hass::HassSensor>(uptimeConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(uptimeSensor));

    // Secondary free memory sensor
    auto freememConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        ("SYS: Free memory " + secondaryPrefix).c_str(),
        mqttPublisher.getUId(("freemem-" + secondaryPrefix).c_str(), "").c_str(),
        HASS_CLASS_DATA_SIZE,
        HASS_UNIT_BYTE);
    freememConfig.valueTemplate = "{{ value_json.freemem }}";
    freememConfig.stateTopic = subjectSYStoMQTTSecondaryModule;
    freememConfig.stateClass = stateClassMeasurement;
    auto freememSensor = std::make_unique<omg::hass::HassSensor>(freememConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(freememSensor));

    // Secondary restart button
    auto restartConfig = omg::hass::HassEntity::EntityConfig::createButton(
        ("SYS: Restart " + secondaryPrefix).c_str(),
        mqttPublisher.getUId(("restart-" + secondaryPrefix).c_str(), "").c_str());
    restartConfig.commandTopic = subjectMQTTtoSYSsetSecondaryModule;
    restartConfig.availabilityTopic = will_Topic;

    auto restartButtonConfig = omg::hass::HassButton::ButtonConfig::createRestart();
    auto restartButton = std::make_unique<omg::hass::HassButton>(restartConfig, restartButtonConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(restartButton));
  }
#  endif

#  ifdef LED_ADDRESSABLE
  // LED Brightness control
  {
    auto ledBrightnessConfig = omg::hass::HassEntity::EntityConfig::createNumber(
        "SYS: LED Brightness",
        mqttPublisher.getUId("rgbb", "").c_str());
    ledBrightnessConfig.valueTemplate = "{{ (value_json.rgbb/2.55) | round(0) }}";
    ledBrightnessConfig.stateTopic = subjectSYStoMQTT;
    ledBrightnessConfig.commandTopic = subjectMQTTtoSYSset;
    ledBrightnessConfig.commandTemplate = "{\"rgbb\":{{ (value*2.55) | round(0) }},\"save\":true}";
    ledBrightnessConfig.availabilityTopic = will_Topic;
    auto ledBrightnessNumber = std::make_unique<omg::hass::HassSensor>(ledBrightnessConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(ledBrightnessNumber));
  }
#  endif

#  ifdef ZdisplaySSD1306
  // SSD1306 Display Controls
  {
    // Display on/off control
    auto displayControlConfig = omg::hass::HassEntity::EntityConfig::createSwitch(
        "SSD1306: Control",
        mqttPublisher.getUId("onstate", "").c_str());
    displayControlConfig.valueTemplate = "{{ value_json.onstate }}";
    displayControlConfig.stateTopic = subjectSSD1306toMQTT;
    displayControlConfig.commandTopic = subjectMQTTtoSSD1306set;

    auto displaySwitchConfig = omg::hass::HassSwitch::SwitchConfig::createWithJsonPayloads(
        "{\"onstate\":true,\"save\":true}",
        "{\"onstate\":false,\"save\":true}");
    displaySwitchConfig.stateOn = "true";
    displaySwitchConfig.stateOff = "false";

    auto displaySwitch = std::make_unique<omg::hass::HassSwitch>(displayControlConfig, displaySwitchConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(displaySwitch));

    // Display metric toggle
    auto metricConfig = omg::hass::HassEntity::EntityConfig::createSwitch(
        "SSD1306: Display metric",
        mqttPublisher.getUId("displayMetric", "").c_str());
    metricConfig.valueTemplate = "{{ value_json.displayMetric }}";
    metricConfig.stateTopic = subjectWebUItoMQTT;
    metricConfig.commandTopic = subjectMQTTtoWebUIset;

    auto metricSwitchConfig = omg::hass::HassSwitch::SwitchConfig::createWithJsonPayloads(
        "{\"displayMetric\":true,\"save\":true}",
        "{\"displayMetric\":false,\"save\":true}");
    metricSwitchConfig.stateOn = "true";
    metricSwitchConfig.stateOff = "false";

    auto metricSwitch = std::make_unique<omg::hass::HassSwitch>(metricConfig, metricSwitchConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(metricSwitch));

    // Display brightness control
    auto brightnessConfig = omg::hass::HassEntity::EntityConfig::createNumber(
        "SSD1306: Brightness",
        mqttPublisher.getUId("brightness", "").c_str());
    brightnessConfig.valueTemplate = "{{ value_json.brightness }}";
    brightnessConfig.stateTopic = subjectSSD1306toMQTT;
    brightnessConfig.commandTopic = subjectMQTTtoSSD1306set;
    brightnessConfig.commandTemplate = "{\"brightness\":{{value}},\"save\":true}";
    auto brightnessNumber = std::make_unique<omg::hass::HassSensor>(brightnessConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(brightnessNumber));
  }
#  endif

#  if defined(ESP32) && !defined(NO_INT_TEMP_READING)
  // ESP32 Internal Sensors
  {
    // Internal temperature
    auto tempConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "SYS: Internal temperature",
        mqttPublisher.getUId("tempc", "").c_str(),
        HASS_CLASS_TEMPERATURE,
        HASS_UNIT_CELSIUS);
    tempConfig.valueTemplate = "{{ value_json.tempc | round(1) }}";
    tempConfig.stateTopic = subjectSYStoMQTT;
    tempConfig.stateClass = stateClassMeasurement;
    auto tempSensor = std::make_unique<omg::hass::HassSensor>(tempConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(tempSensor));

#    if defined(ZboardM5STICKC) || defined(ZboardM5STICKCP) || defined(ZboardM5TOUGH)
    // M5 Battery voltage
    auto batVoltConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "SYS: Bat voltage",
        mqttPublisher.getUId("m5batvoltage", "").c_str(),
        HASS_CLASS_VOLTAGE,
        HASS_UNIT_VOLT);
    batVoltConfig.valueTemplate = "{{ value_json.m5batvoltage }}";
    batVoltConfig.stateTopic = subjectSYStoMQTT;
    auto batVoltSensor = std::make_unique<omg::hass::HassSensor>(batVoltConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(batVoltSensor));

    // M5 Battery current
    auto batCurrentConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "SYS: Bat current",
        mqttPublisher.getUId("m5batcurrent", "").c_str(),
        HASS_CLASS_CURRENT,
        HASS_UNIT_AMP);
    batCurrentConfig.valueTemplate = "{{ value_json.m5batcurrent }}";
    batCurrentConfig.stateTopic = subjectSYStoMQTT;
    auto batCurrentSensor = std::make_unique<omg::hass::HassSensor>(batCurrentConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(batCurrentSensor));

    // M5 VIN voltage
    auto vinVoltConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "SYS: Vin voltage",
        mqttPublisher.getUId("m5vinvoltage", "").c_str(),
        HASS_CLASS_VOLTAGE,
        HASS_UNIT_VOLT);
    vinVoltConfig.valueTemplate = "{{ value_json.m5vinvoltage }}";
    vinVoltConfig.stateTopic = subjectSYStoMQTT;
    auto vinVoltSensor = std::make_unique<omg::hass::HassSensor>(vinVoltConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(vinVoltSensor));

    // M5 VIN current
    auto vinCurrentConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "SYS: Vin current",
        mqttPublisher.getUId("m5vincurrent", "").c_str(),
        HASS_CLASS_CURRENT,
        HASS_UNIT_AMP);
    vinCurrentConfig.valueTemplate = "{{ value_json.m5vincurrent }}";
    vinCurrentConfig.stateTopic = subjectSYStoMQTT;
    auto vinCurrentSensor = std::make_unique<omg::hass::HassSensor>(vinCurrentConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(vinCurrentSensor));
#    endif

#    ifdef ZboardM5STACK
    // M5 Battery level
    auto batLevelConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "SYS: Batt level",
        mqttPublisher.getUId("m5battlevel", "").c_str(),
        HASS_CLASS_BATTERY,
        HASS_UNIT_PERCENT);
    batLevelConfig.valueTemplate = "{{ value_json.m5battlevel }}";
    batLevelConfig.stateTopic = subjectSYStoMQTT;
    auto batLevelSensor = std::make_unique<omg::hass::HassSensor>(batLevelConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(batLevelSensor));

    // M5 Is Charging
    auto isChargingConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "SYS: Is Charging",
        mqttPublisher.getUId("m5ischarging", "").c_str());
    isChargingConfig.componentType = "binary_sensor";
    isChargingConfig.valueTemplate = "{{ value_json.m5ischarging }}";
    isChargingConfig.stateTopic = subjectSYStoMQTT;
    auto isChargingSensor = std::make_unique<omg::hass::HassSensor>(isChargingConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(isChargingSensor));

    // M5 Is Charge Full
    auto chargeFullConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "SYS: Is Charge Full",
        mqttPublisher.getUId("m5ischargefull", "").c_str());
    chargeFullConfig.componentType = "binary_sensor";
    chargeFullConfig.valueTemplate = "{{ value_json.m5ischargefull }}";
    chargeFullConfig.stateTopic = subjectSYStoMQTT;
    auto chargeFullSensor = std::make_unique<omg::hass::HassSensor>(chargeFullConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(chargeFullSensor));
#    endif
  }
#  endif

#  ifdef ZsensorBME280
  // BME280 Sensors
  {
    THEENGS_LOG_TRACE(F("BME280 Discovery" CR));

    auto tempConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "BME: Temp",
        mqttPublisher.getUId("bme-temp", "").c_str(),
        HASS_CLASS_TEMPERATURE,
        HASS_UNIT_CELSIUS);
    tempConfig.valueTemplate = jsonTempc;
    tempConfig.stateTopic = BMETOPIC;
    tempConfig.stateClass = stateClassMeasurement;
    auto tempSensor = std::make_unique<omg::hass::HassSensor>(tempConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(tempSensor));

    auto pressureConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "BME: Pressure",
        mqttPublisher.getUId("bme-pressure", "").c_str(),
        HASS_CLASS_PRESSURE,
        HASS_UNIT_HPA);
    pressureConfig.valueTemplate = jsonPa;
    pressureConfig.stateTopic = BMETOPIC;
    pressureConfig.stateClass = stateClassMeasurement;
    auto pressureSensor = std::make_unique<omg::hass::HassSensor>(pressureConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(pressureSensor));

    auto humConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "BME: Humidity",
        mqttPublisher.getUId("bme-humidity", "").c_str(),
        HASS_CLASS_HUMIDITY,
        HASS_UNIT_PERCENT);
    humConfig.valueTemplate = jsonHum;
    humConfig.stateTopic = BMETOPIC;
    humConfig.stateClass = stateClassMeasurement;
    auto humSensor = std::make_unique<omg::hass::HassSensor>(humConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(humSensor));

    auto altimConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "BME: Altitude",
        mqttPublisher.getUId("bme-altim", "").c_str(),
        "",
        HASS_UNIT_METER);
    altimConfig.valueTemplate = jsonAltim;
    altimConfig.stateTopic = BMETOPIC;
    altimConfig.stateClass = stateClassMeasurement;
    auto altimSensor = std::make_unique<omg::hass::HassSensor>(altimConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(altimSensor));

    auto altiftConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "BME: Altitude (ft)",
        mqttPublisher.getUId("bme-altift", "").c_str(),
        "",
        HASS_UNIT_FT);
    altiftConfig.valueTemplate = jsonAltif;
    altiftConfig.stateTopic = BMETOPIC;
    altiftConfig.stateClass = stateClassMeasurement;
    auto altiftSensor = std::make_unique<omg::hass::HassSensor>(altiftConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(altiftSensor));
  }
#  endif

#  ifdef ZsensorHTU21
  // HTU21 Sensors
  {
    THEENGS_LOG_TRACE(F("HTU21 Discovery" CR));

    auto tempConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "HTU: Temperature",
        mqttPublisher.getUId("htu-temp", "").c_str(),
        HASS_CLASS_TEMPERATURE,
        HASS_UNIT_CELSIUS);
    tempConfig.valueTemplate = jsonTempc;
    tempConfig.stateTopic = HTUTOPIC;
    tempConfig.stateClass = stateClassMeasurement;
    auto tempSensor = std::make_unique<omg::hass::HassSensor>(tempConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(tempSensor));

    auto humConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "HTU: Humidity",
        mqttPublisher.getUId("htu-hum", "").c_str(),
        HASS_CLASS_HUMIDITY,
        HASS_UNIT_PERCENT);
    humConfig.valueTemplate = jsonHum;
    humConfig.stateTopic = HTUTOPIC;
    humConfig.stateClass = stateClassMeasurement;
    auto humSensor = std::make_unique<omg::hass::HassSensor>(humConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(humSensor));
  }
#  endif

#  ifdef ZsensorLM75
  // LM75 Sensor
  {
    THEENGS_LOG_TRACE(F("LM75 Discovery" CR));

    auto tempConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "LM75: Temperature",
        mqttPublisher.getUId("lm75-temp", "").c_str(),
        HASS_CLASS_TEMPERATURE,
        HASS_UNIT_CELSIUS);
    tempConfig.valueTemplate = jsonTempc;
    tempConfig.stateTopic = LM75TOPIC;
    tempConfig.stateClass = stateClassMeasurement;
    auto tempSensor = std::make_unique<omg::hass::HassSensor>(tempConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(tempSensor));
  }
#  endif

#  ifdef ZsensorAHTx0
  // AHTx0 Sensors
  {
    THEENGS_LOG_TRACE(F("AHTx0 Discovery" CR));

    auto tempConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "AHT: Temperature",
        mqttPublisher.getUId("aht-temp", "").c_str(),
        HASS_CLASS_TEMPERATURE,
        HASS_UNIT_CELSIUS);
    tempConfig.valueTemplate = jsonTempc;
    tempConfig.stateTopic = AHTTOPIC;
    tempConfig.stateClass = stateClassMeasurement;
    auto tempSensor = std::make_unique<omg::hass::HassSensor>(tempConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(tempSensor));

    auto humConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "AHT: Humidity",
        mqttPublisher.getUId("aht-hum", "").c_str(),
        HASS_CLASS_HUMIDITY,
        HASS_UNIT_PERCENT);
    humConfig.valueTemplate = jsonHum;
    humConfig.stateTopic = AHTTOPIC;
    humConfig.stateClass = stateClassMeasurement;
    auto humSensor = std::make_unique<omg::hass::HassSensor>(humConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(humSensor));
  }
#  endif

#  ifdef ZsensorDHT
  // DHT Sensors
  {
    THEENGS_LOG_TRACE(F("DHT Discovery" CR));

    auto tempConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "DHT: Temperature",
        mqttPublisher.getUId("dht-temp", "").c_str(),
        HASS_CLASS_TEMPERATURE,
        HASS_UNIT_CELSIUS);
    tempConfig.valueTemplate = jsonTempc;
    tempConfig.stateTopic = DHTTOPIC;
    tempConfig.stateClass = stateClassMeasurement;
    auto tempSensor = std::make_unique<omg::hass::HassSensor>(tempConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(tempSensor));

    auto humConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "DHT: Humidity",
        mqttPublisher.getUId("dht-hum", "").c_str(),
        HASS_CLASS_HUMIDITY,
        HASS_UNIT_PERCENT);
    humConfig.valueTemplate = jsonHum;
    humConfig.stateTopic = DHTTOPIC;
    humConfig.stateClass = stateClassMeasurement;
    auto humSensor = std::make_unique<omg::hass::HassSensor>(humConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(humSensor));
  }
#  endif

#  ifdef ZsensorADC
  // ADC Sensor
  {
    THEENGS_LOG_TRACE(F("ADC Discovery" CR));

    auto adcConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "ADC",
        mqttPublisher.getUId("adc", "").c_str());
    adcConfig.valueTemplate = jsonAdc;
    adcConfig.stateTopic = ADCTOPIC;
    adcConfig.stateClass = stateClassMeasurement;
    auto adcSensor = std::make_unique<omg::hass::HassSensor>(adcConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(adcSensor));
  }
#  endif

#  ifdef ZsensorBH1750
  // BH1750 Light Sensors
  {
    THEENGS_LOG_TRACE(F("BH1750 Discovery" CR));

    auto luxConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "BH1750: Lux",
        mqttPublisher.getUId("BH1750-lux", "").c_str(),
        HASS_CLASS_ILLUMINANCE,
        HASS_UNIT_LX);
    luxConfig.valueTemplate = jsonLux;
    luxConfig.stateTopic = subjectBH1750toMQTT;
    luxConfig.stateClass = stateClassMeasurement;
    auto luxSensor = std::make_unique<omg::hass::HassSensor>(luxConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(luxSensor));

    auto ftcdConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "BH1750: ftCd",
        mqttPublisher.getUId("BH1750-ftcd", "").c_str(),
        HASS_CLASS_IRRADIANCE);
    ftcdConfig.valueTemplate = jsonFtcd;
    ftcdConfig.stateTopic = subjectBH1750toMQTT;
    ftcdConfig.stateClass = stateClassMeasurement;
    auto ftcdSensor = std::make_unique<omg::hass::HassSensor>(ftcdConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(ftcdSensor));

    auto wm2Config = omg::hass::HassEntity::EntityConfig::createSensor(
        "BH1750: wattsm2",
        mqttPublisher.getUId("BH1750-wm2", "").c_str(),
        HASS_CLASS_IRRADIANCE,
        HASS_UNIT_WM2);
    wm2Config.valueTemplate = jsonWm2;
    wm2Config.stateTopic = subjectBH1750toMQTT;
    wm2Config.stateClass = stateClassMeasurement;
    auto wm2Sensor = std::make_unique<omg::hass::HassSensor>(wm2Config, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(wm2Sensor));
  }
#  endif

#  ifdef ZsensorMQ2
  // MQ2 Gas Sensors
  {
    THEENGS_LOG_TRACE(F("MQ2 Discovery" CR));

    auto gasConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "MQ2: gas",
        mqttPublisher.getUId("MQ2-gas", "").c_str(),
        HASS_CLASS_GAS,
        HASS_UNIT_PPM);
    gasConfig.valueTemplate = jsonVal;
    gasConfig.stateTopic = subjectMQ2toMQTT;
    gasConfig.stateClass = stateClassMeasurement;
    auto gasSensor = std::make_unique<omg::hass::HassSensor>(gasConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(gasSensor));

    auto presenceConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "MQ2",
        mqttPublisher.getUId("MQ2", "").c_str(),
        HASS_CLASS_GAS);
    presenceConfig.componentType = "binary_sensor";
    presenceConfig.valueTemplate = jsonPresence;
    presenceConfig.stateTopic = subjectMQ2toMQTT;
    auto presenceSensor = std::make_unique<omg::hass::HassSensor>(presenceConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(presenceSensor));
  }
#  endif

#  ifdef ZsensorTEMT6000
  // TEMT6000 Light Sensors
  {
    THEENGS_LOG_TRACE(F("TEMT6000 Discovery" CR));

    auto luxConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "TEMT6000: Lux",
        mqttPublisher.getUId("TEMT6000-lux", "").c_str(),
        HASS_CLASS_ILLUMINANCE,
        HASS_UNIT_LX);
    luxConfig.valueTemplate = jsonLux;
    luxConfig.stateTopic = subjectTEMT6000toMQTT;
    luxConfig.stateClass = stateClassMeasurement;
    auto luxSensor = std::make_unique<omg::hass::HassSensor>(luxConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(luxSensor));

    auto ftcdConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "TEMT6000: ftCd",
        mqttPublisher.getUId("TEMT6000-ftcd", "").c_str(),
        HASS_CLASS_IRRADIANCE);
    ftcdConfig.valueTemplate = jsonFtcd;
    ftcdConfig.stateTopic = subjectTEMT6000toMQTT;
    ftcdConfig.stateClass = stateClassMeasurement;
    auto ftcdSensor = std::make_unique<omg::hass::HassSensor>(ftcdConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(ftcdSensor));

    auto wm2Config = omg::hass::HassEntity::EntityConfig::createSensor(
        "TEMT6000: wattsm2",
        mqttPublisher.getUId("TEMT6000-wm2", "").c_str(),
        HASS_CLASS_IRRADIANCE,
        HASS_UNIT_WM2);
    wm2Config.valueTemplate = jsonWm2;
    wm2Config.stateTopic = subjectTEMT6000toMQTT;
    wm2Config.stateClass = stateClassMeasurement;
    auto wm2Sensor = std::make_unique<omg::hass::HassSensor>(wm2Config, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(wm2Sensor));
  }
#  endif

#  ifdef ZsensorTSL2561
  // TSL2561 Light Sensors
  {
    THEENGS_LOG_TRACE(F("TSL2561 Discovery" CR));

    auto luxConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "TSL2561: Lux",
        mqttPublisher.getUId("TSL2561-lux", "").c_str(),
        HASS_CLASS_ILLUMINANCE,
        HASS_UNIT_LX);
    luxConfig.valueTemplate = jsonLux;
    luxConfig.stateTopic = subjectTSL12561toMQTT;
    luxConfig.stateClass = stateClassMeasurement;
    auto luxSensor = std::make_unique<omg::hass::HassSensor>(luxConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(luxSensor));

    auto ftcdConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "TSL2561: ftCd",
        mqttPublisher.getUId("TSL2561-ftcd", "").c_str(),
        HASS_CLASS_IRRADIANCE);
    ftcdConfig.valueTemplate = jsonFtcd;
    ftcdConfig.stateTopic = subjectTSL12561toMQTT;
    ftcdConfig.stateClass = stateClassMeasurement;
    auto ftcdSensor = std::make_unique<omg::hass::HassSensor>(ftcdConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(ftcdSensor));

    auto wm2Config = omg::hass::HassEntity::EntityConfig::createSensor(
        "TSL2561: wattsm2",
        mqttPublisher.getUId("TSL2561-wm2", "").c_str(),
        HASS_CLASS_IRRADIANCE,
        HASS_UNIT_WM2);
    wm2Config.valueTemplate = jsonWm2;
    wm2Config.stateTopic = subjectTSL12561toMQTT;
    wm2Config.stateClass = stateClassMeasurement;
    auto wm2Sensor = std::make_unique<omg::hass::HassSensor>(wm2Config, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(wm2Sensor));
  }
#  endif

#  ifdef ZsensorHCSR501
  // HCSR501 Motion Sensor
  {
    THEENGS_LOG_TRACE(F("HCSR501 Discovery" CR));

    auto motionConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "hcsr501",
        mqttPublisher.getUId("hcsr501", "").c_str(),
        HASS_CLASS_MOTION);
    motionConfig.componentType = "binary_sensor";
    motionConfig.valueTemplate = jsonPresence;
    motionConfig.stateTopic = subjectHCSR501toMQTT;
    auto motionSensor = std::make_unique<omg::hass::HassSensor>(motionConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(motionSensor));
  }
#  endif

#  ifdef ZsensorGPIOInput
  // GPIO Input Sensor
  {
    THEENGS_LOG_TRACE(F("GPIOInput Discovery" CR));

    auto gpioConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "GPIOInput",
        mqttPublisher.getUId("GPIOInput", "").c_str());
    gpioConfig.componentType = "binary_sensor";
    gpioConfig.valueTemplate = jsonGpio;
    gpioConfig.stateTopic = subjectGPIOInputtoMQTT;
    auto gpioSensor = std::make_unique<omg::hass::HassSensor>(gpioConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(gpioSensor));
  }
#  endif

#  ifdef ZsensorINA226
  // INA226 Power Sensors
  {
    THEENGS_LOG_TRACE(F("INA226 Discovery" CR));

    auto voltConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "INA226: volt",
        mqttPublisher.getUId("INA226-volt", "").c_str(),
        HASS_CLASS_VOLTAGE,
        HASS_UNIT_VOLT);
    voltConfig.valueTemplate = jsonVolt;
    voltConfig.stateTopic = subjectINA226toMQTT;
    voltConfig.stateClass = stateClassMeasurement;
    auto voltSensor = std::make_unique<omg::hass::HassSensor>(voltConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(voltSensor));

    auto currentConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "INA226: current",
        mqttPublisher.getUId("INA226-current", "").c_str(),
        HASS_CLASS_CURRENT,
        HASS_UNIT_AMP);
    currentConfig.valueTemplate = jsonCurrent;
    currentConfig.stateTopic = subjectINA226toMQTT;
    currentConfig.stateClass = stateClassMeasurement;
    auto currentSensor = std::make_unique<omg::hass::HassSensor>(currentConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(currentSensor));

    auto powerConfig = omg::hass::HassEntity::EntityConfig::createSensor(
        "INA226: power",
        mqttPublisher.getUId("INA226-power", "").c_str(),
        HASS_CLASS_POWER,
        HASS_UNIT_WATT);
    powerConfig.valueTemplate = jsonPower;
    powerConfig.stateTopic = subjectINA226toMQTT;
    powerConfig.stateClass = stateClassMeasurement;
    auto powerSensor = std::make_unique<omg::hass::HassSensor>(powerConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(powerSensor));
  }
#  endif

#  ifdef ZactuatorONOFF
  // Actuator ON/OFF Switch
  {
    THEENGS_LOG_TRACE(F("ActuatorONOFF Discovery" CR));

    auto actuatorConfig = omg::hass::HassEntity::EntityConfig::createSwitch(
        "actuatorONOFF",
        mqttPublisher.getUId("actuatorONOFF", "").c_str());
    actuatorConfig.valueTemplate = "{{ value_json.cmd }}";
    actuatorConfig.stateTopic = subjectGTWONOFFtoMQTT;
    actuatorConfig.commandTopic = subjectMQTTtoONOFF;

    auto actuatorSwitchConfig = omg::hass::HassSwitch::SwitchConfig::createWithJsonPayloads(
        "{\"cmd\":1}",
        "{\"cmd\":0}");
    actuatorSwitchConfig.stateOn = "1";
    actuatorSwitchConfig.stateOff = "0";

    auto actuatorSwitch = std::make_unique<omg::hass::HassSwitch>(actuatorConfig, actuatorSwitchConfig, gatewayDevice);
    g_discoveryManager->publishEntity(std::move(actuatorSwitch));
  }
#  endif

  THEENGS_LOG_TRACE(F("HMD discovery completed" CR));
}

#endif // ZmqttDiscovery2
