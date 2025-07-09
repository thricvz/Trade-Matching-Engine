#include "DataBaseCommunication.hpp"


void DBCommunication::addNewRequest(DBrequest operationRequest){
    requestsThreads.push_back(operationRequest);
};
DBresponse DBCommunication::waitResponse(int threadId){
    while(true){
        //wait for the response to be made
        auto response = perThreadResponse.find(threadId); 
        if(response!=perThreadResponse.end() &&  (!response->second.empty()))
            break;
    
    };
    DBresponse response =perThreadResponse[threadId].top();
    perThreadResponse[threadId].pop();
    return response;
};

void DBCommunication::addResponse(int threadId,DBresponse operationResult){
    if(perThreadResponse.find(threadId)==perThreadResponse.end()){
        perThreadResponse.insert({threadId,{}});
        return addResponse(threadId,operationResult);
    }
    perThreadResponse[threadId].push(operationResult);
    return;
};

std::optional<DBrequest> DBCommunication::getRequest(){
    if(requestsThreads.size()==0){
        return std::optional<DBrequest>(std::nullopt);
    }
    DBrequest request = requestsThreads.front();
    requestsThreads.pop_front();
    return std::optional<DBrequest>(request);
}