#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <signal.h>
#include "EchoServer.h"
#include "../../Source/Protocol/Public/Config.h"


int LaunchEchoServer()
{
    // 0. Load configuration
    auto& config = MCP::Config::GetInstance();
    int configErr = config.LoadFromFile("config.ini");
    if (MCP::ERRNO_OK != configErr)
    {
        std::cout << "Warning: Could not load config.ini, using defaults" << std::endl;
    }

    // 1. Configure the Server.
    auto& server = Implementation::CEchoServer::GetInstance();
    int iErrCode = server.Initialize();
    if (MCP::ERRNO_OK == iErrCode)
    {
        // 2. Start the Server.
        iErrCode = server.Start();
        if (MCP::ERRNO_OK == iErrCode)
        {
            // 3. Stop the Server.
            server.Stop();
        }
    }

    return iErrCode;
}


std::atomic_bool g_bStop{ false };
void signal_handler(int signal) { g_bStop = true; }

int main(int argc, char* argv[])
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    return LaunchEchoServer();
}
