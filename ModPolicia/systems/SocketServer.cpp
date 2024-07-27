#include "SocketServer.h"

#include "Log.h"

#include "client/WebSocket.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

void SocketServer::Init()
{
    std::string address = "ws://192.168.1.4";
    address = "wss://echo.websocket.org";
    int port = 443;

    //menuVSL->debug->visible = true;
    menuVSL->debug->AddLine("Connecting to " + address + ":" + std::to_string(port));
    Log::Level(LOG_LEVEL::LOG_BOTH) << address << ":" << std::to_string(port) << std::endl;

    WebSocket client_socket;

    auto err = client_socket.connectSocket(address, port);

    if(err)
    {
        menuVSL->debug->AddLine("Connection error: " + std::to_string(err));
        Log::Level(LOG_LEVEL::LOG_BOTH) << "result: " << err << std::endl;
    }
}