/*  
  Theengs OpenMQTTGateway - We Unite Sensors in One Open-Source Interface

   Act as a gateway between your 433mhz, infrared IR, BLE, LoRa signal and one interface like an MQTT broker 
   Send and receiving command by MQTT
 
  This gateway enables to:
 - receive MQTT data from a topic and send RF 433Mhz signal corresponding to the received MQTT data
 - publish MQTT data to a different topic related to received 433Mhz signal

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
#ifndef ZGATEWAYRF_H
#define ZGATEWAYRF_H

#ifdef ZgatewayRF
#  define ARDUINOJSON_USE_LONG_LONG     1
#  define ARDUINOJSON_ENABLE_STD_STRING 1
#  include <Arduino.h>
#  include <ArduinoJson.h>
#  include <RCSwitch.h> // library for controling Radio frequency switch

#  include "../../User_config.h"
#  include "../AbstractGatewayRF.h" // include the header file for AbstractGatewayRF

class ZGatewayRF : public AbstractGatewayRF {
public:
  ZGatewayRF();

  bool disableReceive();

  bool enableReceive();

  /**
     * @brief Processes received RF signals and converts them to JSON format for further handling.
     * 
     * This function checks if an RF signal is available, extracts relevant data from the signal,
     * and stores it in a JSON object. It also handles duplicate signal detection and optionally
     * publishes the signal data for MQTT discovery and repetition.
     * 
     * @note This function is designed to work with both ESP32 and ESP8266 platforms.
     * 
     * @details The function performs the following steps:
     * - Checks if an RF signal is available.
     * - Logs the reception of the RF signal.
     * - Extracts the value, protocol, length, delay, tristate, and binary representation of the signal.
     * - For ESP32 and ESP8266, extracts the raw data of the signal.
     * - If the ZradioCC1101 is defined, includes the frequency in the JSON object.
     * - Resets the availability status of the RF signal.
     * - Checks for duplicate signals and processes the signal if it is not a duplicate.
     * - Optionally publishes the signal data for MQTT discovery and repetition.
     * 
     * @param None
     * @return void
     */
  void RFtoX();

#  if simpleReceiving // FALSE MEAN you don't want to use old way reception analysis
  /**
   * @brief Transmits RF signals based on the provided MQTT topic and data.
   *
   * This function processes the MQTT topic and data to determine the RF protocol,
   * pulse length, and number of bits to use for transmission. It then transmits
   * the RF signal using the specified parameters. If no specific parameters are
   * provided, default values are used.
   *
   * @param topicOri The original MQTT topic string.
   * @param datacallback The data to be transmitted, provided as a string.
   *
   * The function performs the following steps:
   * 1. Disables the current RF receiver and enables the transmitter if ZradioCC1101 is defined.
   * 2. Converts the data string to a 64-bit unsigned integer.
   * 3. Analyzes the topic string to extract RF protocol, pulse length, and bit count.
   * 4. Transmits the RF signal using the extracted or default parameters.
   * 5. Publishes an acknowledgment to the GTWRF topic.
   * 6. Re-enables the RF receiver and disables the transmitter if ZradioCC1101 is defined.
   */
  void XtoRF(const char* topicOri, const char* datacallback);
#  endif

#  if jsonReceiving // FALSE MEAN you don't want to use Json  reception analysis
  /**
   * @brief Handles the conversion of MQTT messages to RF signals.
   *
   * This function decodes a JSON object received via MQTT and transmits the corresponding RF signal.
   * It supports different RF protocols and configurations.
   *
   * @param topicOri The original MQTT topic.
   * @param RFdata The JSON object containing RF data to be transmitted.
   *
   * The JSON object should contain the following fields:
   * - "value": The RF signal value to be transmitted (required).
   * - "protocol": The RF protocol to be used (optional, default is 1).
   * - "delay": The pulse length in microseconds (optional, default is 350).
   * - "length": The number of bits in the RF signal (optional, default is 24).
   * - "repeat": The number of times the RF signal should be repeated (optional, default is RF_EMITTER_REPEAT).
   * - "txpower": The transmission power for CC1101 (optional, default is RF_CC1101_TXPOWER).
   * - "frequency": The transmission frequency for CC1101 (optional, default is RFConfig.frequency).
   *
   * The function logs the transmission details and acknowledges the sending by publishing the value to an acknowledgement topic.
   * It also restores the default repeat transmit value after sending the signal.
   */
  void XtoRF(const char* topicOri, JsonObject& RFdata);
#  endif

private:
  RCSwitch mySwitch;
  /**
   * @brief Disables the RF receiver.
   *
   * This function disables the RF receiver by calling the disableReceive method
   * on the mySwitch object. It also logs a trace message indicating that the RF
   * receiver has been disabled, along with the GPIO pin number used for the RF
   * receiver.
   *
   * @note THIS SEEMS LIKE A DEAD CODE. THE FUNCTION IS NOT CALLED ANYWHERE.
   */
  void disableRFReceive();

  /**
   * @brief Enables the RF receiver and optionally the RF transmitter.
   *
   * This function initializes the RF receiver on the specified GPIO pin and, if not disabled, 
   * initializes the RF transmitter on the specified GPIO pin. It also sets the RF frequency 
   * and logs the configuration details.
   *
   * @param rfFrequency The frequency for the RF communication in MHz. Default is RFConfig.frequency.
   * @param rfReceiverGPIO The GPIO pin number for the RF receiver. Default is RF_RECEIVER_GPIO.
   * @param rfEmitterGPIO The GPIO pin number for the RF transmitter. Default is RF_EMITTER_GPIO.
   *
   * @note If RF_DISABLE_TRANSMIT is defined, the RF transmitter will be disabled.
   */
  void enableRFReceive();

  static const char* bin2tristate(const char* bin);

  static char* dec2binWzerofill(unsigned long Dec, unsigned int bitLength);

#  if defined(ZmqttDiscovery) && defined(RF_on_HAS_as_DeviceTrigger)
  void announceGatewayTriggerTypeToHASS(uint64_t MQTTvalue);
#  endif
};

#endif // ZgatewayRF

#endif // ZGATEWAYRF_H
