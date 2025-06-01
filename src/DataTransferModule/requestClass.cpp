#include "requestClass.h"
#include "byteCodes.h"
using std::int8_t;
using std::int32_t;
using std::vector;


request::request(){
    messageType = 0;
    messageCommand = 0;
    textArgs = vector<char*>(0);
    numericArgs = vector<int32_t>(0);
};
request::request(int8_t messageType_,int8_t messageCommand_,vector<int32_t> args_){
    messageType = messageType_;
    messageCommand = messageCommand_;
    textArgs = vector<char*>(0);
    numericArgs = args_;
};
request::request(int8_t messageType_,int8_t messageCommand_,vector<char*> args_){
    messageType = messageType_;
    messageCommand = messageCommand_;
    textArgs = args_;
    numericArgs = vector<int32_t>(0);
};

vector<int8_t> request::serialize(){
    vector<int8_t> encoded_data;
    encoded_data.push_back(0); //placeholder for future size element
    encoded_data.push_back(messageType);
    encoded_data.push_back(messageCommand);

    //now serialize the arguments
    serialize_args(encoded_data,textArgs);
    serialize_args(encoded_data,numericArgs);
    encoded_data.push_back(END_STREAM);
    encoded_data[0] = encoded_data.size();

};

void request::serialize_args(vector<int8_t> &stream,std::vector<char*> args_){
    stream.push_back(LIST);
    vector<int8_t>::iterator indexListSize = stream.end();
    for(int i =0;i<args_.size();i++){
        char *currentString = args_[i];
        vector<int8_t>::iterator indexStringSize = stream.end();
        int8_t stringSize = 0;

        stream.push_back(STRING);
        while(*currentString){
            stream.push_back(*currentString);
            currentString++;
            stringSize++;
        };
        stream.insert(indexStringSize,stringSize);
    }
};
void request::serialize_args(vector<int8_t> &stream,std::vector<int32_t> args_){

};


