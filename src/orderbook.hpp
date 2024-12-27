#include<vector>

class order{
    private:
        float price;
        int volume;
        int type;
        int UserId;
        int id;
    public:
        order(int price_,int volume_,int type_,int userId_);
        float getPrice(); 
        int getVolume();
        int getType();
        int getUserId();
        int getId();
};

class priceLevel{
    private:
        std::vector<order> orders;
        float price;
        int volume;
    
    public:
        //insert the order in its correct position (the order list is sorted)
        void add(order ordo);

        //returns the price of the current price level 
        float getPrice(); 
        //returns the complete volume of the current price level 
        int getVolume(); 
        //returns the list of the orders in this price level
        std::vector<order> getOrders();
        //returns a list of orders matched with the order given as a parameter
        std::vector<order> matchOrder(order order_);
        //deletes and order from the price level
        void remove(int userId,int orderId);
};

class orderBook{
    private:
        std::vector<priceLevel> askOrders;
        std::vector<priceLevel> bidOrders;
    public:
        //Matches the order using the pro rata algorithm and returns the 
        // price and quantity matched to the order(to be changed) 
        void match(order order_);
};