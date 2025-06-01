#pragma once
#include <cstdint>


//commands
const int8_t SELL = 0x77;
//data types
const int8_t INTEGER = 0x88;
const int8_t STRING = 0x89;
const int8_t LIST =  0x90;
//delimiter 
const int8_t END_STREAM = 0xff;
///this is how it works
/*
first value is the size in bytes of the whole message to make sure it is intact
then it follows the following format

single/multi value
type

first three bytes contain msg size
msg type
and msg command


*/ 

