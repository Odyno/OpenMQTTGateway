#ifndef WEBUIHANDLER_H
#define WEBUIHANDLER_H

#if defined(ZwebUI) && defined(ESP32)

#  include <ArduinoJson.h>
#  include <WebServer.h>

#  include <map>

#  include "ZCommonRF.h"

class WebUIHandlerRF {
public:
  WebUIHandlerRF(WebServer& server, ZCommonRF& zCommonRF, JsonArray& modules);
  void handleRF();

private:
  WebServer& iWebServer;
  ZCommonRF& iZCommonRF;
  JsonArray& iModules;

  std::map<int, String> activeReceiverOptions = {
      {0, "Inactive"},
#  if defined(ZgatewayPilight) && !defined(ZradioSX127x)
      {1, "PiLight"},
#  endif
#  if defined(ZgatewayRF) && !defined(ZradioSX127x)
      {2, "RF"},
#  endif
#  ifdef ZgatewayRTL_433
      {3, "RTL_433"},
#  endif
#  if defined(ZgatewayRF2) && !defined(ZradioSX127x)
      {4, "RF2 (restart required)"}
#  endif
  };

  bool isValidReceiver(int receiver);

  String generateActiveReceiverOptions(int activeReceiver);
};

#endif // ZwebUI && ESP32

#endif // WEBUIHANDLER_H