#pragma once

// Conditional include for ArduinoLog with graceful fallback
#ifdef __has_include
#  if __has_include(<ArduinoLog.h>)
#    define HAS_ARDUINO_LOG 1
#  else
#    define HAS_ARDUINO_LOG 0
#  endif
#else
// Fallback for older compilers
#  if defined(ARDUINO) && !defined(UNIT_TEST)
#    define HAS_ARDUINO_LOG 1
#  else
#    define HAS_ARDUINO_LOG 0
#  endif
#endif

// Include ArduinoLog only if available
#if HAS_ARDUINO_LOG
#  include <ArduinoLog.h>
#endif

/*-------------------DEFINE LOG LEVEL----------------------*/
#ifndef LOG_LEVEL
#  if HAS_ARDUINO_LOG
#    define LOG_LEVEL LOG_LEVEL_NOTICE
#  else
#    define LOG_LEVEL -100 // Disabled when ArduinoLog not available
#  endif
#endif

/*-------------------SIMPLIFIED LOGGING MACROS----------------------*/
// ArduinoLog levels: SILENT=0, FATAL=1, ERROR=2, WARNING=3, NOTICE=4, TRACE=5, VERBOSE=6
#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
#  define THEENGS_LOG_VERBOSE(...) Log.verbose(__VA_ARGS__)
#else
#  define THEENGS_LOG_VERBOSE(...) ((void)0)
#endif
#if LOG_LEVEL >= LOG_LEVEL_TRACE
#  define THEENGS_LOG_TRACE(...) Log.trace(__VA_ARGS__)
#else
#  define THEENGS_LOG_TRACE(...) ((void)0)
#endif
#if LOG_LEVEL >= LOG_LEVEL_NOTICE
#  define THEENGS_LOG_NOTICE(...) Log.notice(__VA_ARGS__)
#else
#  define THEENGS_LOG_NOTICE(...) ((void)0)
#endif
#if LOG_LEVEL >= LOG_LEVEL_WARNING
#  define THEENGS_LOG_WARNING(...) Log.warning(__VA_ARGS__)
#else
#  define THEENGS_LOG_WARNING(...) ((void)0)
#endif
#if LOG_LEVEL >= LOG_LEVEL_ERROR
#  define THEENGS_LOG_ERROR(...) Log.error(__VA_ARGS__)
#else
#  define THEENGS_LOG_ERROR(...) ((void)0)
#endif
#if LOG_LEVEL >= LOG_LEVEL_FATAL
#  define LOG_FATAL(...) Log.fatal(__VA_ARGS__)
#else
#  define LOG_FATAL(...) ((void)0)
#endif