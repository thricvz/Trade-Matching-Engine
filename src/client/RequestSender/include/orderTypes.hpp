#pragma once
#include <cstdint>
enum class OrderType:uint8_t{
    MARKET,
    LIMIT
};
enum class OrderSide:uint8_t{
    BUY,
    SELL
};
enum class OrderFillState:uint8_t{
    PARTIAL,FULL,NOFILL
};
