cmake_minimum_required(VERSION 3.2)
project("Server-client project")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
include_directories(DataTransferModule/include)
include_directories(Database/include)

add_subdirectory(DataBase)
add_subdirectory(DataTransferModule)

add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(server DataTransmissionModule)
target_link_libraries(client DataTransmissionModule)