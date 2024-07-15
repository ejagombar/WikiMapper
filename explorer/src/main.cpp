#include <iostream>

#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;
using std::cout;
using std::endl;

// Performs handshake and ensures that the server supports version 4.3 of the bolt protocol.
bool handshake(tcp::socket &socket) {
    // First 4 bytes: handshake, followed by 4 sets of 4 bytes that show the versions that the client supports
    uint8_t handshake[20] = {0x60, 0x60, 0xb0, 0x17, 0x00, 0x00, 0x03, 0x04, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t response[4];

    boost::asio::write(socket, boost::asio::buffer(handshake, 20));

    boost::asio::read(socket, boost::asio::buffer(response, 4));

    return (response[3] == 4) && (response[2] == 3);
}

void send_hello(tcp::socket &socket) {
    // Example HELLO message (simplified for illustration)
    uint8_t hello_message[] = {0xB1, 0x01, 0xA1, 0x8F, 0x6E, 0x65, 0x6F, 0x34,
                               0x6A, 0x2F, 0x31, 0x2E, 0x31, 0x30, 0x2E, 0x30};
    boost::asio::write(socket, boost::asio::buffer(hello_message, sizeof(hello_message)));

    // Read server response
    uint8_t response[256];
    size_t len = socket.read_some(boost::asio::buffer(response));

    std::cout << "Response: ";
    for (size_t i = 0; i < len; ++i) {
        std::cout << std::hex << static_cast<int>(response[i]) << " ";
    }
    std::cout << std::endl;
}

void connectToNeo4j() {
    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("localhost", "7687");
        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        std::cout << "Connected to Neo4j" << std::endl;

        handshake(socket) ? cout << "Handshake successful (Bolt V4.3)" << endl
                          : cout << "Handshake unsuccessful (Unsupported Bolt version?)" << endl;

        send_hello(socket);

    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

int main() {
    connectToNeo4j();
    return 0;
}
