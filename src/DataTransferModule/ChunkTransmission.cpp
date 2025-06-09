#include "ChunkTransmission.h"


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


