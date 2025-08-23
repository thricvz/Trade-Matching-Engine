#pragma once
#include <utility>
#include <sqlite3.h>
#include <string>
#include <map>
#include "DataBaseCommunicationCodes.hpp"

//modify to use relative path (I'm aware that this is a very poor practice,Will be corrected in the future)
#define DIR_PATH "/home/eric/Projects/Server/src/server/ServerFunctionPack/DataBase/SqlRequests/"

using std::string;
using std::map;
//register codes
class DataBase{
    sqlite3 *db=nullptr;
    string filePath;
    //methods for the constructor
    void createDB();
    int connect();
    
    //functionality for modular scripts
    void loadScript(string scriptName,string &requestBuffer);
    void loadScript(string scriptName,string &requestBuffer,map<string,string> &tags);
    void replaceTagsInLine(string &buffer,map<string,string> &tags);
        
    public:
        DataBase(const char *PATH);
        ~DataBase();

        int registerUser(string username, string password);
        int getUserId(string username, string password);

        std::pair<int,int> getUserBalance(int userId);
        void setUserBalance(int userId,int dollars,int cents);
        
        
        int getUserStockHolding(int userId); 
        void setUserStockHolding(int userId,int newStockHolding); 


};