#include <gtest\gtest.h>

TEST(Dummytest,zeroequaltozero){
    EXPECT_EQ(0,0);
}
int main(){
    return RUN_ALL_TESTS();
}