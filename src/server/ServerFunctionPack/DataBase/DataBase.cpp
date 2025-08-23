#include "DataBase.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>



//Callback Functions
static int UserFoundCallback(void *returnVal, int count, char **data, char **columns)
{
    bool *UserFound = static_cast<bool*>(returnVal);
    *UserFound = count >0;
    return 0;
}
//no duplicates allowed in the database
static int RetrieveUserIdCallback(void *outId, int count, char **data, char **columns){
    int *userId = static_cast<int*>(outId); 
    *userId =atoi(data[0]);
    return 0;

}

static int RetrieveUserAccountBalanceCallback(void *accountBalance, int count, char **data, char **columns){
    std::pair<int,int> *accountBalancePair = static_cast<std::pair<int,int>*>(accountBalance);
    //int the script we first have dollars
    accountBalancePair->first = atoi(data[0]);
    accountBalancePair->second = atoi(data[1]);

    return 0;
}


static int RetrieveUserStockHoldingsCallback(void *stockAmount, int count, char **data, char **columns){
    int *resultPtr = static_cast<int*>(stockAmount);
    //int the script we first have dollars
    *resultPtr = atoi(data[0]);
    return 0;
} 
//Class Methods
DataBase::DataBase(const char *PATH){
    sqlite3_config(SQLITE_CONFIG_SERIALIZED);
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
    scriptName = std::string(DIR_PATH) + scriptName;
    std::ifstream script(scriptName.c_str());
    string line;

    while(getline(script,line)){
        requestBuffer += line;
    };
};

void DataBase::loadScript(string scriptName,string &requestBuffer, map<string,string> &tags){
    scriptName = std::string(DIR_PATH) + scriptName;
    std::ifstream script(scriptName.c_str());
    string line;
    
    while(getline(script,line)){
        replaceTagsInLine(line,tags);
        requestBuffer += line;
        requestBuffer+='\n';
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
    map<string,string> tags = {{"@password",password},{"@username",username}};
    string sqlRequest;
    int userId = USER_NOT_FOUND;
    loadScript("RetrieveUserId.txt",sqlRequest,tags);
    sqlite3_exec(db,sqlRequest.c_str(),RetrieveUserIdCallback,&userId,nullptr);   

    return userId;
};

//functions for updating the user account balance
std::pair<int,int> DataBase::getUserBalance(int userId){
    map<string,string> tags = {{"@id",std::to_string(userId)}};
    string sqlRequest;
    std::pair<int,int> accountBalance;
    loadScript("RetrieveUserAccountBalance.txt",sqlRequest,tags);
    sqlite3_exec(db,sqlRequest.c_str(),RetrieveUserAccountBalanceCallback,&accountBalance,nullptr);   

    return accountBalance;
};



void DataBase::setUserBalance(int userId,int dollars,int cents){
    map<string,string> tags = {
        {"@id",std::to_string(userId)},
        {"@dollarsAmount",std::to_string(dollars)},
        {"@centsAmount",std::to_string(cents)}
    };
    string sqlRequest;
    loadScript("UpdateUserAccountBalance.txt",sqlRequest,tags);
    sqlite3_exec(db,sqlRequest.c_str(),NULL,nullptr,nullptr);   
};


void DataBase::setUserStockHolding(int userId,int newStockHolding){
        
    map<string,string> tags = {
        {"@id",std::to_string(userId)},
        {"@stocksAmount",std::to_string(newStockHolding)}
    };
    string sqlRequest;
    loadScript("UpdateUserStocks.txt",sqlRequest,tags);
    sqlite3_exec(db,sqlRequest.c_str(),NULL,nullptr,nullptr);   
};

int DataBase::getUserStockHolding(int userId){
    map<string,string> tags = {{"@id",std::to_string(userId)}};
    string sqlRequest;
    int stockQuantity{ };
    loadScript("RetrieveUserStocks.txt",sqlRequest,tags);
    sqlite3_exec(db,sqlRequest.c_str(),RetrieveUserStockHoldingsCallback,&stockQuantity,nullptr);   
    return stockQuantity;
}; 
