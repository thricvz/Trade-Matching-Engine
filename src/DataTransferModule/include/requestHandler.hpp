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
class RequestHandler{
    bool exitDemanded;
    int clientSocket;
    private:
        //methods for cunstructing the different types of requests
        void constructExitRequest();
        void constructUnkownRequest();
        void constructMessageRequest();
    public: 
        RequestHandler(int clientSocket_){ 
            exitDemanded = false;
            clientSocket = clientSocket_;
        };
        
        //methods for cunstructing the different types of requests
        void handleInput();   
};