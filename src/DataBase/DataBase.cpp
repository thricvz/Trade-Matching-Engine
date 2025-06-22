#include "DataBase.hpp"
#include <fstream>
#include <iostream>


//Callback Functions
static int UserFoundCallback(void *returnVal, int count, char **data, char **columns)
{
    bool *UserFound = static_cast<bool*>(returnVal);
    *UserFound = count >0;
    return 0;
}



//Class Methods
DataBase::DataBase(const char *PATH){
    filePath= string(PATH);
    if(connect()!=CONNECT_SUCCESS)
        throw std::runtime_error("Cant  open database");
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
    string sqlRequest;
    loadScript("CreateUsersTable.txt",sqlRequest);
    sqlite3_exec(db,sqlRequest.c_str(),NULL,nullptr,nullptr);   
    
};

//functions for the login functionality 
int DataBase::registerUser(string username, string password){
    
    map<string,string> tags = {{"@password",password},{"@username",username}};
    string sqlRequest;
    bool foundUser = false;
    //check if user already exists
    loadScript("RetrieveUsername.txt",sqlRequest,tags);
    sqlite3_exec(db,sqlRequest.c_str(),UserFoundCallback,&foundUser,nullptr);   
    
    if(foundUser)
        return USER_DUPLICATE_ERROR;


    //register user
    loadScript("RegisterUser.txt",sqlRequest,tags);
    sqlite3_exec(db,sqlRequest.c_str(),NULL,nullptr,nullptr);   

    return REGISTER_SUCCESS;
};

void DataBase::loadScript(string scriptName,string &requestBuffer){
    scriptName = "/home/eric/Projects/Server/src/DataBase/SqlRequests/" + scriptName;
    std::ifstream script(scriptName.c_str());
    string line;

    while(getline(script,line)){
        requestBuffer += line;
    };
};

void DataBase::loadScript(string scriptName,string &requestBuffer, map<string,string> &tags){
    scriptName = "/home/eric/Projects/Server/src/DataBase/SqlRequests/" + scriptName;
    std::ifstream script(scriptName.c_str());
    string line;

    
    while(getline(script,line)){
        replaceTagsInLine(line,tags);
        requestBuffer += line;
    };
};


void DataBase::replaceTagsInLine(string &buffer,map<string,string> &tags){
    for(auto pair = tags.begin(); pair != tags.end(); ++pair){

        string tag = pair->first;
        string replacement = pair->second;

        int replacementLength =  tag.size();
        
        int searchStart = 0;
        int startOfTag = buffer.find(tag,searchStart);

        while(startOfTag!=string::npos){
            buffer.replace(startOfTag,replacementLength,replacement);
            searchStart=startOfTag+replacementLength;
            startOfTag = buffer.find(tag,searchStart);
        }
    }
};
int DataBase::getUserId(string username, string password){
    return 0;

};

//functions for updating the user account balance
std::pair<int,int> DataBase::getUserBalance(int userId){

};
