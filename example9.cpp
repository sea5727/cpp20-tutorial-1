#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <thread>

using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::asio::streambuf;


int main(int, char**) {

    std::cout << "Hello, world! test : \n";
    boost::asio::io_service io_service_;
    boost::asio::io_service::strand strand_(io_service_);
    boost::asio::ip::tcp::acceptor acceptor_(io_service_);

    acceptor_.open(tcp::v4());
    acceptor_.set_option(tcp::acceptor::reuse_address(true));
    acceptor_.bind({{}, 1234});
    acceptor_.listen();

    // auto sock = tcp::socket(io_service_, tcp::v4());

    using session = std::shared_ptr<tcp::socket>;

    std::function<void(session)> do_session;
    do_session = [&](session s){
        std::cout << "do_session start.. thread Id: " << std::this_thread::get_id() << std::endl;

        auto buf = std::make_shared<std::vector<char>>(1024);

        s->async_read_some(boost::asio::buffer(*buf), strand_.wrap([&,s,buf](error_code ec, size_t bytes) {
            if (ec)
                std::cerr << "read failed: " << ec.message() << "\n";
            else {
                std::cout << "Echo to " << s->remote_endpoint(ec) << ": " << std::string(buf->data(), bytes);
                if (ec)
                    std::cerr << "endpoint failed: " << ec.message() << "\n";
                else 
                    async_write(*s, boost::asio::buffer(*buf), [&,s,buf](error_code ec, size_t) {
                            if (ec) std::cerr << "write failed: " << ec.message() << "\n";
                        });
                std::this_thread::sleep_for(std::chrono::seconds(3));

                do_session(s); // full duplex, can read while writing, using a second buffer
            }

        }));
        std::cout << "do_session end."  << std::endl;
    };

    std::function<void()> do_accept = [&]{ 
        std::cout << "do_accept : " << std::this_thread::get_id() << " ... test : " << std::endl;
        auto sock = std::make_shared<session::element_type>(io_service_);

        acceptor_.async_accept(*sock, [&, sock](boost::system::error_code const & error){
            std::cout << "async_accept : " << std::this_thread::get_id() << std::endl;
            if(error){
                std::cout << "accept error :" << error.message() << std::endl;
            }
            else {
                std::cout << "accept success " << std::endl;
            }
            std::cout << "async_accept.. do_session.. start?" << std::endl;
            do_session(sock);
            std::cout << "async_accept.. do_accept.. start?" << std::endl;
            do_accept();
            std::cout << "async_accept.. end" << std::endl;
        });
    };



    std::cout << "main : " << std::this_thread::get_id() << std::endl;
    do_accept();
    std::cout << "do_accept??" << std::endl;
    std::vector<std::thread> threads;
    for(int i = 0 ; i < 1 ; i++){
        threads.emplace_back([&]{io_service_.run();});
    }
    io_service_.run();
    std::cout << "end??" << std::endl;
}
