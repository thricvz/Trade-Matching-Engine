#include "ChunkTransmission.h"
#include <iostream>

void loadBuffer(uint8_t* buffer,vector<uint8_t> byteStream,int sectionStart){
    uint8_t loadedDataSize = 0;    
    for(int index = 1;index < BUFFER_SIZE;index++){
        //bounds checking
        if(sectionStart < byteStream.size()){
            buffer[index] = byteStream[sectionStart];
            sectionStart++;
            loadedDataSize++;
        }else{
            buffer[index] = 0;
        }
    }
    //always specify the size of the data transferred
    buffer[0] = loadedDataSize;
}
void sendChunk(int senderFD,vector<uint8_t> byteStream){
    int leftToSend = byteStream.size();
    int sectionStart=0;
    
    uint8_t buffer[BUFFER_SIZE];
    
    loadBuffer(buffer,byteStream,sectionStart);

    while(true){
        //try and send data
        int deliveryCode = send(senderFD,&buffer, sizeof(buffer),0);
        if(deliveryCode!=-1 || deliveryCode!=0){
            leftToSend -= CHUNK_SIZE-1;
            
            if(leftToSend <= 0)
                break;
            
            sectionStart += CHUNK_SIZE-1;
            loadBuffer(buffer,byteStream,sectionStart);
        }
    }
};

bool loadVector(vector<uint8_t> &byteStream,uint8_t *buffer){
    int lastMsgByte = buffer[0];
    for(int byteIndex=1;byteIndex <=lastMsgByte;byteIndex++){
        byteStream.push_back(buffer[byteIndex]);
    }
    return lastMsgByte != (CHUNK_SIZE-1);
}
void receiveChunks(int senderFD,vector<uint8_t> &byteStream){
    uint8_t buffer[BUFFER_SIZE];
    while (true)
    {   
        int receivedBytes = recv(senderFD,&buffer,sizeof(buffer),0) ; 
        if(receivedBytes!=-1 || receivedBytes!=0){
            std::cout<<"new chunk received";
            bool lastBuffer = loadVector(byteStream,buffer);
            if(lastBuffer)
                break;
        }
    };
    
    
}
