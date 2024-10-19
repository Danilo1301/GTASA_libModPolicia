#include "SocketServer.h"

#include "Log.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

//#include <websocketpp/config/asio_no_tls_client.hpp>
//#include <websocketpp/client.hpp>
#include <iostream>

//https://github.com/zaphoyd/websocketpp.git

// typedef websocketpp::client<websocketpp::config::asio_client> client;

// class WebSocketClient {
// public:
//     WebSocketClient() {
//         // Initialize ASIO transport
//         m_client.init_asio();

//         // Bind the handlers
//         m_client.set_open_handler(bind(&WebSocketClient::on_open, this, ::_1));
//         m_client.set_message_handler(bind(&WebSocketClient::on_message, this, ::_1, ::_2));
//         m_client.set_close_handler(bind(&WebSocketClient::on_close, this, ::_1));
//     }

//     void connect(const std::string& uri) {
//         websocketpp::lib::error_code ec;
//         client::connection_ptr con = m_client.get_connection(uri, ec);

//         if (ec) {
//             std::cout << "Could not create connection because: " << ec.message() << std::endl;
//             return;
//         }

//         m_client.connect(con);
//         m_client.run();
//     }

//     void on_open(websocketpp::connection_hdl hdl) {
//         std::cout << "Connection opened!" << std::endl;
//     }

//     void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {
//         std::cout << "Received message: " << msg->get_payload() << std::endl;
//     }

//     void on_close(websocketpp::connection_hdl hdl) {
//         std::cout << "Connection closed!" << std::endl;
//     }

// private:
//     client m_client;
// };

void SocketServer::Init()
{
    // //std::string address = "ws://192.168.1.4";
    // std::string address = "ws://echo.websocket.org";
    // int port = 443;

    // //menuVSL->debug->visible = true;
    // menuVSL->debug->AddLine("Connecting to " + address + ":" + std::to_string(port));
    // Log::Level(LOG_LEVEL::LOG_BOTH) << address << ":" << std::to_string(port) << std::endl;

    // WebSocketClient ws_client;
    // ws_client.connect(address);
}

//Attempt 1

/*
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
*/