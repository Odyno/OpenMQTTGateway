/*  
  Theengs OpenMQTTGateway - We Unite Sensors in One Open-Source Interface

   Act as a wifi or ethernet gateway between your BLE/433mhz/infrared IR signal and an MQTT broker 
   Send and receiving command by MQTT
  
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

#if defined(ZgatewayRF) || defined(ZgatewayPilight) || defined(ZgatewayRTL_433) || defined(ZgatewayRF2) || defined(ZactuatorSomfy)

#  include "ZCommonRF.h"

#  include <ArduinoJson.h>
#  include <ArduinoLog.h>

#  include "../User_config.h"
#  include "../config_RF.h"
#  include "../main_function.h"
#  include "AbstractGatewayRF.h"
#  ifdef ZradioCC1101
#    include <ELECHOUSE_CC1101_SRC_DRV.h>
#  endif

ZCommonRF::ZCommonRF() : currentReceiver(ACTIVE_NONE) {};

#  ifdef ZradioCC1101 //receiving with CC1101
/**
 * @brief Initializes the CC1101 RF module.
 *
 * This function attempts to initialize the CC1101 RF module by setting the SPI pins
 * and checking the connection status. It will make up to 10 attempts to establish
 * a connection, with a truncated exponential backoff delay between attempts.
 *
 * The function performs the following steps:
 * 1. Sets the SPI pins for the CC1101 module if they are defined.
 * 2. Attempts to get the CC1101 module status.
 * 3. If the connection is successful, logs a success message, initializes the module,
 *    and sets the receive frequency.
 * 4. If the connection fails, logs an error message and waits for a delay before retrying.
 * 5. The delay between attempts starts at 16 milliseconds and doubles with each attempt,
 *    up to a maximum of 500 milliseconds.
 */
void ZCommonRF::initCC1101() {
  // Loop on getCC1101() until it returns true and break after 10 attempts
  int delayMS = 16;
  int delayMaxMS = 500;
  for (int i = 0; i < 10; i++) {
#    if defined(RF_MODULE_SCK) && defined(RF_MODULE_MISO) && \
        defined(RF_MODULE_MOSI) && defined(RF_MODULE_CS)
    ELECHOUSE_cc1101.setSpiPin(RF_MODULE_SCK, RF_MODULE_MISO, RF_MODULE_MOSI, RF_MODULE_CS);
#    endif
    if (ELECHOUSE_cc1101.getCC1101()) {
      Log.notice(F("C1101 spi Connection OK" CR));
      ELECHOUSE_cc1101.Init();
      ELECHOUSE_cc1101.SetRx(RFConfig.frequency);
      break;
    } else {
      Log.error(F("C1101 spi Connection Error" CR));
      delay(delayMS);
    }
    // truncated exponential backoff
    delayMS = delayMS * 2;
    if (delayMS > delayMaxMS) delayMS = delayMaxMS;
  }
}
#  endif

/**
 * @brief Initializes and loads the common RF configuration.
 * 
 * This function calls the initialization function for the RF configuration
 * and then loads the configuration settings.
 */
void ZCommonRF::setupCommonRF() {
  RFConfig_init();
  RFConfig_load();
}

/**
 * @brief Checks if the given frequency is within the valid ranges for CC1101.
 *
 * This function verifies if the provided frequency in MHz falls within the 
 * valid frequency ranges for the CC1101 transceiver. The valid ranges are:
 * - 300 MHz to 348 MHz
 * - 387 MHz to 464 MHz
 * - 779 MHz to 928 MHz
 *
 * @param mhz The frequency in MHz to be checked.
 * @return true if the frequency is within the valid ranges, false otherwise.
 */
bool ZCommonRF::validFrequency(float mhz) {
  //  CC1101 valid frequencies 300-348 MHZ, 387-464MHZ and 779-928MHZ.
  if (mhz >= 300 && mhz <= 348)
    return true;
  if (mhz >= 387 && mhz <= 464)
    return true;
  if (mhz >= 779 && mhz <= 928)
    return true;
  return false;
}

#  if !defined(ZgatewayRFM69) && !defined(ZactuatorSomfy)
// Check if a receiver is available
/**
 * @brief Checks if the given receiver is valid based on the enabled gateways.
 *
 * This function verifies if the provided receiver identifier corresponds to an
 * active and enabled gateway. The function checks against multiple possible
 * gateways, which are conditionally compiled based on the defined preprocessor
 * directives.
 *
 * @param receiver The identifier of the receiver to validate.
 * @return true if the receiver is valid and corresponds to an active gateway.
 * @return false if the receiver is not valid or not available.
 */
bool ZCommonRF::validReceiver(int receiver) {
  switch (receiver) {
#    ifdef ZgatewayPilight
    case ACTIVE_PILIGHT:
      return true;
#    endif
#    ifdef ZgatewayRF
    case ACTIVE_RF:
      return true;
#    endif
#    ifdef ZgatewayRTL_433
    case ACTIVE_RTL:
      return true;
#    endif
#    ifdef ZgatewayRF2
    case ACTIVE_RF2:
      return true;
#    endif
    default:
      Log.error(F("ERROR: stored receiver %d not available" CR), receiver);
  }
  return false;
}
#  endif

/**
 * @brief Disables the currently active receiver based on the value of `currentReceiver`.
 *
 * This function logs the current receiver being disabled and then disables the receiver
 * by calling the appropriate function based on the value of `currentReceiver`.
 *
 * Supported receivers:
 * - ACTIVE_NONE: No receiver is active, nothing is done.
 * - ACTIVE_PILIGHT: Calls `disablePilightReceive()` if `ZgatewayPilight` is defined.
 * - ACTIVE_RF: Calls `disableRFReceive()` if `ZgatewayRF` is defined.
 * - ACTIVE_RTL: Calls `disableRTLreceive()` if `ZgatewayRTL_433` is defined.
 * - ACTIVE_RF2: Calls `disableRF2Receive()` if `ZgatewayRF2` is defined.
 *
 * If `currentReceiver` does not match any of the supported receivers, an error is logged.
 */
void ZCommonRF::disableCurrentReceiver() {
  Log.trace(F("disableCurrentReceiver: %d" CR), currentReceiver);
  try {
    if (gateways.at(currentReceiver)->disableReceive()) {
    } else {
      Log.error(F("RF ERROR: no receiver disabled" CR));
    }
  } catch (const std::out_of_range& e) {
    Log.error(F("RF ERROR: unsupported receiver %d" CR), RFConfig.activeReceiver);
    currentReceiver = ACTIVE_RECERROR;
  } catch (const std::exception& e) {
    Log.error(F("Exception caught: %s" CR), e.what());
    currentReceiver = ACTIVE_RECERROR;
  } catch (...) {
    Log.error(F("Unknown exception caught" CR));
    currentReceiver = ACTIVE_RECERROR;
  }

  /*
  switch (currentReceiver) {
    case ACTIVE_NONE:
      break;
#  ifdef ZgatewayPilight
    case ACTIVE_PILIGHT:
      disablePilightReceive();
      break;
#  endif
#  ifdef ZgatewayRF
    case ACTIVE_RF:
      disableRFReceive();
      break;
#  endif
#  ifdef ZgatewayRTL_433
    case ACTIVE_RTL:
      disableRTLreceive();
      break;
#  endif
#  ifdef ZgatewayRF2
    case ACTIVE_RF2:
      disableRF2Receive();
      break;
#  endif
    default:
      Log.error(F("ERROR: unsupported receiver %d" CR), RFConfig.activeReceiver);
  }
  */
}

/**
 * @brief Enables the active receiver based on the configuration.
 *
 * This function checks the active receiver configuration and initializes
 * the corresponding receiver. It supports multiple receiver types, including
 * Pilight, RF, RTL_433, and RF2. If no valid receiver is selected, it logs an error.
 *
 * The function performs the following steps:
 * - Logs the active receiver configuration.
 * - Switches based on the active receiver type:
 *   - If ZgatewayPilight is defined and the active receiver is ACTIVE_PILIGHT,
 *     it initializes the CC1101 module and enables Pilight receive mode.
 *   - If ZgatewayRF is defined and the active receiver is ACTIVE_RF,
 *     it initializes the CC1101 module and enables RF receive mode.
 *   - If ZgatewayRTL_433 is defined and the active receiver is ACTIVE_RTL,
 *     it initializes the CC1101 module and enables RTL_433 receive mode.
 *   - If ZgatewayRF2 is defined and the active receiver is ACTIVE_RF2,
 *     it initializes the CC1101 module and enables RF2 receive mode.
 * - If the active receiver is ACTIVE_RECERROR, it logs an error indicating
 *   that no receiver is selected.
 * - If the active receiver is not supported, it logs an error with the
 *   unsupported receiver type.
 */
void ZCommonRF::enableActiveReceiver() {
  Log.trace(F("enableActiveReceiver: %d" CR), RFConfig.activeReceiver);

#  ifdef ZradioCC1101 //receiving with CC1101
  initCC1101();
#  endif

  try {
    if (gateways.at(RFConfig.activeReceiver)->enableReceive()) {
      currentReceiver = RFConfig.activeReceiver;
    } else {
      Log.error(F("RF ERROR: no receiver enabled" CR));
      currentReceiver = ACTIVE_RECERROR;
    }
  } catch (const std::out_of_range& e) {
    Log.error(F("RF ERROR: unsupported receiver %d" CR), RFConfig.activeReceiver);
    currentReceiver = ACTIVE_RECERROR;
  } catch (const std::exception& e) {
    Log.error(F("Exception caught: %s" CR), e.what());
    currentReceiver = ACTIVE_RECERROR;
  } catch (...) {
    Log.error(F("Unknown exception caught" CR));
    currentReceiver = ACTIVE_RECERROR;
  }

  /*
  switch (RFConfig.activeReceiver) {
#  ifdef ZgatewayPilight
    case ACTIVE_PILIGHT:
      initCC1101();
      enablePilightReceive();
      currentReceiver = ACTIVE_PILIGHT;
      break;
#  endif
#  ifdef ZgatewayRF
    case ACTIVE_RF:
      initCC1101();
      enableRFReceive();
      currentReceiver = ACTIVE_RF;
      break;
#  endif
#  ifdef ZgatewayRTL_433
    case ACTIVE_RTL:
      initCC1101();
      enableRTLreceive();
      currentReceiver = ACTIVE_RTL;
      break;
#  endif
#  ifdef ZgatewayRF2
    case ACTIVE_RF2:
      initCC1101();
      enableRF2Receive();
      currentReceiver = ACTIVE_RF2;
      break;
#  endif
    case ACTIVE_RECERROR:
      Log.error(F("ERROR: no receiver selected" CR));
      break;
    default:
      Log.error(F("ERROR: unsupported receiver %d" CR), RFConfig.activeReceiver);
  }
  */
}

/**
 * @brief Publishes the state of the RTL_433 receiver.
 *
 * This function collects various metrics related to the RTL_433 receiver and 
 * serializes them into a JSON object. The JSON object is then enqueued for 
 * further processing and also returned as a string.
 *
 * @return A JSON string representing the state of the RTL_433 receiver.
 *
 * The JSON object contains the following fields:
 * - "active": Indicates if the receiver is active.
 * - "frequency": The frequency of the receiver (if ZradioCC1101 or ZradioSX127x is defined).
 * - "rssithreshold": The RSSI threshold (if ZgatewayRTL_433 is defined and the active receiver is RTL).
 * - "rssi": The current RSSI value (if ZgatewayRTL_433 is defined and the active receiver is RTL).
 * - "avgrssi": The average RSSI value (if ZgatewayRTL_433 is defined and the active receiver is RTL).
 * - "count": The message count (if ZgatewayRTL_433 is defined and the active receiver is RTL).
 * - "rtl433_stack": The high water mark of the rtl_433_Decoder stack (if ZgatewayRTL_433 is defined and the active receiver is RTL).
 * - "ookthreshold": The OOK threshold (if ZradioSX127x is defined and the active receiver is RTL).
 * - "origin": The origin of the message.
 */
String ZCommonRF::stateRFMeasures() {
  //Publish RTL_433 state
  StaticJsonDocument<JSON_MSG_BUFFER> jsonBuffer;
  JsonObject RFdata = jsonBuffer.to<JsonObject>();
  RFdata["active"] = RFConfig.activeReceiver;
#  if defined(ZradioCC1101) || defined(ZradioSX127x)
  RFdata["frequency"] = RFConfig.frequency;
  if (RFConfig.activeReceiver == ACTIVE_RTL) {
#    ifdef ZgatewayRTL_433
    RFdata["rssithreshold"] = (int)getRTLrssiThreshold();
    RFdata["rssi"] = (int)getRTLCurrentRSSI();
    RFdata["avgrssi"] = (int)getRTLAverageRSSI();
    RFdata["count"] = (int)getRTLMessageCount();
    // Capture high water mark of rtl_433_Decoder stack since it can run out and trigger reboot
    extern TaskHandle_t rtl_433_DecoderHandle;
    RFdata["rtl433_stack"] = (int)uxTaskGetStackHighWaterMark(rtl_433_DecoderHandle);
#    endif
#    ifdef ZradioSX127x
    RFdata["ookthreshold"] = (int)getOOKThresh();
#    endif
  }
#  endif
  RFdata["origin"] = subjectcommonRFtoMQTT;
  enqueueJsonObject(RFdata);

  String output;
  serializeJson(RFdata, output);
  return output;
}

/**
 * @brief Configures RF settings from a JSON object.
 *
 * This function updates the RF configuration based on the provided JSON object.
 * It handles various RF parameters such as frequency, active status, RSSI threshold,
 * and OOK threshold. Additionally, it can erase or save the configuration to non-volatile storage.
 *
 * @param RFdata A reference to a JsonObject containing the RF configuration data.
 *
 * The JSON object can contain the following keys:
 * - "frequency": The frequency to set for RF reception.
 * - "active": The active status of the RF receiver.
 * - "rssithreshold" (only if ZgatewayRTL_433 is defined): The RSSI threshold for RTL_433.
 * - "ookthreshold" (only if ZgatewayRTL_433 and RF_SX1276 or RF_SX1278 are defined): The OOK threshold for RTL_433.
 * - "status" (only if ZgatewayRTL_433 is defined): Requests the status of the RF receiver.
 * - "erase" (only if ESP32 is defined): A boolean indicating whether to erase the RF configuration from NVS.
 * - "save" (only if ESP32 is defined): A boolean indicating whether to save the RF configuration to NVS.
 *
 * The function logs the updated configuration and any errors encountered during the process.
 * It also handles enabling and disabling the current and active RF receivers.
 */
void ZCommonRF::RFConfig_fromJson(JsonObject& RFdata) {
  bool success = false;
  if (RFdata.containsKey("frequency") && validFrequency(RFdata["frequency"])) {
    Config_update(RFdata, "frequency", RFConfig.frequency);
    Log.notice(F("RF Receive mhz: %F" CR), RFConfig.frequency);
    success = true;
  }
  if (RFdata.containsKey("active")) {
    Log.notice(F("RF receiver active: %d" CR), RFConfig.activeReceiver);
    Config_update(RFdata, "active", RFConfig.activeReceiver);
    success = true;
  }
#  ifdef ZgatewayRTL_433
  if (RFdata.containsKey("rssithreshold")) {
    Log.notice(F("RTL_433 RSSI Threshold : %d " CR), RFConfig.rssiThreshold);
    Config_update(RFdata, "rssithreshold", RFConfig.rssiThreshold);
    rtl_433.setRSSIThreshold(RFConfig.rssiThreshold);
    success = true;
  }
#    if defined(RF_SX1276) || defined(RF_SX1278)
  if (RFdata.containsKey("ookthreshold")) {
    Config_update(RFdata, "ookthreshold", RFConfig.newOokThreshold);
    Log.notice(F("RTL_433 ookThreshold %d" CR), RFConfig.newOokThreshold);
    rtl_433.setOOKThreshold(RFConfig.newOokThreshold);
    success = true;
  }
#    endif
  if (RFdata.containsKey("status")) {
    Log.notice(F("RF get status:" CR));
    rtl_433.getStatus();
    success = true;
  }
  if (!success) {
    Log.error(F("MQTTtoRF Fail json" CR));
  }
#  endif
  disableCurrentReceiver();
  enableActiveReceiver();
#  ifdef ESP32
  if (RFdata.containsKey("erase") && RFdata["erase"].as<bool>()) {
    // Erase config from NVS (non-volatile storage)
    preferences.begin(Gateway_Short_Name, false);
    if (preferences.isKey("RFConfig")) {
      int result = preferences.remove("RFConfig");
      Log.notice(F("RF config erase result: %d" CR), result);
      preferences.end();
      return; // Erase prevails on save, so skipping save
    } else {
      Log.notice(F("RF config not found" CR));
      preferences.end();
    }
  }
  if (RFdata.containsKey("save") && RFdata["save"].as<bool>()) {
    StaticJsonDocument<JSON_MSG_BUFFER> jsonBuffer;
    JsonObject jo = jsonBuffer.to<JsonObject>();
    jo["frequency"] = RFConfig.frequency;
    jo["active"] = RFConfig.activeReceiver;
// Don't save those for now, need to be tested
#    ifdef ZgatewayRTL_433
//jo["rssithreshold"] = RFConfig.rssiThreshold;
//jo["ookthreshold"] = RFConfig.newOokThreshold;
#    endif
    // Save config into NVS (non-volatile storage)
    String conf = "";
    serializeJson(jsonBuffer, conf);
    preferences.begin(Gateway_Short_Name, false);
    int result = preferences.putString("RFConfig", conf);
    preferences.end();
    Log.notice(F("RF Config_save: %s, result: %d" CR), conf.c_str(), result);
  }
#  endif
}

/**
 * @brief Initializes the RF configuration with default values.
 *
 * This function sets the frequency, active receiver, RSSI threshold, 
 * and new OOK threshold for the RF configuration.
 *
 * The following default values are assigned:
 * - RFConfig.frequency: RF_FREQUENCY
 * - RFConfig.activeReceiver: ACTIVE_RF
 * - RFConfig.rssiThreshold: 0
 * - RFConfig.newOokThreshold: 0
 */
void ZCommonRF::RFConfig_init() {
  RFConfig.frequency = RF_FREQUENCY;
  RFConfig.activeReceiver = ACTIVE_RF; // TODO: default active receiver, shuld be none and set by the user
  RFConfig.rssiThreshold = 0;
  RFConfig.newOokThreshold = 0;
}

/**
 * @brief Loads the RF configuration from persistent storage.
 * 
 * This function attempts to load the RF configuration stored in the ESP32's 
 * preferences. If the configuration is found and successfully deserialized, 
 * it is applied using the RFConfig_fromJson function. If the configuration 
 * is not found or deserialization fails, appropriate log messages are generated.
 * 
 * For non-ESP32 platforms, the function simply enables the active receiver.
 * 
 * @note This function is only applicable for ESP32 platforms. For other platforms,
 *       it directly enables the active receiver.
 */
void ZCommonRF::RFConfig_load() {
#  ifdef ESP32
  StaticJsonDocument<JSON_MSG_BUFFER> jsonBuffer;
  preferences.begin(Gateway_Short_Name, true);
  if (preferences.isKey("RFConfig")) {
    auto error = deserializeJson(jsonBuffer, preferences.getString("RFConfig", "{}"));
    preferences.end();
    if (error) {
      Log.error(F("RF Config deserialization failed: %s, buffer capacity: %u" CR), error.c_str(), jsonBuffer.capacity());
      return;
    }
    if (jsonBuffer.isNull()) {
      Log.warning(F("RF Config is null" CR));
      return;
    }
    JsonObject jo = jsonBuffer.as<JsonObject>();
    RFConfig_fromJson(jo);
    Log.notice(F("RF Config loaded" CR));
  } else {
    preferences.end();
    Log.notice(F("RF Config not found using default" CR));
    enableActiveReceiver();
  }
#  else
  enableActiveReceiver();
#  endif
}

/**
 * @brief Handles the MQTT to RF configuration set command.
 *
 * This function processes the incoming MQTT message to set the RF configuration.
 * It follows a specific order of operations based on the provided JSON data:
 * 1. Executes `init=true` and `load=true` commands (if both are present, INIT prevails over LOAD).
 * 2. Applies parameters included in the JSON data.
 * 3. Executes `erase=true` and `save=true` commands (if both are present, ERASE prevails over SAVE).
 *
 * @param topicOri The original MQTT topic.
 * @param RFdata The JSON object containing the RF configuration data.
 */
void ZCommonRF::XtoRFset(const char* topicOri, JsonObject& RFdata) {
  if (cmpToMainTopic(topicOri, subjectMQTTtoRFset)) {
    Log.trace(F("MQTTtoRF json set" CR));

    /*
     * Configuration modifications priorities:
     *  First `init=true` and `load=true` commands are executed (if both are present, INIT prevails on LOAD)
     *  Then parameters included in json are taken in account
     *  Finally `erase=true` and `save=true` commands are executed (if both are present, ERASE prevails on SAVE)
     */
    if (RFdata.containsKey("init") && RFdata["init"].as<bool>()) {
      // Restore the default (initial) configuration
      RFConfig_init();
    } else if (RFdata.containsKey("load") && RFdata["load"].as<bool>()) {
      // Load the saved configuration, if not initialised
      RFConfig_load();
    }

    // Load config from json if available
    RFConfig_fromJson(RFdata);
    stateRFMeasures();
  }
}

#  if defined(ZwebUI) && defined(ESP32)

/**
 * Aggiungi un nuovo GatewayRF al sistema, permettendo di utilizzarlo per la comunicazione RF.
 * inserisci questo gatewayRF in una lista di gatewayRF di cui solo uno sarà attivo alla volta
 */
void ZCommonRF::addGatewayRF(int gatewayRFId, AbstractGatewayRF* gatewayRF) {
  gatewayRF->setRFHandler(this);
  gateways[gatewayRFId] = gatewayRF;
}

AbstractGatewayRF* ZCommonRF::getGatewayRF(int gatewayRFId) {
  return gateways[gatewayRFId];
}

AbstractGatewayRF* ZCommonRF::getCurrentGatewayRF() {
  return gateways[currentReceiver];
}

#  endif

#endif
