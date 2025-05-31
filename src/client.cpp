#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>


int main(){
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);    

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
    char buffer[128];
    while(true){
        //read the messages and print them
        std::cout <<"enter message : ";
        std::cin >> buffer;
        send(clientSocket,&buffer, sizeof(buffer),0);

        if(strcmp(buffer,"exit")==0){
            shutdown(clientSocket,SHUT_RDWR);
            close(clientSocket);
            break;
        }
    }
}