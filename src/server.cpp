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

bool balanceSufficient(DataBase &db,Order *order,int userId){
    std::pair<int,int> userBalance = db.getUserBalance(userId);
    
    if(order->side==OrderSide::BUY){
        int balanceWorthCents = userBalance.first *100 + userBalance.second;
        int orderWorthCents =  (order->price.dollars*100 + order->price.cents)*order->quantity;
        
        if(orderWorthCents > balanceWorthCents){
            delete order;
            return false;
        }

    }else if(order->side==OrderSide::SELL){
        int userStockHolding = db.getUserStockHolding(userId);
        if(userStockHolding > order->quantity){
            delete order;
            return false;
        }
    }

    return true;

}
void handleNewOrder(OrderBook& orderbook,DataBase& db,DBCommunication &dbCommunication, DBrequest&request){
    int clientID = request.numericArgs[0];
    OrderType type = static_cast<OrderType>(request.numericArgs[1]);
    OrderSide side = static_cast<OrderSide>(request.numericArgs[2]);
    int quantity =  request.numericArgs[3];
    int dollars = request.numericArgs[4];
    int cents = request.numericArgs[5];

    Order *newOrder =new  Order(type,side,quantity,dollars,cents);
    newOrder->setId(clientID); 

    

    if(balanceSufficient(db,newOrder,clientID)){
        std::pair<MatchesList,OrderFillState> matchResult = orderbook.addOrder(newOrder);
        std::cout << "currently handling a order";
        
        DBresponse serverResponse{{"order created successfully"},{},CONNECT_SUCCESS};

        mtx.lock();
        dbCommunication.addResponse(request.threadId,serverResponse);
        mtx.unlock();

    }else{
        DBresponse serverResponse{{"Failed to put order: check your account balance"},{},CONNECT_SUCCESS};

        mtx.lock();
        dbCommunication.addResponse(request.threadId,serverResponse);
        mtx.unlock();
        
    };




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
                handleNewOrder(orderbook,db,dbCommunication,request.value());
                break;
        }
    }
}

void handleConnection(int clientSocket,DBCommunication & dbCommunication){
    std::cout <<"new client\n";
    
    RequestHandler requestHandler(clientSocket,&dbCommunication,&mtx);
    requestHandler.handleInput();
    shutdown(clientSocket,SHUT_RD);
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