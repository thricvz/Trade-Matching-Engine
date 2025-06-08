cmake_minimum_required(VERSION 3.2)
project("Server-client project")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
include_directories(DataTransferModule/include)


add_subdirectory(DataTransferModule)
add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(server requestModule)