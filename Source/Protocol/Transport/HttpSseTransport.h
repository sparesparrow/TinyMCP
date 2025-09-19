#pragma once
// HTTP + Server-Sent Events transport scaffold (stub implementation)
// This class provides an interface-compatible transport that can be
// implemented later with a real HTTP client and SSE stream handling.

#include <string>
#include <memory>
#include "Transport.h"
#include "../Public/Config.h"

namespace MCP
{
    class CHttpSseTransport : public CMCPTransport
    {
    public:
        int Connect() override;
        int Disconnect() override;
        int Read(std::string& strOut) override;
        int Write(const std::string& strIn) override;
        int Error(const std::string& strIn) override;

        void SetEndpoint(const std::string& url);
        void SetAuthorization(const std::string& bearerToken);
        void LoadConfig();

    private:
        std::string m_url;
        std::string m_bearer;
        bool m_connected{ false };
        bool m_httpsEnabled{ false };
        std::string m_certFile;
        std::string m_keyFile;
    };
}


