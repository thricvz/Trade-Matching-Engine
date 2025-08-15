
find_package(SQLite3 REQUIRED)
include_directories(include)

add_library(DataBaseModule DataBase.cpp)
target_link_libraries(DataBaseModule SQLite::SQLite3)
if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()