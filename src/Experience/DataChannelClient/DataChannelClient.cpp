// DataChannelClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;


int main()
{
    try {
        net::io_context ioc;
        tcp::resolver resolver(ioc);
        websocket::stream<tcp::socket> ws(ioc);

        //Resolve the WebSocket server address
        auto const results = resolver.resolve("localhost", "8080");
        net::connect(ws.next_layer(), results.begin(), results.end());

        //Perform WebSocket handshake
        ws.handshake("localhost", "/");

        //Send message
        ws.write(net::buffer("Hello from the client"));

        //receive a response from the server
        beast::flat_buffer buffer;
        ws.read(buffer);
        std::cout << "Received: " << beast::make_printable(buffer.data()) << std::endl;

        //Close the Websocket connection
        ws.close(websocket::close_code::normal);
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
