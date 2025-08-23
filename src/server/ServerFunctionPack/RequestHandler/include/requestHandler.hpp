#include <vector>
#include <mutex>
#include "requestClass.hpp"
#include "InterThreadCommunication.hpp"

using std::vector; 

class RequestHandler{
    public:
    RequestHandler(int clientSocket_,DBCommunication* ptr,std::mutex* mtx);
    void handleInput();
    
    private:
        int clientSocket;
        int threadId;
        bool connectedToClient = true;
        std::mutex * mtx;
        DBCommunication * dbCommunication;
        
        vector<uint8_t> getSerializedRequest();
        
        void endConnection();
        void registerNewUser(request& registerRequest);
        void constructBalanceRequest(int dollars,int cents);
        void loginUser(request& loginRequest);
        void getBalanceUser(request& balanceCheckUp);
        void getStocksUser(request& stockCheckup);
        void createNewOrder(request& request);
        //communication for the client
        void constructInfoRequest(const char* msg);
        void constructIdRequest(int id);


};