#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

int main(){
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket,(struct sockaddr*)&serverAddress,sizeof(serverAddress));

    listen(serverSocket,1);
    int clientSocket = accept(serverSocket,nullptr,nullptr);
    char buffer[128];
    while(true){
        //read the messages and print them
        recv(clientSocket,&buffer,sizeof(buffer),0);
        std::cout << buffer << std::endl;
    }
}