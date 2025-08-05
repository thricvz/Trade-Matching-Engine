#include "include/RequestSender.hpp"



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

        auto commandEntered = requestOptions.find(request);
        if(commandEntered!=requestOptions.end()){
            (this->*(commandEntered->second))();
        }else{
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
    for(auto command : requestOptions){
        std::cout << "-" << command.first << "\n";
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