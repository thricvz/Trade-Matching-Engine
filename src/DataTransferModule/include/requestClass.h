#pragma once

#include <cstdint>
#include <vector>


using std::int8_t;
using std::int32_t;
using std::vector;


class request{
    int8_t messageType;
    int8_t messageCommand;
    vector<int32_t> numericArgs;
    vector<char*> textArgs;

    
    void serialize_args(vector<int8_t> &stream,std::vector<char*> args_);
    void serialize_args(vector<int8_t> &stream,std::vector<int32_t> args_);

public:
    request();
    request(int8_t messageType_,int8_t messageCommand_,vector<int32_t> args_);
    request(int8_t messageType_,int8_t messageCommand_,vector<char*> args_);

    vector<int8_t> serialize();
};
//serialize method first bit should contain the size in bits of the whole message
