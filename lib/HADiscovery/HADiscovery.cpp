#include "HADiscovery.hpp"

#include <ArduinoLog.h>
#include <Constant.h>

#include <Utilities.hpp>

const char* HADiscovery::StateClassValue[] = {
    NULL,
    "measurement",
    "total",
    "total_increasing",
};

const char* HADiscovery::SensorDeviceClassValue[] = {
    NULL, // Generic sensor. This is the default and doesn’t need to be set.
    "aqi", // Air Quality Index
    "battery", // Percentage of battery that is left.
    "carbon_dioxide", // Carbon Dioxide in CO2 (Smoke)
    "carbon_monoxide", // Carbon Monoxide in CO (Gas CNG/LPG)
    "current", // Current in A.
    "date", // Date string (ISO 8601).
    "energy", // Energy in Wh, kWh or MWh.
    "frequency", // Frequency in Hz, kHz, MHz or GHz.
    "gas", // Gasvolume in m³ or ft³.
    "humidity", // Percentage of humidity in the air.
    "illuminance", // The current light level in lx or lm.
    "monetary", // The monetary value.
    "nitrogen_dioxide", // Concentration of Nitrogen Dioxide in µg/m³
    "nitrogen_monoxide", // Concentration of Nitrogen Monoxide in µg/m³
    "nitrous_oxide", // Concentration of Nitrous Oxide in µg/m³
    "ozone", // Concentration of Ozone in µg/m³
    "pm1", // Concentration of particulate matter less than 1 micrometer in µg/m³
    "pm10", // Concentration of particulate matter less than 10 micrometers in µg/m³
    "pm25", // Concentration of particulate matter less than 2.5 micrometers in µg/m³
    "power_factor", // Power factor in %.
    "power", // Power in W or kW.
    "pressure", // Pressure in hPa or mbar.
    "signal_strength", // Signal strength in dB or dBm.
    "sulphur_dioxide", // Concentration of sulphur dioxide in µg/m³
    "temperature", // Temperature in °C or °F.
    "timestamp", // Datetime object or timestamp string (ISO 8601).
    "volatile_organic_compounds", // Concentration of volatile organic compounds in µg/m³.
    "voltage", // Voltage in V.
};

const char* HADiscovery::BinarySensorDeviceClassValue[] = {
    NULL, // Generic on/off. This is the default and doesn’t need to be set.
    "battery", // on means low, off means normal
    "battery_charging", // on means charging, off means not charging
    "cold", // on means cold, off means normal
    "connectivity", // on means connected, off means disconnected
    "door", // on means open, off means closed
    "garage_door", // on means open, off means closed
    "gas", // on means gas detected, off means no gas (clear)
    "heat", // on means hot, off means normal
    "light", // on means light detected, off means no light
    "lock", // on means open (unlocked), off means closed (locked)
    "moisture", // on means moisture detected (wet), off means no moisture (dry)
    "motion", // on means motion detected, off means no motion (clear)
    "moving", // on means moving, off means not moving (stopped)
    "occupancy", // on means occupied, off means not occupied (clear)
    "opening", // on means open, off means closed
    "plug", // on means device is plugged in, off means device is unplugged
    "power", // on means power detected, off means no power
    "presence", // on means home, off means away
    "problem", // on means problem detected, off means no problem (OK)
    "running", // on means running, off means not running
    "safety", // on means unsafe, off means safe
    "smoke", // on means smoke detected, off means no smoke (clear)
    "sound", // on means sound detected, off means no sound (clear)
    "tamper", // on means tampering detected, off means no tampering (clear)
    "update", // on means update available, off means up-to-date
    "vibration", // on means vibration detected, off means no vibration (clear)
    "window", // on means open, off means closed
};

HADiscovery::HADiscovery(MQTTPublisher oneMQTTPublisher,
                         const char* oneDefaultTopic,
                         const char* oneGatewayName,
                         const char* oneGatewayModels) : iMQTTPublisher(oneMQTTPublisher),
                                                         defaultTopic(oneDefaultTopic),
                                                         gatewayName(oneGatewayName),
                                                         gatewayModels(oneGatewayModels){};

HADiscovery::~HADiscovery(){};

const char* HADiscovery::makeStateTopic(const char* st_topic, JsonObject& device) {
  // If a component cannot render it's state (f.i. KAKU relays) no state topic
  // should be added. Without a state topic HA will use optimistic mode for the
  // component by default. The Home Assistant UI for optimistic switches
  // (separate on and off icons) allows for multiple
  // subsequent on commands. This is required for dimming on KAKU relays like
  // the ACM-300.
  if (st_topic[0]) {
    char state_topic[mqtt_topic_max_size];
    // If not an entity belonging to the gateway we put wild card for the location and gateway name
    // allowing to have the entity detected by several gateways and a consistent discovery topic among the gateways
    if (isOMGDevice(device)) {
      strcpy(state_topic, defaultTopic);
      strcat(state_topic, device["name"]);
    } else {
      strcpy(state_topic, "+/+");
    }
    strcat(state_topic, st_topic);
    return state_topic;
  };
};

boolean HADiscovery::isOMGDevice(JsonObject iOuputDevice) {
  return iOuputDevice["name"] == gatewayName;
};

/**
 * @brief Make device object with OMG information
 * 
 * @param pOuputDevice 
 */
void HADiscovery::makeOMGDevice(JsonObject* pOuputDevice) {
  makeDevice(pOuputDevice,
             OMG::getOMGUniqueId(gatewayName, "").c_str(),
             gatewayName,
             gatewayModels,
             DEVICEMANUFACTURER,
             OMG_VERSION,
             OMG::getMacAddress().c_str());
};

void HADiscovery::makeDevice(JsonObject* pOuputDevice, const char* name, const char* model, const char* manufacturer, const char* version, const char* macAdr) {
  makeDevice(pOuputDevice,
             OMG::getUniqueId(macAdr, name, NULL).c_str(),
             name,
             model,
             manufacturer,
             version,
             macAdr);
};

/**
 * @brief make the devices aspect of mqtt discovery
 * 
 * @param output 
 * @param deviceId 
 * @param name 
 * @param model 
 * @param manufacturer 
 * @param version 
 * @param macAdr 
 */
void HADiscovery::makeDevice(JsonObject* pOuputDevice, const char* deviceId, const char* name, const char* model, const char* manufacturer, const char* version, const char* macAdr) {
  JsonObject ouputDevice = *pOuputDevice;
  //Device representing the actual sensor/switch device
  //The Device ID
  JsonArray identifiers = ouputDevice.createNestedArray("identifiers");
  char deviceid[13];
  memcpy(deviceid, &deviceId[0], 12);
  deviceid[12] = '\0';
  identifiers.add(deviceid);

  if (name[0]) {
    ouputDevice["name"] = name;
  }
  if (model[0]) {
    ouputDevice["model"] = model;
  }
  if (manufacturer[0]) {
    ouputDevice["manufacturer"] = manufacturer;
  }
  if (version[0]) {
    ouputDevice["sw_version"] = version;
  }

  ouputDevice["via_device"] = gatewayName;

  //The Connections
  if (macAdr[0] != 0) {
    JsonArray connections = ouputDevice.createNestedArray("connections");
    JsonArray connection_mac = connections.createNestedArray();
    connection_mac.add("mac");
    connection_mac.add(macAdr);
  };
};

/**
 * @brief Create a message for Discovery Device Trigger. For HA @see https://www.home-assistant.io/integrations/device_trigger.mqtt/
 *
 * @param id        The IDs that uniquely identify the device. For example a serial number.
 * @param topic     The Topic  where the trigger will publish the content
 * @param type      The type of the trigger, e.g. button_short_press. Entries supported by the HA Frontend: button_short_press, button_short_release, button_long_press, button_long_release, button_double_press, button_triple_press, button_quadruple_press, button_quintuple_press. If set to an unsupported value, will render as subtype type, e.g. button_1 spammed with type set to spammed and subtype set to button_1
 * @param subtype   The subtype of the trigger, e.g. button_1. Entries supported by the HA frontend: turn_on, turn_off, button_1, button_2, button_3, button_4, button_5, button_6. If set to an unsupported value, will render as subtype type, e.g. left_button pressed with type set to button_short_press and subtype set to left_button
 * @param device    The device information
 * 
 */
void HADiscovery::createDeviceTrigger(const char* id, const char* topic, const char* type, const char* subtype, JsonObject& device) {
  if (id[0] != 0 || topic[0] != 0 || device != 0) {
    Log.trace(F("Device trigger without some mandatory attribute is not possible! aborted " CR));
  } else {
    //Create The Json
    // StaticJsonDocument<JSON_MSG_BUFFER> jsonBuffer;
    // JsonObject sensor = jsonBuffer.to<JsonObject>();
    StaticJsonDocument<JSON_MSG_BUFFER> sensor;

    // SET Default Configuration
    sensor["automation_type"] = "trigger"; // The type of automation, must be ‘trigger’.
    //SET TYPE
    if (type[0])
      sensor["type"] = type;
    else
      sensor["type"] = "button_short_press";
    //SET SUBTYPE
    if (subtype[0])
      sensor["subtype"] = subtype;
    else
      sensor["subtype"] = "turn_on";

    /* Set The topic */
    char state_topic[mqtt_topic_max_size];
    strcpy(state_topic, defaultTopic);
    strcat(state_topic, device["name"]);
    strcat(state_topic, topic);
    sensor["topic"] = state_topic;

    sensor["device"].add(device); //device representing the board

    std::string buffer = "";
    serializeJson(sensor, buffer);
    /* Publish on the topic */
    const char* topic_to_publish = std::string(discovery_Topic).append("/device_automation/").append(id).append("/config").c_str();
    Log.trace(F("Announce Device Trigger  %s" CR), topic_to_publish);
    iMQTTPublisher.publish(topic_to_publish, buffer.c_str(), true);
  };
};

/**
 * @brief Create Sensor. For HA @see https://www.home-assistant.io/integrations/sensor.mqtt/
 * 
 * @param id 
 * @param state_topic 
 * @param name 
 * @param value_template 
 * @param unit_of_meas 
 * @param deviceClass 
 * @param stateClass 
 * @param availability_topic 
 * @param device 
 */
void HADiscovery::createSensor(const char* id, const char* state_topic, const char* name,
                               const char* value_template, const char* unit_of_meas, SensorDeviceClass deviceClass, StateClass stateClass, const char* availability_topic, JsonObject& device) {
  if (id[0]) {
    Log.trace(F("Switch without ID not possible! aborted " CR));
  } else {
    // StaticJsonDocument < JSON_MSG_BUFFER> jsonBuffer;
    // JsonObject sensor = jsonBuffer.to < JsonObject>();
    StaticJsonDocument<JSON_MSG_BUFFER> sensor;

    sensor["uniq_id"] = id;
    sensor["stat_t"] = makeStateTopic(state_topic, device); //generic topic
    if (stateClass != StateClass::none)
      sensor["state_class"] = StateClassValue[static_cast<int>(stateClass)]; //add the state class on the sensors ( https://developers.home-assistant.io/docs/core/entity/sensor/#available-state-classes )

    if (deviceClass != SensorDeviceClass::generic)
      sensor["dev_cla"] = SensorDeviceClassValue[static_cast<int>(deviceClass)]; //device_class

    if (name[0])
      sensor["name"] = name; //name

    if (value_template[0])
      sensor["val_tpl"] = value_template; //value_template

    if (unit_of_meas[0])
      sensor["unit_of_meas"] = unit_of_meas; //unit_of_measurement*/

    if (availability_topic[0])
      sensor["availability_topic"] = availability_topic;

    sensor["device"].add(device);

    std::string buffer = "";
    serializeJson(sensor, buffer);

    std::string topic = std::string(discovery_Topic) + "/sensor/" + std::string(id) + "/config";
    Log.trace(F("Announce sensor on  %s" CR), topic.c_str());
    iMQTTPublisher.publish(topic.c_str(), buffer.c_str(), true);
  };
};

/**
 * @brief Create Switch. For HA @see https://www.home-assistant.io/integrations/switch.mqtt/
 *  
 * 
 * @param id           An ID that uniquely identifies this switch device. If two switches have the same unique ID, Home Assistant will raise an exception.
 * @param name         The name to use when displaying this switch.
 * @param state_topic             The MQTT topic subscribed to receive state updates.
 * @param value_template          Defines a template to extract device’s state from the state_topic. To determine the switches’s state result of this template will be compared to payload_on if defined, else ON and payload_off if defined, else OFF.
 * @param command_topic           The MQTT topic to publish commands to change the switch state.
 * @param payload_on              The payload that represents (default: ON).          
 * @param payload_off             The payload that represents (default: OFF).        
 * @param availability_topic      The MQTT topic subscribed to receive availability (online/offline) updates.
 * @param payload_available       The payload that represents the available state. ("online" default)
 * @param payload_not_avalaible   The payload that represents the unavailable state. ("offline" default)
 * @param device 
 */
void HADiscovery::createSwitch(
    const char* id, const char* name,
    const char* state_topic, const char* value_template,
    const char* command_topic, const char* payload_on, const char* payload_off,
    const char* availability_topic, const char* payload_available, const char* payload_not_avalaible,
    JsonObject& device) {
  if (id[0]) {
    Log.trace(F("Switch without ID not possible! aborted " CR));
  } else {
    // StaticJsonDocument<JSON_MSG_BUFFER> jsonBuffer;
    // JsonObject sensor = jsonBuffer.to<JsonObject>();
    StaticJsonDocument<JSON_MSG_BUFFER> sensor;

    sensor["uniq_id"] = id; //unique_id
    sensor["stat_t"] = makeStateTopic(state_topic, device); //generic topic

    if (name[0])
      sensor["name"] = name; //name
    if (value_template[0])
      sensor["val_tpl"] = value_template; //value_template
    if (payload_on[0])
      sensor["pl_on"] = payload_on; // payload_on
    if (payload_off[0])
      sensor["pl_off"] = payload_off; //payload_off
    if (availability_topic[0])
      sensor["availability_topic"] = availability_topic;
    if (payload_available[0])
      sensor["pl_avail"] = payload_available; // payload_on

    if (payload_not_avalaible[0])
      sensor["pl_not_avail"] = payload_not_avalaible; //payload_off

    if (command_topic[0]) {
      char command_topic[mqtt_topic_max_size];
      strcpy(command_topic, defaultTopic);
      strcat(command_topic, gatewayName);
      strcat(command_topic, command_topic);
      sensor["cmd_t"] = command_topic; //command_topic
    }

    sensor["device"].add(device);

    std::string topic = std::string(discovery_Topic) + "/switch/" + std::string(id) + "/config";
    std::string buffer = "";
    serializeJson(sensor, buffer);

    Log.trace(F("Announce switch on  %s" CR), topic);
    iMQTTPublisher.publish(topic.c_str(), buffer.c_str(), true);
  };
};

/**
 * @brief Create a Binary Sensor object
 * 
 * @param id           An ID that uniquely identifies this switch device. If two switches have the same unique ID, Home Assistant will raise an exception.
 * @param name         The name to use when displaying this switch.
 * @param state_topic             The MQTT topic subscribed to receive sensor’s state.
 * @param value_template          Defines a template that returns a string to be compared to payload_on/payload_off or an empty string, in which case the MQTT message will be removed. 
 * @param payload_on              The payload that represents (default: ON).          
 * @param payload_off             The payload that represents (default: OFF).        
 * @param deviceClass 
 * @param availability_topic      The MQTT topic subscribed to receive availability (online/offline) updates.
 * @param payload_available       The payload that represents the available state. ("online" default)
 * @param payload_not_avalaible   The payload that represents the unavailable state. ("offline" default)
 * @param device                  Information about the device this sensor is a part of
* @param device 
 */
void HADiscovery::createBinarySensor(
    const char* id, const char* name,
    const char* state_topic, const char* value_template, const char* payload_on, const char* payload_off,
    BinarySensorDeviceClass deviceClass,
    const char* availability_topic, const char* payload_available, const char* payload_not_avalaible,
    JsonObject& device) {
  if (id[0]) {
    Log.trace(F("Binary Switch without ID not possible! aborted " CR));
  } else {
    StaticJsonDocument<JSON_MSG_BUFFER> jsonBuffer;
    JsonObject sensor = jsonBuffer.to<JsonObject>();

    sensor["uniq_id"] = id; //unique_id
    if (name[0])
      sensor["name"] = name; //name

    sensor["stat_t"] = makeStateTopic(state_topic, device); //generic topic

    if (value_template[0])
      sensor["val_tpl"] = value_template; //value_template
    if (payload_on[0])
      sensor["pl_on"] = payload_on; // payload_on
    if (payload_off[0])
      sensor["pl_off"] = payload_off; //payload_off

    if (BinarySensorDeviceClass::generic != deviceClass)
      sensor["dev_cla"] = BinarySensorDeviceClassValue[static_cast<int>(deviceClass)]; //device_class

    if (availability_topic[0])
      sensor["availability_topic"] = availability_topic;

    if (payload_available[0])
      sensor["pl_avail"] = payload_available; // payload_available
    if (payload_not_avalaible[0])
      sensor["pl_not_avail"] = payload_not_avalaible; //payload_not_avalaible

    if (device)
      sensor["device"] = device;

    std::string topic = std::string(discovery_Topic) + "/binary_sensor/" + std::string(id) + "/config";
    Log.trace(F("Announce binary sensor on  %s" CR), topic.c_str());
    iMQTTPublisher.publish(topic.c_str(), sensor, true);
  };
};
