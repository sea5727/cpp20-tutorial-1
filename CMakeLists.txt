cmake_minimum_required(VERSION 3.0.0)
project(cpp20-tutorial-1 VERSION 0.1.0)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(CTest)
enable_testing()


include_directories(/usr/local/include)
link_directories(/usr/local/lib64)
include_directories(/usr/local/gcc/include)
link_directories(/usr/local/gcc/lib64)
find_package(Boost 1.62 REQUIRED COMPONENTS coroutine)

# add_executable(cpp20-tutorial-1 main.cpp)
# # target_link_libraries(cpp20-tutorial-1 stdc++fs)
# target_link_libraries(cpp20-tutorial-1 Boost::coroutine)

add_executable(example-1 example1.cpp)
target_link_libraries(example-1 Boost::coroutine)

add_executable(example-2 example2.cpp)
target_link_libraries(example-2 Boost::coroutine)

add_executable(example-3 example3.cpp)
target_link_libraries(example-3 Boost::coroutine)

add_executable(example-4 example4.cpp)
target_link_libraries(example-4 Boost::coroutine)

add_executable(example-5 example5.cpp)
target_link_libraries(example-5 Boost::coroutine)

add_executable(example-6-1 example6-1.cpp)
target_link_libraries(example-6-1 Boost::coroutine)

add_executable(example-6-2 example6-2.cpp)
target_link_libraries(example-6-2 Boost::coroutine)

add_executable(example-7 example7.cpp)
target_link_libraries(example-7 Boost::coroutine)

add_executable(example-8 example8.cpp)
target_link_libraries(example-8 Boost::coroutine)

add_executable(example-9 example9.cpp)
target_link_libraries(example-9 Boost::coroutine)

add_executable(chat_message chat_message.cpp)
target_link_libraries(chat_message Boost::coroutine)

add_executable(multi_thread_server multi_thread_server.cpp)
target_link_libraries(multi_thread_server Boost::coroutine)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# set(GCC_COVERAGE_COMPILE_FLAGS -fcoroutines)


include(CPack)
