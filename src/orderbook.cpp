#include "orderbook.hpp"
#include <vector>
order::order(int price_,int volume_,int type_,int userId_,int id_){
    price = price_;
    volume = volume_;
    type = type_;
    userId =  userId_;
    id = id_;
}
float order::getPrice(){
    return price;
}; 
int order::getVolume(){
    return volume;
};
void order::setVolume(int newVolume){
    volume = newVolume;
};
int order::getType(){
    return type;
};
int order::getUserId(){
    return userId;
};
int order::getId(){
    return id;
};
//price level class method implementation

priceLevel::priceLevel(){
    //orders vector empty by default 
    price = NULL;
    volume = NULL;
};

priceLevel::priceLevel(order order_){
    orders.push_back(order_); 
    price = order_.getPrice();
    volume = order_.getVolume();
};

void priceLevel::add(order order_){
    std::vector<order>::iterator it;
    //adding the order volume to the total price level volume 
    volume += order_.getVolume();
    for(it = orders.begin();it!=orders.end();it++){
        if(order_.getVolume() > it->getVolume()){
            orders.insert(it,order_);
            return;
        }
    }
    //if the order contains the smallest volume within the price level
    orders.push_back(order_); 
};

float priceLevel::getPrice(){
    return price;
};

int priceLevel::getVolume(){
    return volume;      
}; 

std::vector<order> priceLevel::getOrders(){
    return orders;
};

std::vector<order> priceLevel::matchOrder(order order_){
   std::vector<order> filledOrders;
   int volumeTotal =  getVolume();
   int volumeAllocated=0;
   //calculate the amount of volume taken by each seating order
   for(std::vector<order>::iterator it = orders.begin();it!=orders.end();it++){
        volumeAllocated = (it->getVolume()/volumeTotal)*order_.getVolume();
        //add the order to filled Orders
        it->setVolume(it->getVolume()-volumeAllocated);      
   }

   //deleting the fully filled orders
    for(std::vector<order>::iterator it = orders.begin();it!=orders.end();it++){
        if(it->getVolume()==0)
            orders.erase(it); //need to update the code
    }
    return filledOrders;
};

void priceLevel::remove(int userId,int orderId){};