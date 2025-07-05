cmake_minimum_required(VERSION 3.2)
project("Server-client project")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ENABLE_TESTS ON)

include_directories(include)
include_directories(DataBase/include)
include_directories(DataBaseCommunication/include)
include_directories(DataTransferModule/include)


add_subdirectory(DataBase)
add_subdirectory(DataBaseCommunication)
add_subdirectory(DataTransferModule)

add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(server DataTransmissionModule DataBaseModule DataBaseCommunicationModule)
target_link_libraries(client DataTransmissionModule DataBaseModule)