#pragma once

#include <cstdint>
#include <vector>

using std::vector;


class request{
    uint8_t messageType;
    uint8_t messageCommand;
    vector<int32_t> numericArgs;
    vector<const char*> textArgs;

    //set of serialization helper functions
    void serialize_args(vector<uint8_t> &stream,std::vector<const char*> args_);
    void serialize_args(vector<uint8_t> &stream,std::vector<int32_t> args_);

    //set of deserialization helper functions
    void deserializeIntegerList(vector<uint8_t> &Data,int listSize,int &listIndex);
    void deserializeStringList(vector<uint8_t> &Data,int listSize,int &listIndex);

    public:
        request();
        request(uint8_t messageType_,uint8_t messageCommand_,vector<int32_t> args_);
        request(uint8_t messageType_,uint8_t messageCommand_,vector<const char*> args_);
        request(uint8_t messageType_,uint8_t messageCommand_,vector<const char*> textargs_,vector<int32_t> numericargs_);

        vector<uint8_t> serialize();
        void deserialize(vector<uint8_t>);
        bool operator==(request const &lhs);

        //getters
        vector<const char*> getTextArgs();
        vector<int32_t> getNumericArgs();
        uint8_t getMessageCommand();
        uint8_t getMessageType();

};
//serialize method first bit should contain the size in bits of the whole mes
