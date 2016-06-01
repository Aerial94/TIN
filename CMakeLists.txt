cmake_minimum_required(VERSION 3.2)
project(dns-checker C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# custom cmake find_package modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# JSONCPP
find_package(Jsoncpp REQUIRED)
include_directories(${JSONCPP_INCLUDE_DIR})

find_package (Threads REQUIRED)


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
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
set(TEST_SOURCE_FILES ${SRC_LIST})

list(REMOVE_ITEM TEST_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
add_executable(Test ${SRC_LIST_TEST} ${TEST_SOURCE_FILES})
target_link_libraries(Test ${JSONCPP_LIBRARIES})
target_link_libraries(Test ${CMAKE_THREAD_LIBS_INIT})

enable_testing()
add_test(TestProject Test)
