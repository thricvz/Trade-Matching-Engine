#include "requestClass.hpp"
#include <sys/socket.h>
#include <cstring>
#include <string.h>
#include <stdexcept>

using std::vector;
const int32_t BYTE_EXTRACT = 0b11111111;

enum RequestDataType :uint8_t{
    INTEGER,
    STRING,
    LIST
};
enum RequestDataOffSet : int{
    packetSize = 0,
    command = 1
};


request::request(RequestCommand command,vector<const char*> textArgs,vector<int32_t> numericArgs){
    this->command = command;
    numericData = numericArgs;
    
    
    for(auto string:textArgs){
        int stringLength = strlen(string);
        char * stringCopy=  new char[stringLength];
        strcpy(stringCopy,string);

        textData.push_back(stringCopy);
    }

};


request::~request(){
    for(auto allocatedString : textData){
        delete[] allocatedString;
    }
    textData.clear();
    numericData.clear();
}

vector<uint8_t> request::serialize(){
    vector<uint8_t> encoded_data(2);

    encoded_data[RequestDataOffSet::packetSize] == 0;
    encoded_data[RequestDataOffSet::command] = static_cast<uint8_t>(command);

    serialize_args(encoded_data,textData);
    serialize_args(encoded_data,numericData);

    uint8_t dataStreamSize = encoded_data.size();
    encoded_data[RequestDataOffSet::packetSize] = dataStreamSize;
    
    return encoded_data;
};

vector<const char*> request::getTextData() const {
    return textData;
};
vector<int32_t> request::getNumericData() const{
    return numericData;
};


RequestCommand request::getCommand() const{
    return command;
};

void request::serialize_args(vector<uint8_t> &stream,std::vector<const char*> args_){
    stream.push_back(RequestDataType::LIST);
    stream.push_back(RequestDataType::STRING);
    stream.push_back(args_.size());

    for(auto currentString :args_){
        int offsetStringLengthByte = stream.size();
        uint8_t currentStringLength = 0;

        while(*currentString){
            stream.push_back(*currentString);
            currentString++;
            currentStringLength++;
            
        };
        //add the null terminator
        stream.push_back('\0');
        currentStringLength+=1;
        stream.insert(stream.begin()+offsetStringLengthByte,currentStringLength);
    }
};

void request::serialize_args(vector<uint8_t> &stream,std::vector<int32_t> args_){
    stream.push_back(RequestDataType::LIST);
    stream.push_back(RequestDataType::INTEGER);
    stream.push_back(args_.size());
    
    for(auto currentInteger : args_){
        for(int offset=3;offset>=0;offset--){
            std::uint32_t valueExtracted = currentInteger & (BYTE_EXTRACT<<(offset*8));
            std::uint8_t currentByte =  (valueExtracted>>(offset*8));

            stream.push_back(currentByte);
        }
    }
};

void request::deserialize(vector<uint8_t> DataBytes){
    //Checking Data Integrity
    if(DataBytes.empty()){
        throw std::runtime_error("No Data available to deserialize");
    }

    if(DataBytes[RequestDataOffSet::packetSize]!=DataBytes.size()){
        throw std::length_error("Data lost in Packet");
    }

    this->command = static_cast<RequestCommand>(DataBytes[1]);

    //read list data from the stream of data
    int currentByteIndex = 2;
    while(currentByteIndex < DataBytes[RequestDataOffSet::packetSize]){

        RequestDataType listType = static_cast<RequestDataType>(DataBytes[++currentByteIndex]);
        uint8_t listSize = DataBytes[++currentByteIndex];

        if(listSize==0){
            currentByteIndex++;
            continue;

        }

        currentByteIndex++;
        switch(listType){
            case RequestDataType::INTEGER:
                deserializeIntegerList(DataBytes,listSize,currentByteIndex);
                break;
            
            case RequestDataType::STRING:
                deserializeStringList(DataBytes,listSize,currentByteIndex);
                break;
        }
    }
};

bool request::operator==(request const &lhs) const{
    
    if(command != lhs.command)
        return false;
    
    if((textData.size()!=lhs.textData.size()) || (numericData.size()!=lhs.numericData.size()) )
        return false;
       

    int lhsIndex{0};
    for(auto data : numericData){
        if(data!=lhs.numericData[lhsIndex])
            return false;
        lhsIndex++;
    }


    lhsIndex=0;
    for(auto data : textData){
        if(strcmp(data,lhs.getTextData()[lhsIndex]))
            return false;
        lhsIndex++;
    }

    return true;
};


void request::deserializeIntegerList(vector<uint8_t> &data,int listSize,int &listIndex){
    while(listSize>0){
        int32_t argument = data[listIndex];
        for(int bytesLeft = 3;bytesLeft >0;bytesLeft--){
            listIndex++;
            argument <<=8;
            argument |= data[listIndex];
        }
        numericData.push_back(argument);
        
        //prepare for the next integer
        listIndex++;
        listSize--;
    }
};
void request::deserializeStringList(vector<uint8_t> &data,int listSize,int &listIndex){
    while(listSize>0){
        int stringSize = data[listIndex];
        char* currentString = new char[stringSize]; //space for the

        for(int i=0;i<stringSize;i++){
            listIndex++;
            *(currentString+i) = data[listIndex];
        }
        textData.push_back(currentString);
        listIndex++;
        listSize--;
    }
};
