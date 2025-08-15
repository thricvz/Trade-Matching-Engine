cmake_minimum_required(VERSION 3.26)
project(ORDERBOOK)

set(CMAKE_CXX_STANDARD 17)


set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

add_library(OrderbookModule  orderbook.cpp)

if(${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
