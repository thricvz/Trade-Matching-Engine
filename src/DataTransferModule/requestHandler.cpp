#include "requestHandler.hpp"
#include <unistd.h>
#include <stdio.h>
#include "DataBaseCommunication.hpp"
#include "DBCommunicationModuleCodes.hpp"
#include "orderTypes.h"
#include <optional> 



using std::string_view;
using std::map;
using std::string;
using std::optional;
using std::cout;
using std::cin;

RequestSender::RequestSender(int clientSocket_){
    clientSocket =  clientSocket_;
};

void RequestSender::handleInput(){
    string request;
    while(!exitDemanded){
        cout << ">> ";
        cin >> request;

        //replace this by a map with functions pointers to each case
        if(request=="exit"){
            constructExitRequest();

        }else if(request=="message"){
            constructMessageRequest();
        }else if(request=="login"){
            constructLoginRequest();

        }else if(request=="register"){
            constructRegisterRequest();
        }else if(request=="balance"){
            constructBalanceRequest();
        }else if(request=="order"){
            constructNewOrderRequest();

        }else if(request=="stocks"){
            constructStocksRequest();
        }else if(request =="help"){
            constructHelpCommand();
        }
        else{
            //uppon no match default message
            constructUnkownRequest();
        }    
    }
};   


void RequestSender::constructExitRequest(){
    exitDemanded = true;
    request endRequest(COMMUNICATION,END_STREAM,{},{});
    vector<uint8_t> serializedData=endRequest.serialize();

    sendChunk(clientSocket,serializedData);
}

std::optional<OrderType> getOrderType(std::string_view string){
    if(string=="market"){
        return std::optional<OrderType>(OrderType::MARKET);

    }else if(string=="limit"){
        return std::optional<OrderType>(OrderType::LIMIT);
    }
    return std::optional<OrderType>(std::nullopt);

}


std::optional<OrderSide> getOrderSide(std::string_view string){
    if(string=="buy"){
        return std::optional<OrderSide>(OrderSide::BUY);

    }else if(string=="sell"){
        return std::optional<OrderSide>(OrderSide::SELL);
    }
    return std::optional<OrderSide>(std::nullopt);

}

void getOrderResultMessage(int clientSocket){
    request serverFeedbackRequest;
    vector<uint8_t> serverFeedbackByteStream;
    receiveChunks(clientSocket,serverFeedbackByteStream);
    serverFeedbackRequest.deserialize(serverFeedbackByteStream);
    cout << serverFeedbackRequest.getTextArgs()[0] << "\n";
}



bool inputInteger(int &returnVariable,string_view prompt,string_view failureMessage,bool requiredStrictlyPositiveInput=false ){
    string input{};
    cout << prompt << " ";
    cin >> input;


    try{
        
        int inputNumber = stoi(input);

        bool strictlyPositiveInput = requiredStrictlyPositiveInput && (inputNumber >0 ); 
        bool positiveInput = (!requiredStrictlyPositiveInput) && (inputNumber >=0 );

        if(strictlyPositiveInput || positiveInput){
            returnVariable = inputNumber;
            return true;

        }else{
            throw std::runtime_error("unvalid bounds");     
        }  

    }catch(...){
        std::cout<< failureMessage << "\n";
        return false;
    }
} 

template<typename T>
bool selectOptionEnumeration(T& returnVariable,string_view prompt,string_view failureMessage,map<string_view,T> options){
    
    string input{};
    cout << prompt << " ";
    cin >> input;

    if (options.find(input) != options.end())
    {   
        returnVariable = options[input];
        return true;
    }
    
    cout << failureMessage << "\n";
    return false;    
}



void RequestSender::constructNewOrderRequest(){
    std::string userInput{};
    
    if(clientID == UNVALID_ID){
        cout <<"please login first before putting an order" << std::endl;
        return ;
    }   
    OrderType orderType{ }; 
    auto inputSuccess = selectOptionEnumeration(
        orderType,
        "Enter order type( limit or market ):",
        "Please Enter a valid order type",
        {
            {"limit",OrderType::LIMIT},
            {"market",OrderType::MARKET},
        }
    );

    if(!inputSuccess) return;


    OrderSide orderSide{ }; 
    inputSuccess = selectOptionEnumeration(
        orderSide,
        "Enter order side (buy or sell):",
        "not found please provide a valid order side ",
        {
            {"sell",OrderSide::SELL},
            {"buy",OrderSide::BUY},
        }
    );
    if(!inputSuccess) return;

    //order price
    int dollars{};
    int cents{};
    if(orderType == OrderType::LIMIT){
    
        inputSuccess = inputInteger(dollars,"Enter the desired price in dollars:","Please Enter a valid Price");
        if(!inputSuccess) return;
        
        inputSuccess = inputInteger(cents,"Enter the desired price in cents:","Please Enter a valid Price");
        if(!inputSuccess) return;


        if(dollars == 0 && cents==0){
            cout << "Please enter a valid price";
            return;
        }
    }

    int quantity{ };
    inputSuccess = inputInteger(quantity,"Enter the desired quantity:","Please provide a valid Quantity", true);
    if(!inputSuccess) return;

    request newOrderRequest(ORDERBOOK_NEW_ORDER,ORDERBOOK_NEW_ORDER,
        {},
        {clientID, orderType,orderSide,quantity,dollars,cents}
    );
    vector<uint8_t> serializedData=newOrderRequest.serialize();

    sendChunk(clientSocket,serializedData);


    getOrderResultMessage(clientSocket);
};


void  RequestSender::constructHelpCommand(){
    for(auto command : commandList){
        std::cout << "-" << command << "\n";
    }
};


void RequestSender::constructUnkownRequest(){
    cout << "UNKNOWN COMMAND\n";
};


void RequestSender::constructStocksRequest(){


    if(clientID==UNVALID_ID){
        std::cout << "Login required!\n";
        return ;
    }
    
    request getStocksRequests(COMMUNICATION,STOCKS,{},{clientID});
    vector<uint8_t> serializedData=getStocksRequests.serialize();
    sendChunk(clientSocket,serializedData);


    request serverFeedbackRequest;
    vector<uint8_t> serverFeedbackByteStream;
    receiveChunks(clientSocket,serverFeedbackByteStream);
    serverFeedbackRequest.deserialize(serverFeedbackByteStream);

    int stocks = serverFeedbackRequest.getNumericArgs()[0];

    std::cout << "you have " << stocks <<" stocks at your account ";
};

void RequestSender::constructMessageRequest(){
    char message[MAX_MSG_LENGTH];
    cout << "Please enter your message: ";
    
    cin.ignore();
    cin.getline(message,MAX_MSG_LENGTH-1);  
    cout << "\n";

    request msgRequest(COMMUNICATION,MSG,{message});
    vector<uint8_t> serializedData=msgRequest.serialize();

    sendChunk(clientSocket,serializedData);
};

void RequestSender::constructLoginRequest(){
    char username[MAX_CREDENTIALS_LENGTH];
    char password[MAX_CREDENTIALS_LENGTH];

    cout << "New username(" << MAX_CREDENTIALS_LENGTH << "  characters at most): " ;
    cin >> username;

    cout << "Enter your password" << MAX_CREDENTIALS_LENGTH << " characters at most): " ;
    cin >> password;

    request loginRequest(COMMUNICATION,LOGIN,{username,password},{});
    vector<uint8_t> serializedData=loginRequest.serialize();

    sendChunk(clientSocket,serializedData);

    //return feedback from server
    request serverFeedbackRequest;
    vector<uint8_t> serverFeedbackByteStream;
    receiveChunks(clientSocket,serverFeedbackByteStream);
    serverFeedbackRequest.deserialize(serverFeedbackByteStream);
    
    if(serverFeedbackRequest.getMessageCommand()==LOGIN){
        clientID = serverFeedbackRequest.getNumericArgs()[0];
        cout << clientID;
        return;
    }
    cout << serverFeedbackRequest.getTextArgs()[0];
};

void RequestSender::constructRegisterRequest(){
    char username[MAX_CREDENTIALS_LENGTH];
    char password[MAX_CREDENTIALS_LENGTH];

    cout << "New username(" << MAX_CREDENTIALS_LENGTH << "  characters at most): " ;
    cin >> username;

    cout << "Enter your password" << MAX_CREDENTIALS_LENGTH << " characters at most): " ;
    cin >> password;

    request loginRequest(COMMUNICATION,REGISTER,{username,password},{});
    vector<uint8_t> serializedData=loginRequest.serialize();

    sendChunk(clientSocket,serializedData);

    //return feedback from server
    request serverFeedbackRequest;
    vector<uint8_t> serverFeedbackByteStream;
    receiveChunks(clientSocket,serverFeedbackByteStream);
    serverFeedbackRequest.deserialize(serverFeedbackByteStream);
    cout << serverFeedbackRequest.getTextArgs()[0];
    
}

void RequestSender::constructBalanceRequest(){

    if(clientID==UNVALID_ID){
        std::cout << "Login required!\n";
        return ;
    }
    request getBalanceRequest(COMMUNICATION,BALANCE,{},{clientID});
    vector<uint8_t> serializedData=getBalanceRequest.serialize();
    sendChunk(clientSocket,serializedData);


    request serverFeedbackRequest;
    vector<uint8_t> serverFeedbackByteStream;
    receiveChunks(clientSocket,serverFeedbackByteStream);
    serverFeedbackRequest.deserialize(serverFeedbackByteStream);
    int dollars =  serverFeedbackRequest.getNumericArgs()[0];
    int cents =  serverFeedbackRequest.getNumericArgs()[1];

    cout << "you have " << dollars<< " dollars and " << cents << " cents in your account" << std::endl;
}
//requesthandler for server
RequestHandler::RequestHandler(int clientSocket_,DBCommunication *ptr,std::mutex* mtx_){
    clientSocket =  clientSocket_;
    threadId =clientSocket;
    dbCommunication = ptr;
    mtx= mtx_;
};

void RequestHandler::createNewOrder(request& request){
    int clientID = request.getNumericArgs()[0];
    int type = request.getNumericArgs()[1];
    int  side = request.getNumericArgs()[2];
    int quantity =  request.getNumericArgs()[3];
    int dollars = request.getNumericArgs()[4];
    int cents = request.getNumericArgs()[5];

    
    DBrequest newOrderRequest{threadId,OB_NEW_ORDER,{},{clientID,type,side,quantity,dollars,cents}};
    mtx->lock();
    dbCommunication->addNewRequest(newOrderRequest);
    mtx->unlock();

    
    DBresponse response = dbCommunication->waitResponse(threadId);
    constructInfoRequest(response.textArgs[0].c_str());
};

void RequestHandler::handleInput(){
    vector<uint8_t> serializedRequest;
    while(connectedToClient){
        

        receiveChunks(clientSocket,serializedRequest);
        request requestReceived;
        requestReceived.deserialize(serializedRequest);

        switch(requestReceived.getMessageCommand()){
            case END_STREAM:
                endConnection();
                break;
            case MSG:
                cout << requestReceived.getTextArgs()[0] << std::endl;
                break;
            case REGISTER:
                registerNewUser(requestReceived);
                break;
            case LOGIN:
                loginUser(requestReceived);
                break;
            case BALANCE:
                getBalanceUser(requestReceived);
                break;
            case STOCKS:
                getStocksUser(requestReceived);
            case ORDERBOOK_NEW_ORDER:
                createNewOrder(requestReceived);
                break;
        }
        serializedRequest.clear();
    }
}

void RequestHandler::endConnection(){
    connectedToClient = false;
}

void RequestHandler::registerNewUser(request& registerRequest){
    const char* username = registerRequest.getTextArgs()[0];
    const char* password = registerRequest.getTextArgs()[1];
    //replace with our new structure
    DBrequest newUser{threadId,DB_REGISTER,{username,password},{}};
    std::mutex mtx;
    mtx.lock();
    dbCommunication->addNewRequest(newUser);
    mtx.unlock();
    DBresponse requestResponse = dbCommunication->waitResponse(threadId);
    int code = requestResponse.errorCode;

    if(code==REGISTER_SUCCESS){
        constructInfoRequest("User created successfully!");
    }else{
        constructInfoRequest("Please enter a different username User already exists");

    }
};

void RequestHandler::loginUser(request &loginRequest){
    const char* username = loginRequest.getTextArgs()[0];
    const char* password = loginRequest.getTextArgs()[1];
    //replace with our new structure
    DBrequest loginUser{threadId,DB_LOGIN,{username,password},{}};
    std::mutex mtx;
    mtx.lock();
    dbCommunication->addNewRequest(loginUser);
    mtx.unlock();

    mtx.lock();
    DBresponse requestResponse = dbCommunication->waitResponse(threadId);
    mtx.unlock();

    int code = requestResponse.errorCode;

    if(code!=USER_NOT_FOUND){
        int userId = requestResponse.numericArgs[0];
        constructIdRequest(userId);
    }else{
        constructInfoRequest("User not found: please re-enter password");

    }
}

void RequestHandler::constructInfoRequest(const char* msg){
    request infoRequest(COMMUNICATION,MSG,{msg});
    sendChunk(clientSocket,infoRequest.serialize());
};


void  RequestHandler::constructIdRequest(int id){
    request infoRequest(COMMUNICATION,LOGIN,{},{id});
    sendChunk(clientSocket,infoRequest.serialize());
};


void RequestHandler::constructBalanceRequest(int dollars,int cents){
    request infoRequest(COMMUNICATION,BALANCE,{},{dollars,cents});
    sendChunk(clientSocket,infoRequest.serialize());
};

void RequestHandler::getBalanceUser(request& request){
    int userId  = request.getNumericArgs()[0];
    if(userId==UNVALID_ID){
        constructInfoRequest("Wrong ID");
        return;

    }
    DBrequest getBalance{threadId,DB_RETRIEVE_USER_BALANCE,{},{userId}};
    std::mutex mtx;
    mtx.lock();
    dbCommunication->addNewRequest(getBalance);
    mtx.unlock();

    mtx.lock();
    DBresponse requestResponse = dbCommunication->waitResponse(threadId);
    mtx.unlock();

    int BalanceDollars= requestResponse.numericArgs[0];
    int BalanceCents = requestResponse.numericArgs[1];

    constructBalanceRequest(BalanceDollars,BalanceCents);
};


void RequestHandler::getStocksUser(request& stockCheckup){
    int userId  = stockCheckup.getNumericArgs()[0];
    if(userId==UNVALID_ID){
        constructInfoRequest("Wrong ID");
        return;

    }
    DBrequest getStockHolding{threadId,DB_RETRIEVE_USER_STOCKS,{},{userId}};
    std::mutex mtx;
    mtx.lock();
    dbCommunication->addNewRequest(getStockHolding);
    mtx.unlock();

    mtx.lock();
    DBresponse requestResponse = dbCommunication->waitResponse(threadId);
    mtx.unlock();

    const int stockAmount = requestResponse.numericArgs[0];
    
    constexpr int empty_field = 0;
    constructBalanceRequest(stockAmount,empty_field);
};
