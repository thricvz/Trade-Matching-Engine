cmake_minimum_required(VERSION 3.14)
project(my_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})



if(ENABLE_TESTS)
    add_executable(DataTransmissionModuleTESTS tests.cpp)
    target_link_libraries(DataTransmissionModuleTESTS GTest::gtest_main DataTransmissionModule)
    add_test(NAME DataTranmissionModuleTests COMMAND $<TARGET_FILE:DataTransmissionModuleTESTS>)
endif()