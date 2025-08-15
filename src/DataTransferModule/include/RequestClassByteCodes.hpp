#pragma once
#include <cstdint>

const int MAX_MSG_LENGTH = 200;
const int MAX_CREDENTIALS_LENGTH = 20;

enum class RequestCommand:uint8_t{
    ORDERBOOK_NEW_ORDER,
    END_STREAM,
    MSG,
    LOGIN,
    REGISTER,
    BALANCE,
    STOCKS
};
