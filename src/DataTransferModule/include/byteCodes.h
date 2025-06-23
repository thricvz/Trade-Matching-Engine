#pragma once
#include <cstdint>

//MESSAGE TYPES
const uint8_t ORDERBOOK = 0X99;
const uint8_t COMMUNICATION = 0X98;
//COMMANDS  TYPES
const uint8_t BUY = 0x97;
const uint8_t SELL = 0x96;
const uint8_t END_STREAM = 0x95;
const uint8_t MSG = 0x94;
const uint8_t LOGIN =0X93;

//DATA TYPES
const uint8_t INTEGER = 0x92;
const uint8_t STRING = 0x91;
const uint8_t LIST =  0x90;


const int MAX_MSG_LENGTH = 200;
const int MAX_CREDENTIALS_LENGTH = 20;