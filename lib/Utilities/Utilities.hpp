#pragma once

#ifndef OMG_UTILITIES_H
#  define OMG_UTILITIES_H

#  include <Arduino.h>
#  include <string.h>

namespace OMG {
/**
 * @brief Get the Mac Address of the board
 * 
 * @return std::string 
 */
std::string getMacAddress();

/**
 * @brief Get a common pattern for Unique Id of devices/entity
 * 
 * @param name 
 * @param sufix 
 * @return 
 */
std::string getOMGUniqueId(const char* name, const char* sufix);

/**
 * @brief Get the Unique Id object
 * 
 * @param name 
 * @param sufix 
 * @return const char* 
 */
std::string getUniqueId(const char* mac, const char* name, const char* sufix);

} // namespace OMG
#endif