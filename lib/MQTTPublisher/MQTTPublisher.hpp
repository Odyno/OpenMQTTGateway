#pragma once

#ifndef MQTTPublisher_H
#  define MQTTPublisher_H

#  include <Arduino.h>
#  include <ArduinoJson.h>
#  include <PubSubClient.h>

class MQTTPublisher {
private:
  PubSubClient client;

protected:
  /**
 * @brief Very Low level MQTT functions with retain Flag
 * 
 * @param topic the topic 
 * @param payload the payload
 * @param retainFlag  true if retain the retain Flag
 */
  void pubMQTT(const char* topic, const char* payload, bool retainFlag);

public:
  MQTTPublisher(PubSubClient oneClient);
  ~MQTTPublisher();
  /**
     * @brief Publish the payload on the topic with a retantion
     * 
     * @param topic  The topic where to publish
     * @param data   The Json Object that rapresent the message
     * @param retain true if you what a retain 
     */
  void publish(const char* topic, JsonObject& data, boolean retain);
  void publish(const char* topic, std::string buffer, boolean retain);
  void publish(const char* topic, const char* buffer, boolean retain);
};

#endif
