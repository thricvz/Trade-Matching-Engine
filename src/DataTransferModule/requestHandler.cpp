#include "requestHandler.hpp"
#include <unistd.h>
#include <stdio.h>
#include "DataBaseCommunication.hpp"
#include "DBCommunicationModuleCodes.hpp"

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

    std::cout << "you have " << dollars<< " dollars and " << cents << "in your account" << std::endl;
}
//requesthandler for server
RequestHandler::RequestHandler(int clientSocket_,DBCommunication *ptr,std::mutex* mtx_){
    clientSocket =  clientSocket_;
    threadId =clientSocket;
    dbCommunication = ptr;
    mtx= mtx_;
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
