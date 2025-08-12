
include_directories(RequestSender/include)
add_subdirectory(RequestSender)


add_executable(client client.cpp)
target_link_libraries(client RequestSenderModule)
