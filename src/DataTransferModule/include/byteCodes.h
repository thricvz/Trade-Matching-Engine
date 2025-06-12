#pragma once
#include <cstdint>

//MESSAGE TYPES
const uint8_t ORDERBOOK = 0X69;

//COMMANDS  TYPES
const uint8_t BUY = 0x68;
const uint8_t SELL = 0x77;
const uint8_t END_STREAM = 0x22;
const uint8_t MSG = 0x23;


//DATA TYPES
const uint8_t INTEGER = 0x88;
const uint8_t STRING = 0x89;
const uint8_t LIST =  0x90;


const int MAX_MSG_LENGTH = 200;

