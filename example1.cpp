#include <boost/coroutine/all.hpp>
#include <iostream>

using namespace std;
using namespace boost::coroutines;

void cooperative(coroutine<void>::push_type &sink){
    std::cout << "Hello";
    sink();
    cout << "World" ;
}


int main(int, char**) {
    coroutine<void>::pull_type source{ cooperative };
    cout << ", ";
    source();
    cout << "!\n";

}
