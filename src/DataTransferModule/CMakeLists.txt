cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}DataBaseCommunication/include)

add_library(DataTransmissionModule requestClass.cpp ChunkTransmission.cpp requestHandler.cpp )

target_link_libraries(DataTransmissionModule DataBaseCommunicationModule)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()