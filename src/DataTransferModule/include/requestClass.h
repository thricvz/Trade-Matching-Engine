#pragma once

#include <cstdint>
#include <vector>

using std::vector;


class request{
    uint8_t messageType;
    uint8_t messageCommand;
    vector<int32_t> numericArgs;
    vector<char*> textArgs;

    //set of serialization helper functions
    void serialize_args(vector<uint8_t> &stream,std::vector<char*> args_);
    void serialize_args(vector<uint8_t> &stream,std::vector<int32_t> args_);

    //set of deserialization helper functions
    void deserializeIntegerList(vector<uint8_t> &Data,int listSize,int &listIndex);
    void deserializeStringList(vector<uint8_t> &Data,int listSize,int &listIndex);

    public:
    request();
    request(uint8_t messageType_,uint8_t messageCommand_,vector<int32_t> args_);
    request(uint8_t messageType_,uint8_t messageCommand_,vector<char*> args_);
    request(uint8_t messageType_,uint8_t messageCommand_,vector<char*> textargs_,vector<int32_t> numericargs_);

    vector<uint8_t> serialize();
    void deserialize(vector<uint8_t>);
    bool operator==(request const &lhs);
};
//serialize method first bit should contain the size in bits of the whole mes
