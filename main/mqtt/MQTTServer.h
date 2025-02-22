#ifndef MQTTServer_H
#define MQTTServer_H

#include <Arduino.h>
#include <PicoMQTT.h>

#include "../main_function.h"

class MQTTServer : public PicoMQTT::Server {
public:
  size_t get_client_count() const {
    return clients.size();
  }

  bool connected() const {
    return !clients.empty();
  }

protected:
  virtual void on_subscribe(const char* client_id, const char* topic) override {
    // Whenever a client subscribes successfully to some topic, see if this is likely a subscription to a
    // autodiscovery topic.  If it is, fire handle_autodiscovery().
    const String pattern(topic);
    const bool is_autodiscovery_subscription = (pattern == "#") || (pattern.startsWith(String(discovery_prefix) + "/"));
    if (is_autodiscovery_subscription)
      handle_autodiscovery();
  }
};

#endif //MQTTServer_H