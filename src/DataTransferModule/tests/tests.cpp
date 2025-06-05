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