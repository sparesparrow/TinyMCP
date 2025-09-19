#include <iostream>
#include <string>
#include "../../../Source/Protocol/Transport/Transport.h"
#include "../../../Source/Protocol/Transport/HttpSseTransport.h"
#include "../../../Source/Protocol/Session/Session.h"

int main()
{
    // Minimal client placeholder: demonstrate constructing transports
    MCP::CStdioTransport stdio;
    MCP::CHttpSseTransport httpSse;
    httpSse.SetEndpoint("http://127.0.0.1:9000/mcp");

    std::cout << "MCPClient scaffold built successfully." << std::endl;
    return 0;
}


