#pragma once
#include <map>
#include <stack>
#include <list>
#include <vector>
#include <string>
#include<optional>
using std::map;
using std::stack;
using std::list;
using std::string;
using std::vector;

struct DBresponse{
    vector<string> textArgs{};
    vector<int> numericArgs{};
    int errorCode{};
};


struct DBrequest{
    int threadId{};
    int requestType{};
    vector<string> textArgs{};
    vector<int> numericArgs{};
    
};
class DBCommunication{
    map<int,stack<DBresponse>> perThreadResponse{};
    list<DBrequest> requestsThreads{};
    public:
        DBCommunication()=default;
        void addNewRequest(DBrequest operationRequest);
        DBresponse waitResponse(int threadId);

        void addResponse(int threadId,DBresponse operationResult);
        std::optional<DBrequest> getRequest();
};