cmake_minimum_required(VERSION 3.29)
project(HFT_Modules)

set(CMAKE_CXX_STANDARD 26)

add_executable(HFT_Modules main.cpp
        include/network/tcp_client.hpp
        src/network/tcp_client.cpp
        tests/network/test_client.cpp
        tests/network/test_server.cpp)
