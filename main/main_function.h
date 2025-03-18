/*
  OpenMQTTGateway  - ESP8266 or Arduino program for home automation

   Act as a gateway between your 433mhz, infrared IR, BLE, LoRa signal and one interface like an MQTT broker
   Send and receiving command by MQTT

  This program enables to:
 - receive MQTT data from a topic and send signal (RF, IR, BLE, GSM)  corresponding to the received MQTT data
 - publish MQTT data to a different topic related to received signals (RF, IR, BLE, GSM)

  Copyright: (c)Florian ROBERT

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
#ifndef MAIN_FUNCTION_H
#define MAIN_FUNCTION_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct JsonBundle {
  StaticJsonDocument<JSON_MSG_BUFFER> body;
};

void handle_autodiscovery();

bool enqueueJsonObject(const StaticJsonDocument<JSON_MSG_BUFFER>& jsonDoc);

template <typename T>
void Config_update(JsonObject& data, const char* key, T& var);

bool cmpToMainTopic(const char* topicOri, const char* toAdd);

void setupTLS(int index = CNT_DEFAULT_INDEX);

const char* getGatewayName();

#endif // MAIN_FUNCTION_H
