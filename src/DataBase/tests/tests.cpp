#include <gtest/gtest.h>
#include "DataBase.hpp"

TEST(DataBaseModule,ConstructExistingDataBase){
    DataBase db("test.db");
}


TEST(DataBaseModule,CreateNewDataBase){
    DataBase db("new.db");
}

