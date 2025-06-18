#include "DataBase.hpp"
#include <fstream>
#include <iostream>

DataBase::DataBase(const char *PATH){
    filePath= string(PATH);
    if(connect()!=CONNECT_SUCCESS)
        createDB();        
    
};
DataBase::~DataBase(){
    sqlite3_close(db);
};
int DataBase::connect(){
    int resultCode = sqlite3_open(filePath.c_str(),&db);
    if(resultCode== SQLITE_OK)
        return CONNECT_SUCCESS;
    return CONNECT_FAILED;
    
}
void DataBase::createDB(){
    std::ofstream DataBaseFile(filePath.c_str());
    connect();


    string sqlRequest;
    loadScript("CreateUsersTable.txt",sqlRequest);
    std::cout << sqlRequest;
    
};
//functions for the login functionality 
int DataBase::registerUser(const char* username, const char* password){
    return 0;
};

void DataBase::loadScript(string scriptName,string &requestBuffer){
    scriptName = "SqlRequests/" + scriptName;
    std::ifstream script(scriptName.c_str());
    string line;

    while(getline(script,line)){
        requestBuffer += line;
    };
};

int DataBase::getUserId(const char* username, const char* password){
    return 0;

};

//functions for updating the user account balance
std::pair<int,int> DataBase::getUserBalance(int userId){

};
