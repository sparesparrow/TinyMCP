#pragma once
// Simple configuration loader for TinyMCP
// Uses INI file format for easy configuration

#include <string>
#include <unordered_map>

namespace MCP
{
    class Config
    {
    public:
        static Config& GetInstance()
        {
            static Config instance;
            return instance;
        }

        // Load configuration from file
        int LoadFromFile(const std::string& configPath);

        // Get configuration values
        std::string GetString(const std::string& section, const std::string& key, const std::string& defaultValue = "") const;
        int GetInt(const std::string& section, const std::string& key, int defaultValue = 0) const;
        bool GetBool(const std::string& section, const std::string& key, bool defaultValue = false) const;

        // Server configuration
        int GetPort() const { return GetInt("server", "port", 6666); }
        std::string GetHost() const { return GetString("server", "host", "localhost"); }

        // Security configuration
        bool IsHttpsEnabled() const { return GetBool("security", "enable_https", false); }
        std::string GetCertFile() const { return GetString("security", "cert_file", "certs/server.crt"); }
        std::string GetKeyFile() const { return GetString("security", "key_file", "certs/server.key"); }

        // Auth configuration
        bool IsAuthEnabled() const { return GetBool("auth", "enable_auth", false); }
        std::string GetApiKey() const { return GetString("auth", "api_key", ""); }

    private:
        Config() = default;
        ~Config() = default;
        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;

        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_config;
    };
}
