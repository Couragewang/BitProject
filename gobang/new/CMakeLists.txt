cmake_minimum_required(VERSION 3.7)
project(GobangServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")

find_package(Boost COMPONENTS system REQUIRED)
include_directories(
        "./msgpack/include"
        "./server"
)

add_executable(GobangServer GobangServer.cpp)
target_link_libraries(GobangServer ${Boost_LIBRARIES})
