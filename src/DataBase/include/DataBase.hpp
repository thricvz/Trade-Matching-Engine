#pragma once
#include <utility>
#include <sqlite3.h>
#include <string>

using std::string;
const int CONNECT_SUCCESS=0x99;
const int CONNECT_FAILED=0x69;

class DataBase{
    sqlite3 *db=nullptr;
    string filePath;

    //methods for the constructor
    void createDB();
    int connect();
    void loadScript(string scriptName,string &requestBuffer);
    public:
        DataBase(const char *PATH);
        ~DataBase();

        //
        //functions for the login functionality 
        int registerUser(const char* username, const char* password);
        int getUserId(const char* username, const char* password);

        //functions for updating the user account balance
        std::pair<int,int> getUserBalance(int userId);



};