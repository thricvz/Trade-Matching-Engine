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
#include "DBCommunicationModuleCodes.hpp"
#include "orderbook.h"

const int MAX_CONNECTIONS=3;
//something for handling the messages 
DBCommunication DataBaseCommunicationHandler;
std::mutex mtx;

void handleRegistration(DataBase& db,DBCommunication &dbCommunication, DBrequest&request){
    int code = db.registerUser(request.textArgs[0],request.textArgs[1]);
    DBresponse response{{},{},code};
    mtx.lock();
    dbCommunication.addResponse(request.threadId,response);
    mtx.unlock();
}


void handleLogin(DataBase& db,DBCommunication &dbCommunication, DBrequest&request){

    int userId= db.getUserId(request.textArgs[0],request.textArgs[1]);
    DBresponse response{{},{userId},userId};
    mtx.lock();
    dbCommunication.addResponse(request.threadId,response);
    mtx.unlock();
}

void handleRetrieveBalanceData(DataBase& db,DBCommunication &dbCommunication, DBrequest&request){
    int userId= request.numericArgs[0];
    std::pair<int,int> userBalance= db.getUserBalance(userId);

    DBresponse response{{},{userBalance.first,userBalance.second}};
    mtx.lock();
    dbCommunication.addResponse(request.threadId,response);
    mtx.unlock();
};

void handleNewOrder(OrderBook& orderbook,DBCommunication &dbCommunication, DBrequest&request){
    OrderType type = static_cast<OrderType>(request.numericArgs[0]);
    OrderSide side = static_cast<OrderSide>(request.numericArgs[1]);
    int quantity =  request.numericArgs[2];
    int dollars = request.numericArgs[3];
    int cents = request.numericArgs[4];

    Order *newOrder =new  Order(type,side,quantity,dollars,cents);
    std::pair<MatchesList,OrderFillState> matchResult = orderbook.addOrder(newOrder);

    //send notifications for each user which order has been matched

    std::cout << "currently handling a order";


}
void dbThread(DBCommunication& dbCommunication ){
    DataBase db("DATABASE.db");
    Fifo algorithm;
    OrderBook orderbook(&algorithm);

    while(true){
        mtx.lock();
        std::optional<DBrequest> request =  dbCommunication.getRequest();
        mtx.unlock();
        //if no request repeat process
        if(!request.has_value()){
            continue;
        }

        switch(request.value().requestType){
            case DB_REGISTER:
                handleRegistration(db,dbCommunication,request.value());
                break;
            case DB_LOGIN:
                handleLogin(db,dbCommunication,request.value());
                break;
            case DB_RETRIEVE_USER_BALANCE:
                handleRetrieveBalanceData(db,dbCommunication,request.value());
                break;
            case OB_NEW_ORDER:
                handleNewOrder(orderbook,dbCommunication,request.value());
                break;
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


    //start DataBase thread
    std::thread DB = std::thread(dbThread,std::ref(DataBaseCommunicationHandler));
    DB.detach();
    while(true){
        int clientConnection = accept(serverSocket,nullptr,nullptr);
        concurrentConnections.push_back(std::thread(handleConnection,clientConnection,std::ref(DataBaseCommunicationHandler)));
        
    }
}