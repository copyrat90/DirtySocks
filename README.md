# DirtySocks

My dirty C++20 wrapper around BSD socket API.

My wrapped API is quite similar to the [SFML](https://www.sfml-dev.org/)'s networking API, but it uses `std::error_code` for error handling.

> WANING! This project is HEAVILY WIP, so the API changes pretty often.


## Motivation

I wanted a very thin socket wrapper in C++20, without including any third-party license file in my project.


## License

This project is licensed under the [Zero-Clause BSD license](LICENSE), so you can use it without including my license file.


## Example

### Echo-once server

```cpp
#include <DirtySocks/SocketAddress.hpp>
#include <DirtySocks/System.hpp>
#include <DirtySocks/TcpListener.hpp>
#include <DirtySocks/TcpSocket.hpp>

#include <iostream>
#include <system_error>

void log_error_and_exit(const std::error_code& ec) {
    std::cerr << "[" << ec.value() << "] " << ec.message() << std::endl;
    std::exit(ec.value());
}

int main() {
    // DirtySocks uses `std::error_code` to deal with any socket errors
    std::error_code ec;

    // Inialize DirtySocks
    ds::System::init(ec);
    if (ec) log_error_and_exit(ec);

    // Create a tcp listener socket
    // (OS socket is lazily-created, i.e. For now it's an `INVALID_SOCKET`)
    ds::TcpListener listener;

    // Get the address to bind to:  IPv4 any address, port 23457
    ds::SocketAddress any_v4 = ds::SocketAddress::any(23457, ds::IpVersion::V4);

    // Bind & Listen incoming connections from the any address
    listener.listen(any_v4, ec);
    if (ec) log_error_and_exit(ec);

    // Accept a connection
    ds::TcpSocket client;
    listener.accept(client, ec);
    if (ec) log_error_and_exit(ec);

    // Print the client's address
    std::optional<ds::SocketAddress> client_addr =
        client.get_remote_address(ec);
    if (ec) log_error_and_exit(ec);
    std::string client_addr_str = client_addr->get_presentation(ec);
    if (ec) log_error_and_exit(ec);
    std::cout << "Client " << client_addr_str << " connected!" << std::endl;

    // Receive a message from the client
    char message[256];
    std::size_t received_length;
    client.receive(message, sizeof(message), received_length, ec);
    if (ec) log_error_and_exit(ec);

    // Print the message
    message[received_length] = '\0';
    std::cout << "Client said: " << message << std::endl;

    // Send back the same message
    client.send(message, received_length, ec);
    if (ec) log_error_and_exit(ec);
    std::cout << "Sent echo message!" << std::endl;

    // Close the sockets
    client.close();
    listener.close();

    // Destroy DirtySocks
    ds::System::destroy();
}
```

### Echo-once client

```cpp
#include <DirtySocks/SocketAddress.hpp>
#include <DirtySocks/System.hpp>
#include <DirtySocks/TcpSocket.hpp>

#include <cstring>
#include <iostream>
#include <system_error>

void log_error_and_exit(const std::error_code& ec) {
    std::cerr << "[" << ec.value() << "] " << ec.message() << std::endl;
    std::exit(ec.value());
}

int main() {
    // DirtySocks uses `std::error_code` to deal with any socket errors
    std::error_code ec;

    // Inialize DirtySocks
    ds::System::init(ec);
    if (ec) log_error_and_exit(ec);

    // Get the address to connect to:  IPv4 localhost, port 23457
    std::optional<ds::SocketAddress> addr =
        ds::SocketAddress::resolve("localhost", "23457", ds::IpVersion::V4, ec);
    if (ec) log_error_and_exit(ec);

    // Connect to the address
    ds::TcpSocket sock;
    sock.connect(*addr, ec);
    if (ec) log_error_and_exit(ec);
    std::cout << "Connected!" << std::endl;

    // Send "Hello, world!" message
    char send_message[256] = "Hello, world!";
    sock.send(send_message, std::strlen(send_message), ec);
    if (ec) log_error_and_exit(ec);
    std::cout << "Sent: " << send_message << std::endl;

    // Receive echo'ed message
    char received_message[256];
    std::size_t received_length;
    sock.receive(received_message, sizeof(received_message), received_length, ec);
    if (ec) log_error_and_exit(ec);
    received_message[received_length] = '\0';
    std::cout << "Server echo'ed: " << received_message << std::endl;

    // Close the socket
    sock.close();

    // Destroy DirtySocks
    ds::System::destroy();
}
```
