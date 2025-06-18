#pragma once
#include <utility>
#include <sqlite3.h>

class DataBase{
    sqlite3 *db=nullptr;
    public:
        void createDB(const char *);
        //functions for the login functionality 
        int registerUser(const char* username, const char* password);
        int getUserId(const char* username, const char* password);

        //functions for updating the user account balance
        int getUserBalance(int userId);



};