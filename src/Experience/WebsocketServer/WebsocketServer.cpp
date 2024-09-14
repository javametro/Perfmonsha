#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <fstream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;

// Replace with your desired port number
const unsigned short port = 8888;

void handle_connection(websocket::stream<tcp::socket>& ws, boost::asio::io_context& ioc) {
    // Read the filename from the client
    beast::multi_buffer buffer;
    ws.read(buffer);
    std::string filename = beast::buffers_to_string(buffer.data());

    // Open the file for reading in binary mode
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        ws.write(beast::buffer(beast::websocket::frame(true, websocket::opcode::text)), beast::write_options::close);
        ws.close(websocket::close_code::cannot_open);
        return;
    }

    // Read file data in chunks and send to client
    ws.binary(true);
    char data[1024];
    while (file.read(data, sizeof(data))) {
        ws.write(beast::buffer(data, file.gcount()));
    }

    ws.write(beast::buffer(beast::websocket::frame(true, websocket::opcode::binary)), beast::write_options::close);
    ws.close(websocket::close_code::normal);
}

int main() {
    boost::asio::io_context ioc;

    tcp::acceptor acceptor(ioc, { tcp::v4(), port });

    while (true) {
        websocket::stream<tcp::socket> ws(ioc);
        acceptor.accept(ws.next_layer());

        // Perform handshake
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req) {
                req.set(http::field::user_agent, "Beast File Transfer Server");
            }));
        ws.handshake(acceptor.local_endpoint().address().to_string(), "/");

        // Handle connection in a separate thread
        std::thread([&ws, &ioc]() { handle_connection(ws, ioc); }).detach();
    }

    return 0;
}
