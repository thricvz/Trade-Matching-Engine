cmake_minimum_required(VERSION 3.2)
project("Server-client project REFACTORED")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wpedantic)

set(ENABLE_TESTS ON)

if(ENABLE_TESTS)
    set(CMAKE_BUILD_TYPE Debug)
    enable_testing()
endif()

include_directories(DataTransferModule/include)
add_subdirectory(DataTransferModule)

add_subdirectory(client)
add_subdirectory(server)
