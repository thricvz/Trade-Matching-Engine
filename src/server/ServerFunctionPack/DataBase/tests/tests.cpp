#include <gtest/gtest.h>
#include <cstdlib>
#include "DataBase.hpp"
#include "DataBaseCommunicationCodes.hpp"
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


TEST(DataInteraction,getId){
    cleanup("new.db");
    
    DataBase db("new.db");
    db.registerUser("eric","mylittleponey");
    db.registerUser("davi","mysecretpassword");

    EXPECT_EQ(db.getUserId("davi","mysecretpassword"),2);
    EXPECT_EQ(db.getUserId("eric","mylittleponey"),1);
    EXPECT_EQ(db.getUserId("anon","an0nym0us"),USER_NOT_FOUND);

    cleanup("new.db");
}


TEST(DataInteraction,getAccountBalance){
    cleanup("MyDataBase.db");
    
    DataBase db("MyDataBase.db");
    db.registerUser("eric","mylittleponey");

    int userId = db.getUserId("eric","mylittleponey");
    std::pair<int,int> defaultAccountBalance= {200,10}; 
    
    EXPECT_EQ(db.getUserBalance(userId),defaultAccountBalance);
    cleanup("MyDataBase.db");
}


TEST(DataInteraction,setAccountBalance){
    cleanup("MyDataBase.db");
    
    DataBase db("MyDataBase.db");
    db.registerUser("eric","mylittleponey");

    int userId = db.getUserId("eric","mylittleponey");
    db.setUserBalance(userId,0,0);
    std::pair<int,int> expectedAccountBalance= {0,0}; 
    
    EXPECT_EQ(db.getUserBalance(userId),expectedAccountBalance);
    cleanup("MyDataBase.db");
}


TEST(DataInteraction,UserStockHoldingInteraction){
    cleanup("MyDataBase.db");
    
    DataBase db("MyDataBase.db");
    db.registerUser("eric","mylittleponey");

    int userId = db.getUserId("eric","mylittleponey");
    int expectedStocksAmount =  150;
    
    db.setUserStockHolding(userId,expectedStocksAmount);
     
    
    EXPECT_TRUE(db.getUserStockHolding(userId)==expectedStocksAmount);
    cleanup("MyDataBase.db");
}


