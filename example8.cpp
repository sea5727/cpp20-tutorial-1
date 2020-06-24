#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <thread>

using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::asio::streambuf;


template <typename ConnectionHandler>
class asio_generic_server{
    using shared_handler_t = std::shared_ptr<ConnectionHandler>;
    
public:

    asio_generic_server(int thread_count = 1) 
        : thread_count_(thread_count)
        , acceptor_(io_service_)
    {
    }
    void start_server(uint16_t port){
        std::cout << "start server .. port : " << port << std::endl;
        auto handler = std::make_shared<ConnectionHandler> (std::ref(io_service_));
        // auto handler = std::make_shared<ConnectionHandler> (io_service_);

        
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();

        std::cout << "start server .. listen.." << std::endl;
        acceptor_.async_accept( handler->socket() 
                               , [=] (boost::system::error_code ec) {
                                   handle_new_connection(handler, ec);
                               });
        std::cout << "start server .. emplace_back.." << std::endl;
        for(int i = 0 ; i < thread_count_; i++){
            thread_pool_.emplace_back( [=]{io_service_.run();});
        }
        std::cout << "start server .. end.." << std::endl;
    }
private:
    void handle_new_connection( shared_handler_t handler,
                                boost::system::error_code const & error){
        std::cout << "asio_generic_server handle_new_connection" << std::endl;
        if(error) return;
        handler->start();

        auto new_handler
            = std::make_shared<ConnectionHandler> (io_service_);

        acceptor_.async_accept( new_handler->socket()
                                , [=](boost::system::error_code ec) {
                                    handle_new_connection( new_handler, ec);
                                });
    }
    int thread_count_;
    std::vector<std::thread> thread_pool_;
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
};


class chat_handler : public std::enable_shared_from_this<chat_handler>
{
public:
    
    chat_handler(boost::asio::io_service & service) 
        : service_(service)
        , socket_(service)
        , write_strand_(service)
        {
            std::cout << "chat_handler constructor" << std::endl;
        }
    boost::asio::ip::tcp::socket& socket(){
        std::cout << "chat_handler socket" << std::endl;
        return socket_;
    }

    void start(){
        std::cout << "chat_handler start" << std::endl;
        read_packet();
    }

    void read_packet(){
        std::cout << "chat_handler read_packet" << std::endl;
        boost::asio::async_read_until( socket_, 
                                        in_packet_,
                                        '\0',
                                        [me=shared_from_this()] ( boost::system::error_code const & ec, std::size_t bytes_xfer) {
                                            me->read_packet_done(ec, bytes_xfer);
                                        });
    }

    void read_packet_done(boost::system::error_code const & error, 
                          std::size_t bytes_transferred){
        std::cout << "chat_handler read_packet_done" << std::endl;
        if(error) return;
        
        std::istream stream(&in_packet_);
        std::string packet_string;
        stream >> packet_string;

        // do someting with it

        read_packet();
    }

    // void packet_send_done(boost::system:::error_code const & error){
    void packet_send_done(boost::system::error_code const & error){
        std::cout << "chat_handler packet_send_done" << std::endl;
        if(!error){
            send_packet_queue.pop_front();
            if(!send_packet_queue.empty()) { start_packet_send();}
        }
    }

    void start_packet_send(){
        std::cout << "chat_handler start_packet_send" << std::endl;
        send_packet_queue.front() += '\0';
        async_write( socket_
                    , boost::asio::buffer(send_packet_queue.front())
                    , write_strand_.wrap( [me=shared_from_this()]( boost::system::error_code const & ec, std::size_t) {
                        me->packet_send_done(ec);
                    }));

    }

    void queue_message(std::string message){
        std::cout << "chat_handler queue_message" << std::endl;
        bool write_in_progress = !send_packet_queue.empty();
        send_packet_queue.push_back(std::move(message));

        if(!write_in_progress){
            start_packet_send();
        }
    }

    void send(std::string msg){
        std::cout << "chat_handler send" << std::endl;
        service_.post( write_strand_.wrap( [me=shared_from_this(), msg]() {
            me->queue_message(msg);
        }));
    }
private:


    boost::asio::io_service& service_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::io_service::strand write_strand_;
    boost::asio::streambuf in_packet_;
    std::deque<std::string> send_packet_queue; 
    
};

int main() {
    asio_generic_server<chat_handler> server(1);
    server.start_server(1234);
    while(1){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
}
