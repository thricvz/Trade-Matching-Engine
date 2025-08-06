#include "requestClass.hpp"
#include <gtest/gtest.h>

TEST(Serialization, IntegerArguments){
    
    RequestCommand msgCommand = RequestCommand::MSG;
    std::vector<int32_t> args = {32,45,65};

    request requestToSerialize(msgCommand,{},args);
    request unchangedRequest(msgCommand,{},args);

    std::vector<uint8_t> byteStream = requestToSerialize.serialize();
    request requestRestore;
    requestRestore.deserialize(byteStream);

    EXPECT_TRUE(requestRestore==unchangedRequest);

}


TEST(Serialization, TextArguments){
    
    RequestCommand msgCommand = RequestCommand::BALANCE;

    char w1[] = "hahaha";
    char w2[] = "shein";
    char w3[] = "worldwide";
    std::vector<const char*> args = {w1,w2,w3};

    request requestToSerialize(msgCommand,args,{});
    request unchangedRequest(msgCommand,args,{});

    std::vector<uint8_t> byteStream = requestToSerialize.serialize();
    request requestRestore;
    requestRestore.deserialize(byteStream);

    EXPECT_TRUE(requestRestore==unchangedRequest);

}


TEST(Serialization, FullRequest){
    
    RequestCommand msgCommand = RequestCommand::BALANCE;
    char w1[] = "sve";
    char w2[] = "the ";
    char w3[] = "woworrldwide";
    char w4[] = "ffldl";

    std::vector<const char*> args = {w1,w2,w3};
    std::vector<int32_t> numericArgs = {32,45,65};

    request requestToSerialize(msgCommand,args,numericArgs);
    request unchangedRequest(msgCommand,args,numericArgs);

    std::vector<uint8_t> byteStream = requestToSerialize.serialize();
    request requestRestore;
    requestRestore.deserialize(byteStream);

    EXPECT_TRUE(requestRestore==unchangedRequest);

}


TEST(Serialization2, BIGINTEGERS){
    
    RequestCommand msgCommand = RequestCommand::BALANCE;

    char w1[] = "sve";
    char w2[] = "the ";
    char w3[] = "absolute batman is very good";
    char w4[] = "ffldl";

    std::vector<const char*> args = {w1,w2,w3};
    std::vector<int32_t> numericArgs = {32,500,65};

    request requestToSerialize(msgCommand,args,numericArgs);
    request unchangedRequest(msgCommand,args,numericArgs);

    std::vector<uint8_t> byteStream = requestToSerialize.serialize();
    request requestRestore;
    requestRestore.deserialize(byteStream);

    EXPECT_TRUE(requestRestore==unchangedRequest);

}

TEST(Serialization, BIGINTEGERS2){
    
    RequestCommand msgCommand = RequestCommand::BALANCE;

    char w1[] = "sve";
    char w2[] = "some randoom thing ";
    char w3[] = "fifia";
    char w4[] = "i hope this works i guess my varlehi";

    std::vector<const char*> args = {w1,w2,w3};
    std::vector<int32_t> numericArgs = {13000,0,65};


    request requestToSerialize(msgCommand,args,numericArgs);
    request unchangedRequest(msgCommand,args,numericArgs);

    std::vector<uint8_t> byteStream = requestToSerialize.serialize();
    request requestRestore;
    requestRestore.deserialize(byteStream);

    EXPECT_TRUE(requestRestore==unchangedRequest);

}
