#pragma once 
#include <mutex>
#include <map>
#include <string>
#include <iostream>
#include "requestClass.h"
#include "ChunkTransmission.h"
#include "byteCodes.h"
#include "DataBase.hpp"
#include "DataBaseCommunicationCodes.hpp"

using std::string;
using std::map;

struct DATABASE{
    int x;
};

//meant for the client side
class RequestSender{
    bool exitDemanded=false;
    int clientSocket;
    private:
        //methods for cunstructing the different types of requests
        void constructExitRequest();
        void constructUnkownRequest();
        void constructMessageRequest();
        void constructLoginRequest();
    public: 
        RequestSender(int clientSocket_);
        
        //methods for cunstructing the different types of requests
        void handleInput();   
};


//for the use of the server

class RequestHandler{
    std::mutex *mtx;
    DataBase *db;

    int clientSocket;
    bool connectedToClient = true;
    //future lock variable that manages actions on the server with a lock for the threads
    vector<uint8_t> getSerializedRequest();
    //
    void endConnection();
    void registerNewUser(request& registerRequest);

    //communication for the client
    void constructInfoRequest(const char* msg);
    public:
        RequestHandler(int clientSocket_,DataBase* DBptr,std::mutex *mutex);
        void handleInput();


};