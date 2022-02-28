#pragma once

#ifndef Constant_H
#define Constant_H
/**
 * In this file are stored some definition which are platforms depends 
 * but which do not change the behaviour of the OMG.
 */

#if defined(ESP32)
#  define JSON_MSG_BUFFER    768
#  define SIGNAL_SIZE_UL_ULL uint64_t
#  define STRTO_UL_ULL       strtoull
#elif defined(ESP8266)
#  define JSON_MSG_BUFFER    512 // Json message max buffer size, don't put 768 or higher it is causing unexpected behaviour on ESP8266
#  define SIGNAL_SIZE_UL_ULL uint64_t
#  define STRTO_UL_ULL       strtoull
#elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
#  define JSON_MSG_BUFFER    512 // Json message max buffer size, don't put 1024 or higher
#  define SIGNAL_SIZE_UL_ULL uint64_t
#  define STRTO_UL_ULL       strtoul
#else // boards with smaller memory
#  define JSON_MSG_BUFFER    64 // Json message max buffer size, don't put 1024 or higher
#  define SIGNAL_SIZE_UL_ULL uint32_t
#  define STRTO_UL_ULL       strtoul
#endif

//MQTT Parameters definition
#if defined(ESP8266) || defined(ESP32) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
#  define parameters_size     60
#  define mqtt_topic_max_size 100
#  ifdef MQTT_HTTPS_FW_UPDATE
#    define mqtt_max_packet_size 2048
#  else
#    define mqtt_max_packet_size 1024
#  endif
#else
#  define parameters_size      30
#  define mqtt_topic_max_size  50
#  define mqtt_max_packet_size 128
#endif

#endif