cmake_minimum_required(VERSION 3.14)
project(my_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test tests.cpp)
add_executable(aux aux.cpp)

target_link_libraries(test GTest::gtest_main requestModule)
target_link_libraries(aux requestModule)