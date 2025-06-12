#include "requestHandler.hpp"
#include <unistd.h>

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
        }else{
            //uppon no match default message
            constructUnkownRequest();
        }    
    }
};   


void RequestSender::constructExitRequest(){
    exitDemanded = true;
    request endRequest(END_STREAM,0,{},{});
    vector<uint8_t> serializedData=endRequest.serialize();

    sendChunk(clientSocket,serializedData);
}

void RequestSender::constructUnkownRequest(){
    std::cout << "The request you entered is unkown\n";
};


void RequestSender::constructMessageRequest(){
    char message[MAX_MSG_LENGTH];
    std::cout << "Please enter your message: ";
    std::cin >> message;
    
    request msgRequest(0,0,{message});
    vector<uint8_t> serializedData=msgRequest.serialize();

    sendChunk(clientSocket,serializedData);
};

//requesthandler for server
RequestHandler::RequestHandler(int clientSocket_){
    clientSocket =  clientSocket_;
};

void RequestHandler::handleInput(){
    request requestReceived;
    vector<uint8_t> serializedRequest;
    while(true){
        receiveChunks(clientSocket,serializedRequest);
        requestReceived.deserialize(serializedRequest);
        //decide which action to perform according to the data
        //quick implementation
        
        if(requestReceived.getMessageCommand() == END_STREAM){
            break;
        }
        

    }
}