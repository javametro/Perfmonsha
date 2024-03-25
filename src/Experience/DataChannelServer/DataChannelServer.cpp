// DataChannelServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <boost/asio.hpp>
#include <rtc/rtc.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
//#include <api/peer_connection_interface.h>
//#include <api/scoped_refptr.h>
//#include <rtc_base/ref_counted_object.h>
//#include <api/data_channel_interface.h>


namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

int main()
{
    

    try {
       /* rtc::Configuration config;
        config.iceServers.emplace_back("mystunserver.org:3478");
        rtc::PeerConnection pc(config);

        std::shared_ptr<rtc::DataChannel> dc = pc.createDataChannel("myDataChannel");

        dc->onOpen({
            });

        dc->onMessage({
            });*/

      /*  pc.onLocalDescription({
            std::string sdpString = sdp;
            });*/

        
      /*  webrtc::DataChannelInit config;
        rtc::scoped_refptr<webrtc::PeerConnectionInterface> connection;
        auto result = connection->CreateDataChannelOrError("stephentest", &config);
        if (!result.ok()) {
            std::cerr << "CreateDataChannel failed." << std::endl;
        }*/

        
        net::io_context ioc;
        tcp::acceptor acceptor(ioc, { tcp::v4(), 8080 });

        while (true) {
            tcp::socket socket(ioc);
            acceptor.accept(socket);

            //Handle WebSocket handshake
            websocket::stream<tcp::socket> ws(std::move(socket));
            ws.accept();

            //Handle incoming messages
            beast::flat_buffer buffer;
            ws.read(buffer);
            std::cout << "Received: " << beast::make_printable(buffer.data()) << std::endl;

            //Send a response
            ws.write(net::buffer("Hello from the server"));
            
            //close the websocket connection
            ws.close(websocket::close_code::normal);
        }
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

    //pc.onLocalDescription({
    //    //Send the SDP to the remote peer
    //    });

    //pc.onLocalCandidate({
    //    //Handle local ICE candidates (your implementation here)
    //    });

    //pc.setRemoteDescription(remoteSdp);
    //pc.addRemoteCandidate(remoteCandidate);
    //std::cout << "Hello World!\n";
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
