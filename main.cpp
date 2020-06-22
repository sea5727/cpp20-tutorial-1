// Synchronous echo server.

#include <array>
#include <iostream>
#include <string>

#include "boost/asio.hpp"

using boost::asio::ip::tcp;

enum { BUF_SIZE = 1024 };

void Session(tcp::socket socket) {
  try {
    while (true) {
      std::cout << "read_some start" << std::endl;
      std::array<char, BUF_SIZE> data;

      boost::system::error_code ec;
      std::size_t length = socket.read_some(boost::asio::buffer(data), ec);
      std::cout << "read_some.. length : " << length << std::endl;

      if (ec == boost::asio::error::eof) {
        std::cout << "Connection closed cleanly by peer." << std::endl;
        break;
      } else if (ec) {
        // Some other error
        std::cout << "read_some error..." << std::endl;
        throw boost::system::system_error(ec);
      }

      boost::asio::write(socket, boost::asio::buffer(data, length));
    }
  } catch (const std::exception& e) {
    std::cerr << "Exception: " <<  e.what() << std::endl;
  }
  std::cout << "Session end" << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
    return 1;
  }

  unsigned short port = std::atoi(argv[1]);

  boost::asio::io_context io_context;

  // Create an acceptor to listen for new connections.
  tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

  try {
    // Handle one connection at a time.
    while (true) {
      // The socket object returned from accept will be moved to Session's
      // parameter without any copy cost.
      std::cout << "Session start " << std::endl;
      Session(acceptor.accept());
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}