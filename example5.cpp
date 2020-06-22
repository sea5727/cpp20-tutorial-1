#include <boost/asio.hpp>
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <thread>


using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::asio::streambuf;

void timer_expired(std::string id){
    std::cout << boost::chrono::system_clock::now() << " " << id << " enter.\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << boost::chrono::system_clock::now() << " " << id << " leave.\n";
}
int main() {
    boost::asio::io_service service;

    service.post([] { std::cout << "eat\n"; });
    service.post([] { std::cout << "drink\n"; });
    service.post([] { std::cout << "and be merry!\n"; });

    std::cout << "thraed butler start\n";
    std::thread butler([&]{ service.run();});
    std::cout << "thraed butler join\n";
    butler.join();

    std::cout << "done.\n";
}
