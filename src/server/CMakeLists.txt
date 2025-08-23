


include_directories(ServerFunctionPack/include)
add_subdirectory(ServerFunctionPack)


add_executable(server server.cpp)
target_link_libraries(server ServerFunctionPack)
