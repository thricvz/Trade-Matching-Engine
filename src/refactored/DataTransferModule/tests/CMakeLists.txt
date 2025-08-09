cmake_minimum_required(VERSION 3.14)
project(my_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})



if(ENABLE_TESTS)
    add_executable(testMain tests.cpp)
    target_link_libraries(testMain GTest::gtest_main RequestModule)
    add_test(NAME RequestModuleTests COMMAND $<TARGET_FILE:testMain>)
endif()