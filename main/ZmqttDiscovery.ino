/*
  OpenMQTTGateway Addon  - ESP8266 or Arduino program for home automation

   Act as a wifi or ethernet gateway between your 433mhz/infrared IR signal  and a MQTT broker
   Send and receiving command by MQTT

   This is the Home Assistant mqtt Discovery addon.

    Copyright: (c) Rafal Herok

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

#ifdef ZmqttDiscovery

#  include <ArduinoLog.h>

#  include <HADiscovery.hpp>
#  include <MQTTPublisher.hpp>
#  include <Utilities.hpp>

void pubOMGFeaturesOnMqttDiscovery(HADiscovery iHADiscovery) {
  Log.trace(F("omgStatusDiscovery" CR));

  /* Set The Device */
  StaticJsonDocument<JSON_MSG_BUFFER> jsonDeviceBuffer;
  JsonObject device = jsonDeviceBuffer.to<JsonObject>();
  JsonArray identifiers = device.createNestedArray("identifiers");

  if (use_gateway_info) {
    char JSONmessageBuffer[JSON_MSG_BUFFER];
    serializeJson(modules, JSONmessageBuffer, sizeof(JSONmessageBuffer));
                            HADiscovery::SensorDeviceClass::power_factor,
                            HADiscovery::StateClass::total,
                            will_Topic,
                            omgDevice);

                            iHADiscovery.createBinarySensor(
                                OMG::getOMGUniqueId("m5ischarging", "SYS").c_str(), "SYS: Is Charging",
                                subjectSYStoMQTT, "{{ value_json.m5ischarging }}", NULL, NULL,
                                BinarySensorDeviceClass::battery_charging,
                                will_Topic, NULL, NULL,
                                omgDevice);

                            iHADiscovery.createBinarySensor(
                                OMG::getOMGUniqueId("m5ischargefull", "SYS").c_str(), "SYS: Is Charge Full",
                                subjectSYStoMQTT, "{{ value_json.m5ischargefull }}", NULL, NULL,
                                BinarySensorDeviceClass::battery,
                                will_Topic, NULL, NULL,
                                omgDevice);
#endif
                            iHADiscovery.createSwitch(OMG::getOMGUniqueId("restart", "SYS").c_str(), "SYS: Restart gateway",
                                                      will_Topic, NULL,
                                                      subjectMQTTtoSYSset, "{\"cmd\":\"restart\"}", NULL,
                                                      will_Topic, Gateway_AnnouncementMsg, will_Message,
                                                      omgDevice);

                            iHADiscovery.createSwitch(OMG::getOMGUniqueId("erase", "SYS").c_str(), "SYS: Erase credentials",
                                                      will_Topic, NULL,
                                                      subjectMQTTtoSYSset, "{\"cmd\":\"erase\"}", NULL,
                                                      will_Topic, Gateway_AnnouncementMsg, will_Message,
                                                      omgDevice);

                            iHADiscovery.createSwitch(OMG::getOMGUniqueId("discovery", "SYS").c_str(), "SYS: Auto discovery",
                                                      will_Topic, NULL,
                                                      subjectMQTTtoSYSset, "{\"discovery\":true}", "{\"discovery\":false}",
                                                      will_Topic, Gateway_AnnouncementMsg, will_Message,
                                                      omgDevice);

#ifdef ZsensorBME280
                            Log.trace(F("bme280Discovery" CR));

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("bme", "temp").c_str(),
                                                      BMETOPIC, "bme-temp", jsonTempc, "°C", HADiscovery::SensorDeviceClass::temperature, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("bme", "hum").c_str(),
                                                      BMETOPIC, "bme-hum", jsonHum, "%", HADiscovery::SensorDeviceClass::humidity, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("bme", "pa").c_str(),
                                                      BMETOPIC, "bme-pa", jsonPa, "hPa", HADiscovery::SensorDeviceClass::pressure, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("bme", "altim").c_str(),
                                                      BMETOPIC, "bme-altim", jsonAltim, "m", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("bme", "altift").c_str(),
                                                      BMETOPIC, "bme-altift", jsonAltif, "ft", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

#endif

#ifdef ZsensorHTU21

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("htu", "temp").c_str(),
                                                      HTUTOPIC, "htu-temp", jsonTempc, "°C", HADiscovery::SensorDeviceClass::temperature, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("bme", "hum").c_str(),
                                                      HTUTOPIC, "htu-hum", jsonHum, "%", HADiscovery::SensorDeviceClass::humidity, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

#endif

#ifdef ZsensorAHTx0

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("aht", "temp").c_str(),
                                                      AHTTOPIC, "aht-temp", jsonTempc, "°C", HADiscovery::SensorDeviceClass::temperature, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("aht", "hum").c_str(),
                                                      AHTTOPIC, "aht-hum", jsonHum, "%", HADiscovery::SensorDeviceClass::humidity, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

#endif

#ifdef ZsensorDHT

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("dht", "temp").c_str(),
                                                      DHTTOPIC, "dht-temp", jsonTempc, "°C", HADiscovery::SensorDeviceClass::temperature, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("dht", "hum").c_str(),
                                                      DHTTOPIC, "dht-hum", jsonHum, "%", HADiscovery::SensorDeviceClass::humidity, HADiscovery::StateClass::measurement, will_Topic, omgDevice);
#endif

#ifdef ZsensorADC
                            Log.trace(F("ADCDiscovery" CR));
                            //TODO Maybe here a Input number is the best solution
                            iHADiscovery.createSensor(OMG::getOMGUniqueId("adc").c_str(),
                                                      ADCTOPIC, "adc", jsonAdc, "", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

#endif

#ifdef ZsensorBH1750
                            Log.trace(F("BH1750Discovery" CR));

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("BH1750", "lux").c_str(),
                                                      subjectBH1750toMQTT, "BH1750-lux", jsonLux, "lx", HADiscovery::SensorDeviceClass::illuminance, HADiscovery::StateClass::measurement, will_Topic, omgDevice);
                            iHADiscovery.createSensor(OMG::getOMGUniqueId("BH1750", "ftCd").c_str(),
                                                      subjectBH1750toMQTT, "BH1750-ftCd", jsonFtcd, "", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::measurement, will_Topic, omgDevice);
                            iHADiscovery.createSensor(OMG::getOMGUniqueId("BH1750", "wattsm2").c_str(),
                                                      subjectBH1750toMQTT, "BH1750-wattsm2", jsonWm2, "wm²", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

#endif

#ifdef ZsensorTSL2561
                            Log.trace(F("TSL2561Discovery" CR));

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("TSL2561", "lux").c_str(),
                                                      subjectTSL12561toMQTT, "TSL2561-lux", jsonLux, "lx", HADiscovery::SensorDeviceClass::illuminance, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("TSL2561", "ftCd").c_str(),
                                                      subjectTSL12561toMQTT, "TSL2561-ftCd", jsonFtcd, "", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("TSL2561", "wattsm2").c_str(),
                                                      subjectTSL12561toMQTT, "TSL2561-wattsm2", jsonWm2, "wm²", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::measurement, will_Topic, omgDevice);
#endif

#ifdef ZsensorHCSR501
                            Log.trace(F("HCSR501Discovery" CR));
                            iHADiscovery.createBinarySensor(
                                OMG::getOMGUniqueId("hcsr501", "presence").c_str(), "hcsr501-presence",
                                subjectHCSR501toMQTT, jsonPresence, "true", "false",
                                BinarySensorDeviceClass::occupancy,
                                will_Topic, NULL, NULL,
                                omgDevice);
#endif

#ifdef ZsensorGPIOInput
                            Log.trace(F("GPIOInputDiscovery" CR));
                            iHADiscovery.createBinarySensor(
                                OMG::getOMGUniqueId("GPIOInput", "gpio").c_str(), "GPIOInput-gpio",
                                subjectGPIOInputtoMQTT, jsonGpio, INPUT_GPIO_ON_VALUE, INPUT_GPIO_OFF_VALUE,
                                BinarySensorDeviceClass::generic,
                                will_Topic, INPUT_GPIO_ON_VALUE, INPUT_GPIO_OFF_VALUE,
                                omgDevice);
#endif

#ifdef ZsensorINA226
                            Log.trace(F("INA226Discovery" CR));

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("INA226", "volt").c_str(),
                                                      subjectINA226toMQTT, "TSL2561-volt", jsonVolt, "V", HADiscovery::SensorDeviceClass::voltage, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("INA226", "current").c_str(),
                                                      subjectINA226toMQTT, "TSL2561-current", jsonCurrent, "A", HADiscovery::SensorDeviceClass::current, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

                            iHADiscovery.createSensor(OMG::getOMGUniqueId("INA226", "power").c_str(),
                                                      subjectINA226toMQTT, "TSL2561-power", jsonPower, "W", HADiscovery::SensorDeviceClass::power, HADiscovery::StateClass::measurement, will_Topic, omgDevice);

#endif

#ifdef ZsensorDS1820
                            // Publish any DS1820 sensors found on the OneWire bus
                            pubOneWire_HADiscovery(HADiscovery iHADiscovery);
#endif

#ifdef ZactuatorONOFF
                            Log.trace(F("actuatorONOFFDiscovery" CR));
                            iHADiscovery.createSwitch(OMG::getOMGUniqueId("actuator", "ONOFF").c_str(), "ONOFF: actuator" will_Topic, NULL,
                                                      subjectGTWONOFFtoMQTT, "{\"cmd\":1}", "{\"cmd\":0}",
                                                      will_Topic, NULL, NULL,
                                                      omgDevice);
#endif

#ifdef ZgatewayRF
// Sensor to display RF received value
#  ifndef valueAsASubject
    Log.trace(F("gatewayRFDiscovery" CR));
    iHADiscovery.createSensor(OMG::getOMGUniqueId("RF", "value").c_str(),
                              subjectRFtoMQTT, "RF-value", jsonVal, "", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::none, will_Topic, omgDevice);
#  endif
#endif

#ifdef ZgatewayRF2
    // Sensor to display RF received value
    Log.trace(F("gatewayRF2Discovery" CR));
    Log.trace(F("gatewayRFDiscovery" CR));
    iHADiscovery.createSensor(OMG::getOMGUniqueId("gatewayRF2", "address").c_str(),
                              subjectRF2toMQTT, "gatewayRF2-address", jsonAddress, "", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::none, will_Topic, omgDevice);
#endif

#ifdef ZgatewayRFM69
    // Sensor to display RF received value
    Log.trace(F("gatewayRFM69Discovery" CR));
    iHADiscovery.createSensor(OMG::getOMGUniqueId("gatewayRFM69", "value").c_str(),
                              subjectRFM69toMQTT, "gatewayRFM69-value", jsonVal, "", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::none, will_Topic, omgDevice);

#endif

#ifdef ZgatewayLORA
    // Sensor to display RF received value
    Log.trace(F("gatewayLORADiscovery" CR));
    iHADiscovery.createSensor(OMG::getOMGUniqueId("gatewayLORA", "message").c_str(),
                              subjectLORAtoMQTT, "gatewayLORA-message", jsonMsg, "", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::none, will_Topic, omgDevice);

#endif

#ifdef ZgatewaySRFB
    // Sensor to display RF received value
    Log.trace(F("gatewaySRFBDiscovery" CR));
    iHADiscovery.createSensor(OMG::getOMGUniqueId("gatewaySRFB", "value").c_str(),
                              subjectSRFBtoMQTT, "gatewaySRFB-value", jsonVal, "", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::none, will_Topic, omgDevice);

#endif

#ifdef ZgatewayPilight
    // Sensor to display RF received value
    Log.trace(F("gatewayPilightDiscovery" CR));
    iHADiscovery.createSensor(OMG::getOMGUniqueId("gatewayPilight", "message").c_str(),
                              subjectPilighttoMQTT, "gatewayPilight-message", jsonMsg, "", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::none, will_Topic, omgDevice);
#endif

#ifdef ZgatewayIR
    // Sensor to display IR received value
    Log.trace(F("gatewayIRDiscovery" CR));
    iHADiscovery.createSensor(OMG::getOMGUniqueId("gatewayIR", "value").c_str(),
                              subjectIRtoMQTT, "gatewayIR-value", jsonVal, "", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::none, will_Topic, omgDevice);

#endif

#ifdef Zgateway2G
    // Sensor to display 2G received value
    Log.trace(F("gateway2GDiscovery" CR));
    iHADiscovery.createSensor(OMG::getOMGUniqueId("gateway2G", "message").c_str(),
                              subject2GtoMQTT, "gateway2G-message", jsonMsg, "", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::none, will_Topic, omgDevice);
#endif

#ifdef ZgatewayBT

    iHADiscovery.createSensor(OMG::getOMGUniqueId("interval", "BT").c_str(),
                              subjectSYStoMQTT, "BT: Interval between scans", "{{ value_json.interval }}", "ms", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::none, will_Topic, omgDevice);

    iHADiscovery.createSensor(OMG::getOMGUniqueId("scanbcnct", "BT").c_str(),
                              subjectSYStoMQTT, "BT: Connnect every X scan(s)", "{{ value_json.scanbcnct }}", "", HADiscovery::SensorDeviceClass::generic, HADiscovery::StateClass::none, will_Topic, omgDevice);

    iHADiscovery.createSwitch(OMG::getOMGUniqueId("force_scan", "BT").c_str(), "BT: Force scan",
                              will_Topic, NULL,
                              subjectMQTTtoBTset, "{\"interval\":0}", NULL,
                              will_Topic, Gateway_AnnouncementMsg, will_Message,
                              omgDevice);

    iHADiscovery.createSwitch(OMG::getOMGUniqueId("only_sensors", "BT").c_str(), "BT: Publish only sensors",
                              will_Topic, NULL,
                              subjectMQTTtoBTset, "{\"onlysensors\":true}", "{\"onlysensors\":false}",
                              will_Topic, Gateway_AnnouncementMsg, will_Message,
                              omgDevice);

    iHADiscovery.createSwitch(OMG::getOMGUniqueId("hasspresence", "BT").c_str(), "BT: Publish presence on HA",
                              will_Topic, NULL,
                              subjectMQTTtoBTset, "{\"hasspresence\":true}", "{\"hasspresence\":false}",
                              will_Topic, Gateway_AnnouncementMsg, will_Message,
                              omgDevice);

#  ifdef ESP32
    iHADiscovery.createSwitch(OMG::getOMGUniqueId("lowpowermode", "BT").c_str(), "BT: Low Power Mode command",
                              will_Topic, NULL,
                              subjectMQTTtoBTset, "{\"lowpowermode\":2}", "{\"lowpowermode\":0}",
                              will_Topic, Gateway_AnnouncementMsg, will_Message,
                              omgDevice);

    iHADiscovery.createSwitch(OMG::getOMGUniqueId("bleconnect", "BT").c_str(), "BT: Connect to devices",
                              will_Topic, NULL,
                              subjectMQTTtoBTset, "{\"bleconnect\":true}", "{\"bleconnect\":false}",
                              will_Topic, Gateway_AnnouncementMsg, will_Message,
                              omgDevice);

#  endif
#endif
  }

#endif
