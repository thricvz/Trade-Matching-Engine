#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <cstring>
#include <vector>
#include <mutex>
#include <functional>
#include "byteCodes.h"  
#include "ChunkTransmission.h"
#include "requestClass.h"
#include "requestHandler.hpp" 
#include "DataBase.hpp"
#include "DataBaseCommunication.hpp"

const int MAX_CONNECTIONS=3;
//something for handling the messages 
DBCommunication DataBaseCommunicationHandler;
std::mutex mtx;

void dbThread(DBCommunication& dbCommunication ){
    DataBase db("DATABASE.db");
    while(true){
        mtx.lock();
        DBrequest request =  dbCommunication.getRequest();
        mtx.unlock();
        if(request.requestType == 200){
            int code = db.registerUser(request.textArgs[0],request.textArgs[0]);
            DBresponse response{{},{},code};
            mtx.lock();
            dbCommunication.addResponse(request.threadId,response);
            mtx.unlock();
            
        }
    }
}

void handleConnection(int clientSocket,DBCommunication & dbCommunication){
    std::cout <<"new client\n";
    
    RequestHandler requestHandler(clientSocket,&dbCommunication,&mtx);
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


    //start Db thread
    std::thread DB = std::thread(dbThread,std::ref(DataBaseCommunicationHandler));
    DB.detach();
    while(true){
        int clientConnection = accept(serverSocket,nullptr,nullptr);
        concurrentConnections.push_back(std::thread(handleConnection,clientConnection,std::ref(DataBaseCommunicationHandler)));
        
    }
}