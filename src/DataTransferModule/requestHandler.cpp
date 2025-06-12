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

//requesthandler for server
RequestHandler::RequestHandler(int clientSocket_){
    clientSocket =  clientSocket_;
};

void RequestHandler::handleInput(){
    vector<uint8_t> serializedRequest;
    while(true){
        receiveChunks(clientSocket,serializedRequest);
        request requestReceived;
        requestReceived.deserialize(serializedRequest);
        //decide which action to perform according to the data
        //quick implementation
        
        if(requestReceived.getMessageCommand() == END_STREAM){
            break;
        }else if(requestReceived.getMessageCommand() == MSG){
            std::cout << requestReceived.getTextArgs()[0] << std::endl;
        }
        //clean the serializedRequest
        serializedRequest.clear();
    }
}