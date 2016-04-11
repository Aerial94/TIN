project(dns-checker CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
cmake_minimum_required(VERSION 2.8)
file(GLOB SRC_LIST
    "src/*.hpp"
    "src/*.cpp"
)
add_executable(${PROJECT_NAME} ${SRC_LIST})
