#include <boost/asio.hpp>
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>

using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::asio::streambuf;

// using namespace std;
int main() {
    
    boost::asio::io_service service;
    boost::asio::deadline_timer timer(service, boost::posix_time::seconds(5));

    
    timer.async_wait([](auto ... vn){
        std::cout << boost::chrono::system_clock::now()
                  << " : timer expired\n";

    });

    std::cout << boost::chrono::system_clock::now() << " : calling run\n";
    service.run();
    std::cout << boost::chrono::system_clock::now() << " : done\n";
}
