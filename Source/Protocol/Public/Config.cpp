#include "Config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace MCP
{
    int Config::LoadFromFile(const std::string& configPath)
    {
        std::ifstream file(configPath);
        if (!file.is_open())
        {
            return ERRNO_INTERNAL_ERROR;
        }

        std::string line;
        std::string currentSection;
        
        while (std::getline(file, line))
        {
            // Remove whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == ';')
                continue;

            // Check for section header [section]
            if (line[0] == '[' && line.back() == ']')
            {
                currentSection = line.substr(1, line.length() - 2);
                continue;
            }

            // Parse key=value pairs
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos)
            {
                std::string key = line.substr(0, equalPos);
                std::string value = line.substr(equalPos + 1);

                // Remove whitespace from key and value
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                m_config[currentSection][key] = value;
            }
        }

        return ERRNO_OK;
    }

    std::string Config::GetString(const std::string& section, const std::string& key, const std::string& defaultValue) const
    {
        auto sectionIt = m_config.find(section);
        if (sectionIt != m_config.end())
        {
            auto keyIt = sectionIt->second.find(key);
            if (keyIt != sectionIt->second.end())
            {
                return keyIt->second;
            }
        }
        return defaultValue;
    }

    int Config::GetInt(const std::string& section, const std::string& key, int defaultValue) const
    {
        std::string value = GetString(section, key);
        if (value.empty())
            return defaultValue;

        try
        {
            return std::stoi(value);
        }
        catch (...)
        {
            return defaultValue;
        }
    }

    bool Config::GetBool(const std::string& section, const std::string& key, bool defaultValue) const
    {
        std::string value = GetString(section, key);
        if (value.empty())
            return defaultValue;

        // Convert to lowercase for comparison
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        
        return (value == "1" || value == "true" || value == "yes" || value == "on");
    }
}
