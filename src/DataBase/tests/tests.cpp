#include <gtest/gtest.h>
#include <cstdlib>
#include "DataBase.hpp"

//change the default argument for each platform
void cleanup(const std::string& filename,std::string removeCommand="rm"){
    removeCommand +=" " + filename;
    std::system(removeCommand.c_str());
}
void createFile(const std::string& filename,std::string createFileCommand = "touch"){
    createFileCommand += " " + filename;
    std::system(createFileCommand.c_str());
}

TEST(Initialization,CreateDataBase){
    //check that the file is actually created
    cleanup("new.db");
    DataBase db("new.db");
    cleanup("new.db");
}

TEST(Initialization,LoadExistingDataBase){
    createFile("test.db");
    DataBase db("test.db");
    cleanup("test.db");
}

TEST(UserRegistration,CreateNewUser){
    DataBase db("new.db");
    EXPECT_EQ(db.registerUser("eric","mysecretpassword"),REGISTER_SUCCESS);
    cleanup("new.db");
}

TEST(UserRegistration,existingUser){
    DataBase db("new.db");
    db.registerUser("double","mylittleponey");
    //same username and password
    EXPECT_EQ(db.registerUser("double","mylittleponey"),USER_DUPLICATE_ERROR);
    //same username
    EXPECT_EQ(db.registerUser("double","chiefkiefisgoated"),USER_DUPLICATE_ERROR);
    
    cleanup("new.db");
}
