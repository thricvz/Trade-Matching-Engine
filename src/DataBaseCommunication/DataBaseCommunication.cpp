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

DBrequest DBCommunication::getRequest(){
    while(requestsThreads.size()==0){
        //wait
    }
    DBrequest request = requestsThreads.front();
    requestsThreads.pop_front();
    return request;
}