cmake_minimum_required(VERSION 3.0)

project(dns-checker C CXX)

macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    set (CMAKE_CXX_FLAGS "--std=gnu++14 ${CMAKE_CXX_FLAGS}")
    set (CMAKE_CXX_FLAGS "--std=c++14 ${CMAKE_CXX_FLAGS}")
  else ()
    set (CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
  endif ()
endmacro(use_cxx11)

use_cxx11()

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

if ( CMAKE_COMPILER_IS_GNUCC )
    set (CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic ${CMAKE_CXX_FLAGS}")
endif ( CMAKE_COMPILER_IS_GNUCC )

enable_testing()
add_test(TestProject Test)
