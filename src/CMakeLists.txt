cmake_minimum_required(VERSION 3.2)
project("Server-client project")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wpedantic)
set(ENABLE_TESTS ON)

if(ENABLE_TESTS)
    enable_testing()
endif()

include_directories(include)
include_directories(DataBase/include)
include_directories(DataBaseCommunication/include)
include_directories(DataTransferModule/include)
include_directories(OrderBook/include)

add_subdirectory(DataBase)
add_subdirectory(DataBaseCommunication)
add_subdirectory(DataTransferModule)
add_subdirectory(OrderBook)

add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(server DataTransmissionModule DataBaseModule DataBaseCommunicationModule orderbookModule)
target_link_libraries(client DataTransmissionModule DataBaseModule)