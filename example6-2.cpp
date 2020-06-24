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
    boost::asio::io_service::strand strand(service);

    boost::asio::deadline_timer timer1(service, boost::posix_time::seconds(5));
    boost::asio::deadline_timer timer2(service, boost::posix_time::seconds(5));

    service.post([] { 
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "eat1.. " << std::this_thread::get_id() << std::endl;
        });
    service.post([] { 
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "eat2.. " << std::this_thread::get_id() << std::endl;
        });
    service.post([] { 
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "eat3.. " << std::this_thread::get_id() << std::endl;
        });
    service.post([] { 
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "eat4.. " << std::this_thread::get_id() << std::endl;
        });


    timer1.async_wait(
        strand.wrap([](const boost::system::error_code& err/*e*/){ timer_expired("timer1");})
    );
    timer2.async_wait(
        strand.wrap([](const boost::system::error_code& err/*e*/){ timer_expired("timer2");})
    );

    std::thread ta([&]() { service.run();});
    std::thread tb([&]() { service.run();});

    ta.join();
    tb.join();


    std::cout << "done.\n";
}
