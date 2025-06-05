#include "byteCodes.h"
#include "requestClass.h"
#include <gtest/gtest.h>

TEST(Serialization, IntegerArguments){
    
    uint8_t msgCommand = 88;
    uint8_t msgType = 45;
    std::vector<int32_t> args = {32,45,65};

    request requestToSerialize(msgCommand,msgType,args);
    request unchangedRequest(msgCommand,msgType,args);

    std::vector<uint8_t> byteStream = requestToSerialize.serialize();
    request requestRestore;
    requestRestore.deserialize(byteStream);

    EXPECT_TRUE(requestRestore==unchangedRequest);

}


TEST(Serialization, TextArguments){
    
    uint8_t msgCommand = 88;
    uint8_t msgType = 45;

    char w1[] = "hahaha";
    char w2[] = "shein";
    char w3[] = "worldwide";
    std::vector<char*> args = {w1,w2,w3};

    request requestToSerialize(msgCommand,msgType,args);
    request unchangedRequest(msgCommand,msgType,args);

    std::vector<uint8_t> byteStream = requestToSerialize.serialize();
    request requestRestore;
    requestRestore.deserialize(byteStream);

    EXPECT_TRUE(requestRestore==unchangedRequest);

}