#pragma once 
#include <mutex>
#include <map>
#include <string>
#include <iostream>
#include "requestClass.h"
#include "ChunkTransmission.h"
#include "byteCodes.h"
#include "DataBaseCommunication.hpp"
#include "DataBaseCommunicationCodes.hpp"



using std::string;
using std::map;


const int UNVALID_ID =-1;

class RequestSender{
    bool exitDemanded=false;
    int clientSocket{};
    int clientID{UNVALID_ID};
    private:
        //methods for cunstructing the different types of requests
        void constructExitRequest();
        void constructUnkownRequest();
        void constructMessageRequest();
        void constructLoginRequest();
        void constructRegisterRequest();
        void constructBalanceRequest();
        public: 
        RequestSender(int clientSocket_);
        
        //methods for constructing the different types of requests
        void handleInput();   
};
    
    
    //for the use of the server
    
    class RequestHandler{
        
        int clientSocket;
        int threadId;
        bool connectedToClient = true;
        std::mutex * mtx;
        DBCommunication * dbCommunication;

        vector<uint8_t> getSerializedRequest();
        void endConnection();
        void registerNewUser(request& registerRequest);
        void loginUser(request& loginRequest);
        void getBalanceUser(request& balanceCheckUp);
        //communication for the client
        void constructInfoRequest(const char* msg);
        void constructIdRequest(int id);
        void constructBalanceRequest(int dollars,int cents);
        public:
        RequestHandler(int clientSocket_,DBCommunication* ptr,std::mutex* mtx);
            void handleInput();


};