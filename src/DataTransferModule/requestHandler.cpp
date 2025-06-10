#include "requestHandler.hpp"



void RequestHandler::handleInput(){
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


void RequestHandler::constructExitRequest(){
    exitDemanded = true;
}

void RequestHandler::constructUnkownRequest(){
    std::cout << "The request you entered is unkown\n";
};


void RequestHandler::constructMessageRequest(){
    vector<uint8_t> values = {0x65,0x75,0x65,0x75,0x65,0x75};
    sendChunk(clientSocket,values);
};