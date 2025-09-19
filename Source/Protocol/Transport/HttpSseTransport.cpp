#include "HttpSseTransport.h"
#include "../Public/PublicDef.h"
#include "../Public/Config.h"

namespace MCP
{
    int CHttpSseTransport::Connect()
    {
        // Load configuration
        LoadConfig();
        
        // Stub: mark as connected
        // In a real implementation, this would establish HTTP/HTTPS connection
        m_connected = true;
        return ERRNO_OK;
    }

    int CHttpSseTransport::Disconnect()
    {
        m_connected = false;
        return ERRNO_OK;
    }

    int CHttpSseTransport::Read(std::string& strOut)
    {
        // Stub: SSE reading not implemented
        (void)strOut;
        return ERRNO_INTERNAL_INPUT_TERMINATE;
    }

    int CHttpSseTransport::Write(const std::string& strIn)
    {
        // Stub: HTTP POST/stream send not implemented
        (void)strIn;
        return ERRNO_OK;
    }

    int CHttpSseTransport::Error(const std::string& strIn)
    {
        (void)strIn;
        return ERRNO_OK;
    }

    void CHttpSseTransport::SetEndpoint(const std::string& url)
    {
        m_url = url;
    }

    void CHttpSseTransport::SetAuthorization(const std::string& bearerToken)
    {
        m_bearer = bearerToken;
    }

    void CHttpSseTransport::LoadConfig()
    {
        auto& config = Config::GetInstance();
        m_httpsEnabled = config.IsHttpsEnabled();
        m_certFile = config.GetCertFile();
        m_keyFile = config.GetKeyFile();
        
        // Log configuration status (in a real implementation)
        // std::cout << "HTTPS enabled: " << (m_httpsEnabled ? "yes" : "no") << std::endl;
        // if (m_httpsEnabled) {
        //     std::cout << "Cert file: " << m_certFile << std::endl;
        //     std::cout << "Key file: " << m_keyFile << std::endl;
        // }
    }
}


