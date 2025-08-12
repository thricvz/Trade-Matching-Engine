

add_subdirectory(Orderbook)
add_subdirectory(RequestHandler)


include_directories(Orderbook/include)
include_directories(RequestHandler/include)


add_executable(server server.cpp)
target_link_libraries(server OrderbookModule RequestHandlerModule)
