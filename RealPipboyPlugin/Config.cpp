
#include "Utilities.h"
#include "nvse/Utilities.h"

const std::string getConfigPath()
{
	static std::string configPath;
	if (configPath.empty()) {
		std::string falloutPath = GetFalloutDirectory();
		if (!falloutPath.empty()) {
			configPath = falloutPath + "Data\\NVSE\\real_pipboy.ini";
			_MESSAGE("config path = %s", configPath.c_str());
		}
	}
	return configPath;
}

std::string GetConfigOption(const char *section, const char *key)
{
	std::string	result;

	const std::string & configPath = getConfigPath();
	if (!configPath.empty())
	{
		char	resultBuf[256];
		resultBuf[0] = 0;

		UInt32	resultLen = GetPrivateProfileString(section, key, NULL, resultBuf, 255, configPath.c_str());

		result = resultBuf;
	}

	return result;
}

bool GetConfigOption_UInt32(const char *section, const char *key, UInt32 *dataOut)
{
	std::string	data = GetConfigOption(section, key);
	if (data.empty())
		return false;
	errno = 0;
	*dataOut = std::strtoul(data.c_str(), NULL, 0);
	return errno != ERANGE;
}

