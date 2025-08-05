cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_library(RequestModule requestClass.cpp)
add_library(ChunkTransmissionModule ChunkTransmission.cpp)

target_link_libraries(ChunkTransmissionModule RequestModule)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()