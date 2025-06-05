#include "requestClass.h"
#include "byteCodes.h"
#include <cstring>

using std::vector;
const int32_t BYTE_EXTRACT = 0b11111111;


request::request(){
    messageType = 0;
    messageCommand = 0;
    textArgs = vector<char*>(0);
    numericArgs = vector<int32_t>(0);
};
request::request(uint8_t messageType_,uint8_t messageCommand_,vector<int32_t> args_){
    messageType = messageType_;
    messageCommand = messageCommand_;
    textArgs = vector<char*>(0);
    numericArgs = args_;
};
request::request(uint8_t messageType_,uint8_t messageCommand_,vector<char*> args_){
    messageType = messageType_;
    messageCommand = messageCommand_;
    textArgs = args_;
    numericArgs = vector<int32_t>(0);
};

vector<uint8_t> request::serialize(){
    vector<uint8_t> encoded_data;
    encoded_data.push_back(0); //placeholder for future size element
    encoded_data.push_back(messageType);
    encoded_data.push_back(messageCommand);

    //now serialize the arguments
    serialize_args(encoded_data,textArgs);
    serialize_args(encoded_data,numericArgs);
    encoded_data.push_back(END_STREAM);
    encoded_data[0] = encoded_data.size();
    return encoded_data;
};

void request::serialize_args(vector<uint8_t> &stream,std::vector<char*> args_){
    stream.push_back(LIST);
    vector<uint8_t>::iterator indexListSize = stream.end();
    for(int i =0;i<args_.size();i++){
        char *currentString = args_[i];
        vector<uint8_t>::iterator indexStringSize = stream.end();
        uint8_t stringSize = 0;

        stream.push_back(STRING);
        while(*currentString){
            stream.push_back(*currentString);
            currentString++;
            stringSize++;
        };
        stream.insert(indexStringSize,stringSize);
    }
    stream.insert(indexListSize,stream.size());
};

void request::serialize_args(vector<uint8_t> &stream,std::vector<int32_t> args_){
    stream.push_back(LIST);
    vector<uint8_t>::iterator indexListSize = stream.end();
    for(int i =0;i<args_.size();i++){
        std::uint32_t currentInteger = args_[i];

        stream.push_back(INTEGER);
        for(int offset=3;offset>=0;offset--){
            //extract current byte
            std::uint16_t currentByte = currentInteger & (BYTE_EXTRACT<<(offset*8));
            stream.push_back(currentByte);
        }
    }
    stream.insert(indexListSize,stream.size());
};

void request::deserialize(vector<uint8_t>){

};

bool request::operator==(request const &lhs){
    if(this->messageCommand != lhs.messageCommand)
        return false;
    if(this->messageType != lhs.messageType)
        return false;
    
    if(this->textArgs.size()!= lhs.textArgs.size())
        return false;

    if(this->numericArgs.size()!= lhs.numericArgs.size())
        return false;
        
    //check for test Args list equality
    for(int i=0;i<textArgs.size();i++){
        if(strcmp(textArgs[i],lhs.textArgs[i])!=0)
            return false;
    }

    for(int i=0;i<numericArgs.size();i++){
        if(numericArgs[i]!=lhs.numericArgs[i])
            return false;
    }
    return true;
};
