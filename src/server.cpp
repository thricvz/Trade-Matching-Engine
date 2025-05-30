#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <vector>



void handleConnection(int clientSocket){
    char buffer[128];
    while(true){
        recv(clientSocket,&buffer,sizeof(buffer),0);
        std::cout << buffer << std::endl;
    }
};

int main(){
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket,(struct sockaddr*)&serverAddress,sizeof(serverAddress));
    listen(serverSocket,2);

    std::vector<int> clientConnections(0);
    std::vector<std::thread> concurrentConnections(0);


    while(true){
        int clientConnection = accept(serverSocket,nullptr,nullptr);
        if(clientConnection != -1){
            std::cout<<"connected client";
            concurrentConnections.push_back(std::thread(handleConnection,clientConnection));
        }else{
            std::cout << "waiting for connection" <<std::endl;
        }
    }
}