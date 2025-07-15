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
    
    if(order->type != OrderType::MARKET && order->side==OrderSide::BUY){
        int balanceWorthCents = userBalance.first *100 + userBalance.second;
        int orderWorthCents =  (order->price.dollars*100 + order->price.cents)*order->quantity;
        
        if(orderWorthCents > balanceWorthCents){
            delete order;
            return false;
        }

    }else if(order->side==OrderSide::SELL){
        int userStockHolding = db.getUserStockHolding(userId);
        if(userStockHolding < order->quantity){
            delete order;
            return false;
        }
    }

    return true;

}


void updateUserEntry(DataBase& db,OrderMatch orderMatch,OrderSide side){
    int clientID = orderMatch.ownerId;
    
    const std::pair<int,int> userBalance = db.getUserBalance(clientID);
    int userStockHolding = db.getUserStockHolding(clientID);

    int balanceWorthCents = userBalance.first *100 + userBalance.second;
    const int orderWorthCents = (orderMatch.price.dollars* 100 + orderMatch.price.cents)*orderMatch.quantity;
    

    if(side == OrderSide::BUY){
        balanceWorthCents -= orderWorthCents;
        userStockHolding += orderMatch.quantity;    
    }else if(side==OrderSide::SELL){
        balanceWorthCents += orderWorthCents;
        userStockHolding -= orderMatch.quantity;
    }

    const int balanceAmountDollars = balanceWorthCents/100;
    const int balanceAmountCents =  balanceWorthCents%100;

    db.setUserBalance(clientID,balanceAmountDollars,balanceAmountCents);
    db.setUserStockHolding(clientID,userStockHolding);
};

void registerOrderChanges(DataBase& db,std::pair<MatchesList,OrderFillState> matchResult,Order* order){
    int matchTotalWorth{ };
    int matchTotalQuantity{ };

    auto matches = matchResult.first.matches ;
    auto fillstate = matchResult.second;
    auto matchedOrderSide = (order->side == OrderSide::BUY) ? OrderSide::SELL : OrderSide::BUY; 

    for (auto orderMatched : matches){
        std::cout << "A MATCH WAS FOUND \n";
        const int worthOrder = (orderMatched.price.dollars*100+ orderMatched.price.cents) * orderMatched.quantity; 
        matchTotalWorth += worthOrder;
        matchTotalQuantity += orderMatched.quantity;
        updateUserEntry(db,orderMatched,matchedOrderSide);
    }

    if(fillstate!=OrderFillState::NOFILL){
        OrderMatch originalOrder(
            order->ownerID,
            order->id,
            matchTotalQuantity,
            {matchTotalWorth/100,matchTotalWorth%100},
            fillstate
        );
        updateUserEntry(db,originalOrder,order->side);
    }

};

void handleNewOrder(OrderBook& orderbook,DataBase& db,DBCommunication &dbCommunication, DBrequest&request){
    int clientID = request.numericArgs[0];
    OrderType type = static_cast<OrderType>(request.numericArgs[1]);
    OrderSide side = static_cast<OrderSide>(request.numericArgs[2]);
    int quantity =  request.numericArgs[3];
    int dollars = request.numericArgs[4];
    int cents = request.numericArgs[5];

    Order *newOrder =new  Order(type,side,quantity,dollars,cents);
    newOrder->setId(OrderBook::genOrderId()); 
    newOrder->setOwnerId(clientID);
    

    DBresponse serverResponse;

    if(balanceSufficient(db,newOrder,clientID)){
        //updateAccountBalance(db,newOrder,clientID);
        std::pair<MatchesList,OrderFillState> matchResult = orderbook.addOrder(newOrder);
        std::cout << "currently handling a order";
        
        serverResponse = {{"order created successfully"},{},CONNECT_SUCCESS};
        
        registerOrderChanges(db,matchResult,newOrder);

    }else{
        serverResponse = {{"Failed to put order: check your account balance"},{},CONNECT_SUCCESS};
    }
    
    mtx.lock();
    dbCommunication.addResponse(request.threadId,serverResponse);
    mtx.unlock();

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