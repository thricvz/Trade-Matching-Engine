
find_package(SQLite3 REQUIRED)
include_directories(include)
add_library(DatabaseModule DataBase.cpp)