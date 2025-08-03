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
#include <functional>

using std::string;
using std::map;



const int UNVALID_ID =-1;

class RequestSender{

    public: 
        RequestSender(int clientSocket_);
        
        //methods for constructing the different types of requests
        void handleInput();   
    private:
        bool exitDemanded=false;
        int clientSocket{};
        int clientID{UNVALID_ID};


        void constructExitRequest();
        void constructUnkownRequest();
        void constructMessageRequest();
        void constructLoginRequest();
        void constructRegisterRequest();
        void constructBalanceRequest();
        void constructNewOrderRequest();
        void constructStocksRequest();
        void constructHelpCommand();

        using requestFunction = void (RequestSender::*)();

        static const inline std::map<std::string_view,requestFunction> requestOptions{
            {"exit",&RequestSender::constructExitRequest},
            {"message",&RequestSender::constructMessageRequest},
            {"login",&RequestSender::constructLoginRequest},
            {"register",&RequestSender::constructRegisterRequest},
            {"stocks",&RequestSender::constructStocksRequest},
            {"balance",&RequestSender::constructBalanceRequest},
            {"order",&RequestSender::constructStocksRequest},
            {"help",&RequestSender::constructHelpCommand}
        };
};

    
    //for the use of the server
    
    class RequestHandler{
        public:
        RequestHandler(int clientSocket_,DBCommunication* ptr,std::mutex* mtx);
        void handleInput();
        
        private:
            int clientSocket;
            int threadId;
            bool connectedToClient = true;
            std::mutex * mtx;
            DBCommunication * dbCommunication;
            
            vector<uint8_t> getSerializedRequest();
            
            void endConnection();
            void registerNewUser(request& registerRequest);
            void constructBalanceRequest(int dollars,int cents);
            void loginUser(request& loginRequest);
            void getBalanceUser(request& balanceCheckUp);
            void getStocksUser(request& stockCheckup);
            void createNewOrder(request& request);
            //communication for the client
            void constructInfoRequest(const char* msg);
            void constructIdRequest(int id);


};