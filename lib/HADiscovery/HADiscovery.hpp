#pragma once

#ifndef HASSDiscovery_H
#  define HASSDiscovery_H

#  include <Arduino.h>
#  include <ArduinoJson.h>
#  include <MQTTPublisher.hpp>
#  include <Utilities.hpp>
#  define discovery_Topic    "homeassistant"
#  define DEVICEMANUFACTURER "OMG_community"

#  define jsonUptime   "{{ value_json.uptime | is_defined }}"
#  define jsonBatt     "{{ value_json.batt | is_defined }}"
#  define jsonLux      "{{ value_json.lux | is_defined }}"
#  define jsonPres     "{{ value_json.pres | is_defined }}"
#  define jsonFer      "{{ value_json.fer | is_defined }}"
#  define jsonFor      "{{ value_json.for | is_defined }}"
#  define jsonMoi      "{{ value_json.moi | is_defined }}"
#  define jsonHum      "{{ value_json.hum | is_defined }}"
#  define jsonStep     "{{ value_json.steps | is_defined }}"
#  define jsonWeight   "{{ value_json.weight | is_defined }}"
#  define jsonPresence "{{ value_json.presence | is_defined }}"
#  define jsonAltim    "{{ value_json.altim | is_defined }}"
#  define jsonAltif    "{{ value_json.altift | is_defined }}"
#  define jsonTempc    "{{ value_json.tempc | is_defined }}"
#  define jsonTempc2   "{{ value_json.tempc2 | is_defined }}"
#  define jsonTempc3   "{{ value_json.tempc3 | is_defined }}"
#  define jsonTempc4   "{{ value_json.tempc4 | is_defined }}"
#  define jsonTempf    "{{ value_json.tempf | is_defined }}"
#  define jsonMsg      "{{ value_json.message | is_defined }}"
#  define jsonVal      "{{ value_json.value | is_defined }}"
#  define jsonVolt     "{{ value_json.volt | is_defined }}"
#  define jsonCurrent  "{{ value_json.current | is_defined }}"
#  define jsonPower    "{{ value_json.power | is_defined }}"
#  define jsonEnergy   "{{ value_json.energy | is_defined }}"
#  define jsonGpio     "{{ value_json.gpio | is_defined }}"
#  define jsonFtcd     "{{ value_json.ftcd | is_defined }}"
#  define jsonWm2      "{{ value_json.wattsm2 | is_defined }}"
#  define jsonAdc      "{{ value_json.adc | is_defined }}"
#  define jsonPa       "{{ float(value_json.pa) * 0.01 | is_defined }}"
#  define jsonId       "{{ value_json.id | is_defined }}"
#  define jsonAddress  "{{ value_json.address | is_defined }}"
#  define jsonOpen     "{{ value_json.open | is_defined }}"
#  define jsonTime     "{{ value_json.time | is_defined }}"
#  define jsonCount    "{{ value_json.count | is_defined }}"
#  define jsonAlarm    "{{ value_json.alarm | is_defined }}"

class HADiscovery {
private:
  static const char* StateClassValue[];
  static const char* BinarySensorDeviceClassValue[];
  static const char* SensorDeviceClassValue[];
  MQTTPublisher iMQTTPublisher;
  const char* defaultTopic;
  const char* gatewayName;
  const char* gatewayModels;

protected:
  const char* makeStateTopic(const char* st_topic, JsonObject& device);
  boolean isOMGDevice(JsonObject iOuputDevice);

public:
  HADiscovery(MQTTPublisher oneMQTTPublisher, const char* defaultTopic, const char* gatewayName, const char* gatewayModels);
  ~HADiscovery();

  enum class StateClass {
    none,
    measurement,
    total,
    totalIncreasing,
  };

  enum class SensorDeviceClass : int {
    generic = 0, // Generic sensor. This is the default and doesn’t need to be set.
    aqi, // Air Quality Index
    battery, // Percentage of battery that is left.
    carbon_dioxide, // Carbon Dioxide in CO2 (Smoke)
    carbon_monoxide, // Carbon Monoxide in CO (Gas CNG/LPG)
    current, // Current in A.
    date, // Date string (ISO 8601).
    energy, // Energy in Wh, kWh or MWh.
    frequency, // Frequency in Hz, kHz, MHz or GHz.
    gas, // Gasvolume in m³ or ft³.
    humidity, // Percentage of humidity in the air.
    illuminance, // The current light level in lx or lm.
    monetary, // The monetary value.
    nitrogen_dioxide, // Concentration of Nitrogen Dioxide in µg/m³
    nitrogen_monoxide, // Concentration of Nitrogen Monoxide in µg/m³
    nitrous_oxide, // Concentration of Nitrous Oxide in µg/m³
    ozone, // Concentration of Ozone in µg/m³
    pm1, // Concentration of particulate matter less than 1 micrometer in µg/m³
    pm10, // Concentration of particulate matter less than 10 micrometers in µg/m³
    pm25, // Concentration of particulate matter less than 2.5 micrometers in µg/m³
    power_factor, // Power factor in %.
    power, // Power in W or kW.
    pressure, // Pressure in hPa or mbar.
    signal_strength, // Signal strength in dB or dBm.
    sulphur_dioxide, // Concentration of sulphur dioxide in µg/m³
    temperature, // Temperature in °C or °F.
    timestamp, // Datetime object or timestamp string (ISO 8601).
    volatile_organic_compounds, // Concentration of volatile organic compounds in µg/m³.
    voltage, // Voltage in V.
  };

  enum class BinarySensorDeviceClass : int {
    generic, // Generic on/off. This is the default and doesn’t need to be set.
    battery, // on means low, off means normal
    battery_charging, // on means charging, off means not charging
    cold, // on means cold, off means normal
    connectivity, // on means connected, off means disconnected
    door, // on means open, off means closed
    garage_door, // on means open, off means closed
    gas, // on means gas detected, off means no gas (clear)
    heat, // on means hot, off means normal
    light, // on means light detected, off means no light
    lock, // on means open (unlocked), off means closed (locked)
    moisture, // on means moisture detected (wet), off means no moisture (dry)
    motion, // on means motion detected, off means no motion (clear)
    moving, // on means moving, off means not moving (stopped)
    occupancy, // on means occupied, off means not occupied (clear)
    opening, // on means open, off means closed
    plug, // on means device is plugged in, off means device is unplugged
    power, // on means power detected, off means no power
    presence, // on means home, off means away
    problem, // on means problem detected, off means no problem (OK)
    running, // on means running, off means not running
    safety, // on means unsafe, off means safe
    smoke, // on means smoke detected, off means no smoke (clear)
    sound, // on means sound detected, off means no sound (clear)
    tamper, // on means tampering detected, off means no tampering (clear)
    update, // on means update available, off means up-to-date
    vibration, // on means vibration detected, off means no vibration (clear)
    window, // on means open, off means closed
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
  void makeDevice(JsonObject* pOuputDevice, const char* deviceId, const char* name, const char* model, const char* manufacturer, const char* version, const char* macAdr);

  /**
 * @brief 
 * 
 * @param pOuputDevice 
 * @param name 
 * @param model 
 * @param manufacturer 
 * @param version 
 * @param macAdr 
 */
  void makeDevice(JsonObject* pOuputDevice, const char* name, const char* model, const char* manufacturer, const char* version, const char* macAdr);

  /**
 * @brief Make device object with OMG information
 * 
 * @param pOuputDevice 
 */
  void makeOMGDevice(JsonObject* pOuputDevice);

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
  void createDeviceTrigger(const char* id, const char* topic, const char* type, const char* subtype, JsonObject& device);

  /**
 * @brief Create Sensor. For HA @see https://www.home-assistant.io/integrations/sensor.mqtt/
 * 
 * @param id                An ID that uniquely identifies this sensor. 
 * @param state_topic       The MQTT topic subscribed to receive sensor values.
 * @param name              The name of the MQTT sensor.
 * @param value_template    Defines a template to extract the value.
 * @param unit_of_meas      Defines the units of measurement of the sensor, if any.
 * @param deviceClass       The device class of the sensor. @see DeviceClass
 * @param stateClass        The state class of the sensor. @see StateClass
 * @param availability_topic  The MQTT topic subscribed to receive availability (online/offline) updates.
 * @param device               Information about the device this sensor is a part of
 */
  void createSensor(const char* id, const char* state_topic, const char* name, const char* value_template, const char* unit_of_meas, SensorDeviceClass deviceClass, StateClass stateClass, const char* availability_topic, JsonObject& device);

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
 * @param device                  Information about the device this sensor is a part of
 */
  void createSwitch(
      const char* id, const char* name,
      const char* state_topic, const char* value_template,
      const char* command_topic, const char* payload_on, const char* payload_off,
      const char* availability_topic, const char* payload_available, const char* payload_not_avalaible,
      JsonObject& device);

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
  void createBinarySensor(
      const char* id, const char* name,
      const char* state_topic, const char* value_template, const char* payload_on, const char* payload_off,
      BinarySensorDeviceClass deviceClass,
      const char* availability_topic, const char* payload_available, const char* payload_not_avalaible,
      JsonObject& device);
};
#endif
