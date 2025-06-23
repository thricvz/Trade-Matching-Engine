#include "requestHandler.hpp"
#include <unistd.h>
#include <stdio.h>

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
        }else if(request=="Login"){
            constructLoginRequest();
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
    std::cout << serverFeedbackRequest.getTextArgs()[0];
};
//requesthandler for server
RequestHandler::RequestHandler(int clientSocket_,DataBase* DBptr,std::mutex* mutex){
    clientSocket =  clientSocket_;
    db = db;
    mtx = mutex;
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
            case LOGIN:
                registerNewUser(requestReceived);
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
    
    mtx->lock();
    int code =  db->registerUser(username,password);
    mtx->unlock();

    if(code==REGISTER_SUCCESS){
        constructInfoRequest("User created successfully!");
    }else{
        constructInfoRequest("Please enter a different username User already exists");

    }
};


void RequestHandler::constructInfoRequest(const char* msg){
    request infoRequest(COMMUNICATION,MSG,{msg});
    sendChunk(clientSocket,infoRequest.serialize());
};
