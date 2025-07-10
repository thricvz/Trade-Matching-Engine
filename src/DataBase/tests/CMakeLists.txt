cmake_minimum_required(VERSION 3.14)
project(my_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

if(ENABLE_TESTS)
    add_executable(DataBaseModuleTESTS tests.cpp)
    target_link_libraries(DataBaseModuleTESTS GTest::gtest_main DataBaseModule)
    add_test(NAME DatabaseTests COMMAND $<TARGET_FILE:DataBaseModuleTESTS>)
endif()