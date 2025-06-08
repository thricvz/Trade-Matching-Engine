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
request::request(uint8_t messageType_,uint8_t messageCommand_,vector<char*> textargs_,vector<int32_t> numericargs_){
    messageType = messageType_;
    messageCommand = messageCommand_;
    textArgs = textargs_;
    numericArgs = numericargs_;
};

vector<uint8_t> request::serialize(){
    vector<uint8_t> encoded_data;
    encoded_data.push_back(0); //placeholder for future size element
    encoded_data.push_back(messageType);
    encoded_data.push_back(messageCommand);

    //now serialize the arguments
    serialize_args(encoded_data,textArgs);
    serialize_args(encoded_data,numericArgs);
    encoded_data[0] = encoded_data.size();
    return encoded_data;
};

void request::serialize_args(vector<uint8_t> &stream,std::vector<char*> args_){
    stream.push_back(LIST);
    stream.push_back(STRING);
    stream.push_back(args_.size());

    for(int i =0;i<args_.size();i++){
        char *currentString = args_[i];
        int indexStringSize = stream.size();
        uint8_t stringSize = 0;

        while(*currentString){
            stream.push_back(*currentString);
            currentString++;
            stringSize++;
        };
        stream.push_back('\0');
        stream.insert(stream.begin()+indexStringSize,stringSize+1);
    }
};

void request::serialize_args(vector<uint8_t> &stream,std::vector<int32_t> args_){
    stream.push_back(LIST);
    //List information: type of data and total size
    stream.push_back(INTEGER);
    stream.push_back(args_.size());
    
    for(int i =0;i<args_.size();i++){
        std::uint32_t currentInteger = args_[i];

        for(int offset=3;offset>=0;offset--){
            //extract current byte
            std::uint16_t currentByte = currentInteger & (BYTE_EXTRACT<<(offset*8));
            stream.push_back(currentByte);
        }
    }
};

void request::deserialize(vector<uint8_t> DataBytes){
    //check that no data is missing
    if(DataBytes[0]!=DataBytes.size())
        return;

    this->messageType = DataBytes[1];
    this->messageCommand = DataBytes[2];

    //read list data from the stream of data
    int currentByteIndex = 3;
    while(currentByteIndex < DataBytes.size()){
        //jump list indicator
        uint8_t listType = DataBytes[++currentByteIndex];
        uint8_t listSize = DataBytes[++currentByteIndex];
        //if list is empty just jump to the other list
        if(listSize==0){
            currentByteIndex++;
            continue;
        }
        //for supporting multiple types later
        currentByteIndex++;
        switch(listType){
            case INTEGER:
                deserializeIntegerList(DataBytes,listSize,currentByteIndex);
                break;
            
            case STRING:
                deserializeStringList(DataBytes,listSize,currentByteIndex);
                break;
        }
    }
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


void request::deserializeIntegerList(vector<uint8_t> &data,int listSize,int &listIndex){
    while(listSize>0){
        //construct the 32 bit integer
        int32_t argument = data[listIndex];
        for(int bytesLeft = 3;bytesLeft >0;bytesLeft--){
            listIndex++;
            argument <<=8;
            argument |= data[listIndex];
        }
        numericArgs.push_back(argument);
        //prepare for the next integer
        listIndex++;
        listSize--;
    }
};
void request::deserializeStringList(vector<uint8_t> &data,int listSize,int &listIndex){
    while(listSize>0){
        int stringSize = data[listIndex];
        char* currentString = new char[stringSize];

        for(int i=0;i<stringSize;i++){
            listIndex++;
            *(currentString+i) = data[listIndex];
        }
        textArgs.push_back(currentString);
        listIndex++;
        listSize--;
    }
};
