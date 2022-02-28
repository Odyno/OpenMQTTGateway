
#include "MQTTPublisher.hpp"

#include <ArduinoLog.h>

MQTTPublisher::MQTTPublisher(PubSubClient client) : client(client){};
MQTTPublisher::~MQTTPublisher(){};

void MQTTPublisher::publish(const char* topic, JsonObject& data, boolean retain) {
  String buffer = "";
  serializeJson(data, buffer);
  pubMQTT(topic, buffer.c_str(), retain);
}
void MQTTPublisher::publish(const char* topic, std::string buffer, boolean retain) {
  pubMQTT(topic, buffer.c_str(), retain);
}
void MQTTPublisher::publish(const char* topic, const char* buffer, boolean retain) {
  pubMQTT(topic, buffer, retain);
}

/**
 * @brief Very Low level MQTT functions with retain Flag
 * 
 * @param topic the topic 
 * @param payload the payload
 * @param retainFlag  true if retain the retain Flag
 */
void MQTTPublisher::pubMQTT(const char* topic, const char* payload, bool retainFlag) {
  if (client.connected()) {
    Log.trace(F("[ OMG->MQTT ] topic: %s msg: %s " CR), topic, payload);
#if AWS_IOT
    client.publish(topic, payload); // AWS IOT doesn't support retain flag for the moment
#else
    client.publish(topic, payload, retainFlag);
#endif
  } else {
    Log.warning(F("Client not connected, aborting thes publication" CR));
  }
}
