#include "requestHandler.hpp"
#include <unistd.h>
#include <stdio.h>
#include "DataBaseCommunication.hpp"
#include "DBCommunicationModuleCodes.hpp"
#include "orderTypes.h"
#include <optional> 
RequestSender::RequestSender(int clientSocket_){
    clientSocket =  clientSocket_;
};

void RequestSender::handleInput(){
    string request;
    while(!exitDemanded){
        std::cin >> request;

        if(request=="exit"){
            constructExitRequest();

        }else if(request=="msg"){
            constructMessageRequest();
        }else if(request=="login"){
            constructLoginRequest();

        }else if(request=="register"){
            constructRegisterRequest();
        }else if(request=="balance"){
            constructBalanceRequest();
        }else if(request=="order"){
            constructNewOrderRequest();
        }else{
            //uppon no match default message
            constructUnkownRequest();
        }    
    }
};   


void RequestSender::constructExitRequest(){
    exitDemanded = true;
    request endRequest(COMMUNICATION,END_STREAM,{},{});
    vector<uint8_t> serializedData=endRequest.serialize();

    sendChunk(clientSocket,serializedData);
}

std::optional<OrderType> getOrderType(std::string_view string){
    if(string=="market"){
        return std::optional<OrderType>(OrderType::MARKET);

    }else if(string=="limit"){
        return std::optional<OrderType>(OrderType::LIMIT);
    }
    return std::optional<OrderType>(std::nullopt);

}


std::optional<OrderSide> getOrderSide(std::string_view string){
    if(string=="buy"){
        return std::optional<OrderSide>(OrderSide::BUY);

    }else if(string=="sell"){
        return std::optional<OrderSide>(OrderSide::SELL);
    }
    return std::optional<OrderSide>(std::nullopt);

}

void getOrderResultMessage(int clientSocket){
    request serverFeedbackRequest;
    vector<uint8_t> serverFeedbackByteStream;
    receiveChunks(clientSocket,serverFeedbackByteStream);
    serverFeedbackRequest.deserialize(serverFeedbackByteStream);
    std::cout << serverFeedbackRequest.getTextArgs()[0];
}
void RequestSender::constructNewOrderRequest(){
    std::string userInput{};
    
    if(clientID == UNVALID_ID){
        std::cout <<"please login first before putting an order" << std::endl;
        return ;
    }
    
    std::cout << "Enter order type( limit or market ):";
    std::cin >> userInput;
    std::optional<OrderType> orderType {getOrderType(userInput)};
    
    if(!orderType.has_value()){
        std::cout <<"Ordertype not found please provide a valid ordertype" << std::endl;
        return;
    }
    //order price
    int dollars{};
    int cents{};
    if(orderType.value()==OrderType::LIMIT){
        std::cout << "Enter the desired price in dollars";
        std::cin >> userInput;   
        dollars = stoi(userInput);

        std::cout << "Enter the desired price in cents";
        std::cin >> userInput;   
        cents = stoi(userInput);

        if(dollars <=0 || cents <= 0){
            std::cout << "Please enter a valid price";
            return;
        }
    }
    std::cout << "Enter order side (buy or sell) :";
    std::cin >> userInput;
    std::optional<OrderSide> orderSide {getOrderSide(userInput)};
    if(!orderType.has_value()){
        std::cout <<" not found please provide a valid order side";
        return;
    }

    std::cout << "Enter the quanity : ";
    std::cin >> userInput;
    int quantity{stoi(userInput)};
    if(quantity<=0){
        std::cout << "Please enter a valid quantity";
        return;
    }

    request newOrderRequest(ORDERBOOK_NEW_ORDER,ORDERBOOK_NEW_ORDER,{},{clientID, orderType.value(),orderSide.value(),quantity,dollars,cents});
    vector<uint8_t> serializedData=newOrderRequest.serialize();

    sendChunk(clientSocket,serializedData);


    getOrderResultMessage(clientSocket);
};

void RequestSender::constructUnkownRequest(){
    std::cout << "UNKNOWN COMMAND\n";
};


void RequestSender::constructMessageRequest(){
    char message[MAX_MSG_LENGTH];
    std::cout << "Please enter your message: ";
    
    std::cin.ignore();
    std::cin.getline(message,MAX_MSG_LENGTH-1);  
    std::cout << "\n";

    request msgRequest(COMMUNICATION,MSG,{message});
    vector<uint8_t> serializedData=msgRequest.serialize();

    sendChunk(clientSocket,serializedData);
};

void RequestSender::constructLoginRequest(){
    char username[MAX_CREDENTIALS_LENGTH];
    char password[MAX_CREDENTIALS_LENGTH];

    std::cout << "New username(" << MAX_CREDENTIALS_LENGTH << "  characters at most): " ;
    std::cin >> username;

    std::cout << "Enter your password" << MAX_CREDENTIALS_LENGTH << " characters at most): " ;
    std::cin >> password;

    request loginRequest(COMMUNICATION,LOGIN,{username,password},{});
    vector<uint8_t> serializedData=loginRequest.serialize();

    sendChunk(clientSocket,serializedData);

    //return feedback from server
    request serverFeedbackRequest;
    vector<uint8_t> serverFeedbackByteStream;
    receiveChunks(clientSocket,serverFeedbackByteStream);
    serverFeedbackRequest.deserialize(serverFeedbackByteStream);
    
    if(serverFeedbackRequest.getMessageCommand()==LOGIN){
        clientID = serverFeedbackRequest.getNumericArgs()[0];
        std::cout << clientID;
        return;
    }
    std::cout << serverFeedbackRequest.getTextArgs()[0];
};

void RequestSender::constructRegisterRequest(){
    char username[MAX_CREDENTIALS_LENGTH];
    char password[MAX_CREDENTIALS_LENGTH];

    std::cout << "New username(" << MAX_CREDENTIALS_LENGTH << "  characters at most): " ;
    std::cin >> username;

    std::cout << "Enter your password" << MAX_CREDENTIALS_LENGTH << " characters at most): " ;
    std::cin >> password;

    request loginRequest(COMMUNICATION,REGISTER,{username,password},{});
    vector<uint8_t> serializedData=loginRequest.serialize();

    sendChunk(clientSocket,serializedData);

    //return feedback from server
    request serverFeedbackRequest;
    vector<uint8_t> serverFeedbackByteStream;
    receiveChunks(clientSocket,serverFeedbackByteStream);
    serverFeedbackRequest.deserialize(serverFeedbackByteStream);
    std::cout << serverFeedbackRequest.getTextArgs()[0];
    
}

void RequestSender::constructBalanceRequest(){
    request getBalanceRequest(COMMUNICATION,BALANCE,{},{clientID});
    vector<uint8_t> serializedData=getBalanceRequest.serialize();
    sendChunk(clientSocket,serializedData);


    request serverFeedbackRequest;
    vector<uint8_t> serverFeedbackByteStream;
    receiveChunks(clientSocket,serverFeedbackByteStream);
    serverFeedbackRequest.deserialize(serverFeedbackByteStream);
    int dollars =  serverFeedbackRequest.getNumericArgs()[0];
    int cents =  serverFeedbackRequest.getNumericArgs()[1];

    std::cout << "you have " << dollars<< " dollars and " << cents << " cents in your account" << std::endl;
}
//requesthandler for server
RequestHandler::RequestHandler(int clientSocket_,DBCommunication *ptr,std::mutex* mtx_){
    clientSocket =  clientSocket_;
    threadId =clientSocket;
    dbCommunication = ptr;
    mtx= mtx_;
};

void RequestHandler::createNewOrder(request& request){
    int clientID = request.getNumericArgs()[0];
    int type = request.getNumericArgs()[1];
    int  side = request.getNumericArgs()[2];
    int quantity =  request.getNumericArgs()[3];
    int dollars = request.getNumericArgs()[4];
    int cents = request.getNumericArgs()[5];

    
    DBrequest newOrderRequest{threadId,OB_NEW_ORDER,{},{clientID,side,quantity,dollars,cents}};
    mtx->lock();
    dbCommunication->addNewRequest(newOrderRequest);
    mtx->unlock();

    
    DBresponse response = dbCommunication->waitResponse(threadId);
    constructInfoRequest(response.textArgs[0].c_str());
};

void RequestHandler::handleInput(){
    vector<uint8_t> serializedRequest;
    while(connectedToClient){
        

        receiveChunks(clientSocket,serializedRequest);
        request requestReceived;
        requestReceived.deserialize(serializedRequest);

        switch(requestReceived.getMessageCommand()){
            case END_STREAM:
                endConnection();
                break;
            case MSG:
                std::cout << requestReceived.getTextArgs()[0] << std::endl;
                break;
            case REGISTER:
                registerNewUser(requestReceived);
                break;
            case LOGIN:
                loginUser(requestReceived);
                break;
            case BALANCE:
                getBalanceUser(requestReceived);
                break;
            case ORDERBOOK_NEW_ORDER:
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
    const char* username = registerRequest.getTextArgs()[0];
    const char* password = registerRequest.getTextArgs()[1];
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
    const char* username = loginRequest.getTextArgs()[0];
    const char* password = loginRequest.getTextArgs()[1];
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
        int userId = requestResponse.numericArgs[0];
        constructIdRequest(userId);
    }else{
        constructInfoRequest("User not found: please re-enter password");

    }
}

void RequestHandler::constructInfoRequest(const char* msg){
    request infoRequest(COMMUNICATION,MSG,{msg});
    sendChunk(clientSocket,infoRequest.serialize());
};


void  RequestHandler::constructIdRequest(int id){
    request infoRequest(COMMUNICATION,LOGIN,{},{id});
    sendChunk(clientSocket,infoRequest.serialize());
};


void RequestHandler::constructBalanceRequest(int dollars,int cents){
    request infoRequest(COMMUNICATION,BALANCE,{},{dollars,cents});
    sendChunk(clientSocket,infoRequest.serialize());
};

void RequestHandler::getBalanceUser(request& request){
    int userId  = request.getNumericArgs()[0];
    if(userId==UNVALID_ID){
        constructInfoRequest("Fucking wrong id mate");
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

    int BalanceDollars= requestResponse.numericArgs[0];
    int BalanceCents = requestResponse.numericArgs[1];

    constructBalanceRequest(BalanceDollars,BalanceCents);
};
