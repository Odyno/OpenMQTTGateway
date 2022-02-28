/*
  OpenMQTTGateway  - ESP8266 or Arduino program for home automation

   Act as a wifi or ethernet gateway between your 433mhz/infrared IR signal  and a MQTT broker
   Send and receiving command by MQTT

   This files enables to set your parameter for the Home assistant mqtt Discovery

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
#ifndef config_mqttDiscovery_h
#define config_mqttDiscovery_h

/*-------------- Auto discovery macros-----------------*/
// Set the line below to true so as to have autodiscovery working with OpenHAB
#ifndef OpenHABDiscovery
#  define OpenHABDiscovery false
#endif

#if OpenHABDiscovery // OpenHAB autodiscovery value key definition (is defined command is not supported by OpenHAB)
#  define jsonBatt     "{{ value_json.batt }}"
#  define jsonLux      "{{ value_json.lux }}"
#  define jsonPres     "{{ value_json.pres }}"
#  define jsonFer      "{{ value_json.fer }}"
#  define jsonFor      "{{ value_json.for }}"
#  define jsonMoi      "{{ value_json.moi }}"
#  define jsonHum      "{{ value_json.hum }}"
#  define jsonStep     "{{ value_json.steps }}"
#  define jsonWeight   "{{ value_json.weight }}"
#  define jsonPresence "{{ value_json.presence }}"
#  define jsonAltim    "{{ value_json.altim }}"
#  define jsonAltif    "{{ value_json.altift }}"
#  define jsonTempc    "{{ value_json.tempc }}"
#  define jsonTempc2   "{{ value_json.tempc2 }}"
#  define jsonTempc3   "{{ value_json.tempc3 }}"
#  define jsonTempc4   "{{ value_json.tempc4 }}"
#  define jsonTempf    "{{ value_json.tempf }}"
#  define jsonMsg      "{{ value_json.message }}"
#  define jsonVal      "{{ value_json.value }}"
#  define jsonVolt     "{{ value_json.volt }}"
#  define jsonCurrent  "{{ value_json.current }}"
#  define jsonPower    "{{ value_json.power }}"
#  define jsonEnergy   "{{ value_json.energy }}"
#  define jsonGpio     "{{ value_json.gpio }}"
#  define jsonFtcd     "{{ value_json.ftcd }}"
#  define jsonWm2      "{{ value_json.wattsm2 }}"
#  define jsonAdc      "{{ value_json.adc }}"
#  define jsonPa       "{{ float(value_json.pa) * 0.01 }}"
#  define jsonId       "{{ value_json.id }}"
#  define jsonAddress  "{{ value_json.address }}"
#  define jsonOpen     "{{ value_json.open }}"
#  define jsonTime     "{{ value_json.time }}"
#  define jsonCount    "{{ value_json.count }}"
#  define jsonAlarm    "{{ value_json.alarm }}"
#endif

#include <HADiscovery.hpp>
extern void pubOMGFeaturesOnMqttDiscovery(HADiscovery iHADiscovery);

// From https://github.com/home-assistant/core/blob/d7ac4bd65379e11461c7ce0893d3533d8d8b8cbf/homeassistant/const.py#L379
// List of units available in Home Assistant
const char* availableHASSUnits[] = {"W",
                                    "kW",
                                    "V",
                                    "A",
                                    "W",
                                    "°C",
                                    "°F",
                                    "ms",
                                    "s",
                                    "hPa",
                                    "kg",
                                    "lb",
                                    "µS/cm",
                                    "lx",
                                    "%",
                                    "dB",
                                    "B"};

#endif
