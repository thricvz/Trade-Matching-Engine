#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <cstring>
#include <vector>
#include "requestClass.h"
#include "byteCodes.h"  
#include "ChunkTransmission.h"
#include "requestHandler.hpp"

const int MAX_CONNECTIONS=3;

//something for handling the messages 
void handleConnection(int clientSocket){
    std::cout <<"new client\n";
    
    RequestHandler requestHandler(clientSocket);
    requestHandler.handleInput();
    shutdown(clientSocket,2);
};

int main(){
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket,(struct sockaddr*)&serverAddress,sizeof(serverAddress));
    listen(serverSocket,MAX_CONNECTIONS);

    std::vector<int> clientConnections(0);
    std::vector<std::thread> concurrentConnections(0);

    while(true){
        int clientConnection = accept(serverSocket,nullptr,nullptr);
        concurrentConnections.push_back(std::thread(handleConnection,clientConnection));
        
    }
}