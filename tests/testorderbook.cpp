#include<gtest\gtest.h>
#include<C:\Users\ericv\Documents\MATCHING_ENGINE\src\orderbook.cpp>

TEST(PriceLevel,add_method){
    priceLevel priceLevel50; 
    order A(50,200,NULL,NULL,NULL);
    order B(50,50,NULL,NULL,NULL);
    order C(50,75,NULL,NULL,NULL);
    order D(50,2,NULL,NULL,NULL);
    priceLevel50.add(A); 
    priceLevel50.add(B); 
    priceLevel50.add(C); 
    priceLevel50.add(D);
    std::vector<order> expected= {A,C,B,D};
    for(int i =0;i<4;i++){
        EXPECT_EQ(priceLevel50.getOrders()[i],expected[i]);
    } 
}

TEST(PriceLevel,matchOrder_method){
    EXPECT_EQ(0,0);
}

TEST(PriceLevel,remove_method){
    EXPECT_EQ(0,0);
}


TEST(OrderBook,match_method){
    EXPECT_EQ(0,0);
}



int main(){
    return RUN_ALL_TESTS();
}