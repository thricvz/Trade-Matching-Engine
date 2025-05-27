#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

int main(){
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);    

    //the server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    connect(clientSocket,(struct sockaddr*)&serverAddress,sizeof(serverAddress));
    char buffer[128];
    while(true){
        //read the messages and print them
        std::cin >> buffer;
        send(clientSocket,&buffer, sizeof(buffer),0);
        if(strcmp(buffer,"exit")==0){
            shutdown(clientSocket,SHUT_RDWR);
            close(clientSocket);
            break;
        }
    }
}