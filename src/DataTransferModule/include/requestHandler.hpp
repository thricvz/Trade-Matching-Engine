#pragma once 
#include "requestClass.h"
#include "ChunkTransmission.h"
#include "byteCodes.h"
#include <map>
#include <string>
#include <iostream>
using std::string;
using std::map;


//meant for the client side
class RequestSender{
    bool exitDemanded=false;
    int clientSocket;
    private:
        //methods for cunstructing the different types of requests
        void constructExitRequest();
        void constructUnkownRequest();
        void constructMessageRequest();
    public: 
        RequestSender(int clientSocket_);
        
        //methods for cunstructing the different types of requests
        void handleInput();   
};


//for the use of the server

class RequestHandler{
    int clientSocket;
    //future lock variable that manages actions on the server with a lock for the threads
    vector<uint8_t> getSerializedRequest();
    public:
        RequestHandler(int clientSocket_);
        void handleInput();


};