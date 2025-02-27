#include "WebUIHandlerRF.h"

#if defined(ZgatewayRF) || defined(ZgatewayPilight) || defined(ZgatewayRTL_433) || defined(ZgatewayRF2) || defined(ZactuatorSomfy)

#  include <ArduinoLog.h>

#  include "../User_config.h"
#  include "../config_RF.h"
#  include "../config_WebContent.h"
#  include "../config_WebUI.h"

WebUIHandlerRF::WebUIHandlerRF(WebServer& server, ZCommonRF& zCommonRF, JsonArray& modules) : iWebServer(server), iZCommonRF(zCommonRF), iModules(modules) {}

/**
 * @brief /RF - Configure RF Page
 * T: handleRF: uri: /rf, args: 2, method: 1
 * T: handleRF Arg: 0, rf=868.30
 * T: handleRF Arg: 1, oo=0
 * T: handleRF Arg: 2, rs=0
 * T: handleRF Arg: 3, dg=0
 * T: handleRF Arg: 4, ar=0
 * T: handleRF Arg: 4, save=
 */

void WebUIHandlerRF::handleRF() {
  WEBUI_TRACE_LOG(F("handleRF: uri: %s, args: %d, method: %d" CR), iWebServer.uri(), iWebServer.args(), iWebServer.method());
  WEBUI_SECURE
  bool update = false;
  StaticJsonDocument<JSON_MSG_BUFFER> jsonBuffer;
  JsonObject WEBtoRF = jsonBuffer.to<JsonObject>();

  if (iWebServer.args()) {
    for (uint8_t i = 0; i < iWebServer.args(); i++) {
      WEBUI_TRACE_LOG(F("handleRF Arg: %d, %s=%s" CR), i, iWebServer.argName(i).c_str(), iWebServer.arg(i).c_str());
    }
    if (iWebServer.hasArg("save")) {
      if (iWebServer.hasArg("rf")) {
        String freqStr = iWebServer.arg("rf");
        RFConfig.frequency = freqStr.toFloat();
        if (iZCommonRF.validFrequency(RFConfig.frequency)) {
          WEBtoRF["frequency"] = RFConfig.frequency;
          update = true;
        } else {
          Log.warning(F("[WebUI] Invalid Frequency" CR));
        }
      }
      if (iWebServer.hasArg("ar")) {
        int selectedReceiver = iWebServer.arg("ar").toInt();
        if (isValidReceiver(selectedReceiver)) { // Assuming isValidReceiver is a validation function
          RFConfig.activeReceiver = selectedReceiver;
          WEBtoRF["activereceiver"] = RFConfig.activeReceiver;
          update = true;
        } else {
          Log.warning(F("[WebUI] Invalid Active Receiver" CR));
        }
      }
      if (iWebServer.hasArg("oo")) {
        RFConfig.newOokThreshold = iWebServer.arg("oo").toInt();
        WEBtoRF["ookthreshold"] = RFConfig.newOokThreshold;
        update = true;
      }
      if (iWebServer.hasArg("rs")) {
        RFConfig.rssiThreshold = iWebServer.arg("rs").toInt();
        WEBtoRF["rssithreshold"] = RFConfig.rssiThreshold;
        update = true;
      }
      if (update) {
        Log.notice(F("[WebUI] Save data" CR));
        WEBtoRF["save"] = true;
        iZCommonRF.RFConfig_fromJson(WEBtoRF);
        iZCommonRF.stateRFMeasures();
        Log.trace(F("[WebUI] RFConfig end" CR));
      }
    }
  }

  String activeReceiverHtml = generateActiveReceiverOptions(RFConfig.activeReceiver);

  char jsonChar[100];
  serializeJson(iModules, jsonChar, measureJson(iModules) + 1);
  char buffer[WEB_TEMPLATE_BUFFER_MAX_SIZE];

  snprintf(buffer, WEB_TEMPLATE_BUFFER_MAX_SIZE, header_html, (String(gateway_name) + " - Configure RF").c_str());
  String response = String(buffer);
  response += String(script);
  response += String(style);

  snprintf(buffer, WEB_TEMPLATE_BUFFER_MAX_SIZE, config_rf_body, jsonChar, gateway_name, RFConfig.frequency, activeReceiverHtml.c_str());
  response += String(buffer);
  snprintf(buffer, WEB_TEMPLATE_BUFFER_MAX_SIZE, footer, OMG_VERSION);
  response += String(buffer);
  iWebServer.send(200, "text/html", response);
}

bool WebUIHandlerRF::isValidReceiver(int receiverId) {
  // Check if the receiverId exists in the activeReceiverOptions map
  return activeReceiverOptions.find(receiverId) != activeReceiverOptions.end();
}

String WebUIHandlerRF::generateActiveReceiverOptions(int currentSelection) {
  String optionsHtml = "";
  for (const auto& option : activeReceiverOptions) {
    optionsHtml += "<option value='" + String(option.first) + "'";
    if (currentSelection == option.first) {
      optionsHtml += " selected";
    }
    optionsHtml += ">" + option.second + "</option>";
  }
  return optionsHtml;
}

void WebUIHandlerRF::setup() {
  iWebServer.on("/rf", HTTP_GET, [&]() {
    handleRF();
  });
}

#endif // defined(ZgatewayRF) || defined(ZgatewayPilight) || defined(ZgatewayRTL_433) || defined(ZgatewayRF2) || defined(ZactuatorSomfy)