#pragma once
#include <utility>
#include <sqlite3.h>
#include <string>
#include <map>
#include "DataBaseCommunicationCodes.hpp"

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

        //
        //functions for the login functionality 
        int registerUser(string username, string password);
        int getUserId(string username, string password);

        //functions for updating the user account balance
        std::pair<int,int> getUserBalance(int userId);



};