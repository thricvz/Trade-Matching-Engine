#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include "ChunkTransmission.h"
#include "requestHandler.hpp"

int main(){
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);    

    //own datastructures for handling the advice
    RequestSender RequestHandler(clientSocket);    
    //the server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    //loops that ensures that it connects to the server
    int connectionSuccess = -1;
    
    connectionSuccess = connect(clientSocket,(struct sockaddr*)&serverAddress,sizeof(serverAddress));
    if(connectionSuccess==-1)
        std::exit(0);

    //main loop
    std::cout << "Connected to the server!" << std::endl;
    std::cout << connectionSuccess ;

    std::vector<uint8_t> values = {0x65,0x75,0x65,0x75,0x65,0x75};
    

    RequestHandler.handleInput();
    std::exit(0);
}