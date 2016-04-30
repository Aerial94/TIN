project(dns-checker CXX)

# custom cmake find_package modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# JSONCPP
find_package(Jsoncpp REQUIRED)
include_directories(${JSONCPP_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
cmake_minimum_required(VERSION 2.8)
file(GLOB SRC_LIST
    "src/*.hpp"
    "src/*.cpp"
)

file(GLOB SRC_LIST_TEST
        "test/*.hpp"
        "test/*.cpp"
        )


add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${JSONCPP_LIBRARIES})
set(TEST_SOURCE_FILES ${SRC_LIST})

list(REMOVE_ITEM TEST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
add_executable(Test ${SRC_LIST_TEST} ${TEST_SOURCE_FILES})
target_link_libraries(Test ${JSONCPP_LIBRARIES})

enable_testing()
add_test(TestProject Test)
