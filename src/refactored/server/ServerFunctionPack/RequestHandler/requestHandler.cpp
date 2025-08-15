#include "include/requestHandler.hpp"
#include "RequestClassByteCodes.hpp"  
#include "ChunkTransmission.hpp"
#include "InterThreadCommunicationCodes.hpp"
#include "InterThreadCommunication.hpp"
#include "DataBaseCommunicationCodes.hpp" 
#include <iostream>

using std::cout;


RequestHandler::RequestHandler(int clientSocket_,DBCommunication *ptr,std::mutex* mtx_){
    clientSocket =  clientSocket_;
    threadId =clientSocket;
    dbCommunication = ptr;
    mtx= mtx_;
};

void RequestHandler::createNewOrder(request& request){
    int clientID = request.getNumericData()[0];
    int type = request.getNumericData()[1];
    int  side = request.getNumericData()[2];
    int quantity =  request.getNumericData()[3];
    int dollars = request.getNumericData()[4];
    int cents = request.getNumericData()[5];

    
    DBrequest newOrderRequest{threadId,OB_NEW_ORDER,{},{clientID,type,side,quantity,dollars,cents}};
    mtx->lock();
    dbCommunication->addNewRequest(newOrderRequest);
    mtx->unlock();

    
    DBresponse response = dbCommunication->waitResponse(threadId);
    constructInfoRequest(response.textData[0].c_str());
};

void RequestHandler::handleInput(){
    vector<uint8_t> serializedRequest;
    while(connectedToClient){
        

        receiveChunks(clientSocket,serializedRequest);
        if(!std::size(serializedRequest))
            continue;
            
        request requestReceived;
        requestReceived.deserialize(serializedRequest);

        switch(requestReceived.getCommand()){
            case RequestCommand::END_STREAM:
                endConnection();
                break;
            case RequestCommand::MSG:
                cout << requestReceived.getTextData()[0] << std::endl;
                break;
            case RequestCommand::REGISTER:
                registerNewUser(requestReceived);
                break;
            case RequestCommand::LOGIN:
                loginUser(requestReceived);
                break;
            case RequestCommand::BALANCE:
                getBalanceUser(requestReceived);
                break;
            case RequestCommand::STOCKS:
                getStocksUser(requestReceived);
            case RequestCommand::ORDERBOOK_NEW_ORDER:
                createNewOrder(requestReceived);
                break;
        }
        serializedRequest.clear();
    }
}

void RequestHandler::endConnection(){
    connectedToClient = false;
}

void RequestHandler::registerNewUser(request& registerRequest){
    const char* username = registerRequest.getTextData()[0];
    const char* password = registerRequest.getTextData()[1];
    //replace with our new structure
    DBrequest newUser{threadId,DB_REGISTER,{username,password},{}};
    std::mutex mtx;
    mtx.lock();
    dbCommunication->addNewRequest(newUser);
    mtx.unlock();
    DBresponse requestResponse = dbCommunication->waitResponse(threadId);
    int code = requestResponse.errorCode;

    if(code==REGISTER_SUCCESS){
        constructInfoRequest("User created successfully!");
    }else{
        constructInfoRequest("Please enter a different username User already exists");

    }
};

void RequestHandler::loginUser(request &loginRequest){
    const char* username = loginRequest.getTextData()[0];
    const char* password = loginRequest.getTextData()[1];
    //replace with our new structure
    DBrequest loginUser{threadId,DB_LOGIN,{username,password},{}};
    std::mutex mtx;
    mtx.lock();
    dbCommunication->addNewRequest(loginUser);
    mtx.unlock();

    mtx.lock();
    DBresponse requestResponse = dbCommunication->waitResponse(threadId);
    mtx.unlock();

    int code = requestResponse.errorCode;

    if(code!=USER_NOT_FOUND){
        int userId = requestResponse.numericData[0];
        constructIdRequest(userId);
    }else{
        constructInfoRequest("User not found: please re-enter password");

    }
}

void RequestHandler::constructInfoRequest(const char* msg){
    request infoRequest(RequestCommand::MSG,{msg},{});
    sendChunk(clientSocket,infoRequest.serialize());
};


void  RequestHandler::constructIdRequest(int id){
    request infoRequest(RequestCommand::LOGIN,{},{id});
    sendChunk(clientSocket,infoRequest.serialize());
};


void RequestHandler::constructBalanceRequest(int dollars,int cents){
    request infoRequest(RequestCommand::BALANCE,{},{dollars,cents});
    sendChunk(clientSocket,infoRequest.serialize());
};

void RequestHandler::getBalanceUser(request& request){
    int userId  = request.getNumericData()[0];
    if(userId==UNVALID_ID){
        constructInfoRequest("Wrong ID");
        return;

    }
    DBrequest getBalance{threadId,DB_RETRIEVE_USER_BALANCE,{},{userId}};
    std::mutex mtx;
    mtx.lock();
    dbCommunication->addNewRequest(getBalance);
    mtx.unlock();

    mtx.lock();
    DBresponse requestResponse = dbCommunication->waitResponse(threadId);
    mtx.unlock();

    int BalanceDollars= requestResponse.numericData[0];
    int BalanceCents = requestResponse.numericData[1];

    constructBalanceRequest(BalanceDollars,BalanceCents);
};


void RequestHandler::getStocksUser(request& stockCheckup){
    int userId  = stockCheckup.getNumericData()[0];
    if(userId==UNVALID_ID){
        constructInfoRequest("Wrong ID");
        return;

    }
    DBrequest getStockHolding{threadId,DB_RETRIEVE_USER_STOCKS,{},{userId}};
    std::mutex mtx;
    mtx.lock();
    dbCommunication->addNewRequest(getStockHolding);
    mtx.unlock();

    mtx.lock();
    DBresponse requestResponse = dbCommunication->waitResponse(threadId);
    mtx.unlock();

    const int stockAmount = requestResponse.numericData[0];
    
    constexpr int empty_field = 0;
    constructBalanceRequest(stockAmount,empty_field);
};
