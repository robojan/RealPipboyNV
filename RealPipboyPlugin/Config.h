#pragma once

const std::string getConfigPath();
std::string GetConfigOption(const char *section, const char *key);
bool GetConfigOption_UInt32(const char *section, const char *key, UInt32 *dataOut);