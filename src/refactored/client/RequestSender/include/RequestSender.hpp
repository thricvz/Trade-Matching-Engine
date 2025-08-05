#include <map>
#include <string>
#include <functional>
#include "ChunkTransmission.hpp" //file for the chunk transmision
#include "requestCodes.hpp" //placeholder for the requestCodes file

const int UNVALID_ID =-1;

class RequestSender{

    public: 
        RequestSender(int clientSocket_);
        
        //methods for constructing the different types of requests
        void handleInput();   
    private:
        bool exitDemanded=false;
        int clientSocket{};
        int clientID{UNVALID_ID};


        void constructExitRequest();
        void constructUnkownRequest();
        void constructMessageRequest();
        void constructLoginRequest();
        void constructRegisterRequest();
        void constructBalanceRequest();
        void constructNewOrderRequest();
        void constructStocksRequest();
        void constructHelpCommand();

        using requestFunction = void (RequestSender::*)();

        static const inline std::map<std::string_view,requestFunction> requestOptions{
            {"exit",&RequestSender::constructExitRequest},
            {"message",&RequestSender::constructMessageRequest},
            {"login",&RequestSender::constructLoginRequest},
            {"register",&RequestSender::constructRegisterRequest},
            {"stocks",&RequestSender::constructStocksRequest},
            {"balance",&RequestSender::constructBalanceRequest},
            {"order",&RequestSender::constructStocksRequest},
            {"help",&RequestSender::constructHelpCommand}
        };
};
