#include <gtest/gtest.h>
#include "DataBase.hpp"


TEST(Initialization,CreateDataBase){
    DataBase db("new.db");
}

TEST(Initialization,LoadExistingDataBase){
    DataBase db("test.db");
}

TEST(UserRegistration,CreateNewUser){
    DataBase db("new.db");
    db.registerUser("eric","mysecretpassword");
}

TEST(UserRegistration,existingUser){
    DataBase db("new.db");
    db.registerUser("double","mylittleponey");
    //same username and password
    EXPECT_EQ(db.registerUser("double","mylittleponey"),USER_EXISTS_ERROR);
    //same username
    EXPECT_EQ(db.registerUser("double","chiefkiefisgoated"),USER_EXISTS_ERROR);
}
