#pragma once
//connection code
const int CONNECT_SUCCESS=0x99;
const int CONNECT_FAILED=0x98;

//register codes 
const int REGISTER_SUCCESS = 0x97;
const int USER_DUPLICATE_ERROR = 0x96;

//getters data methods
const int USER_NOT_FOUND = -0x55; //needs to be negative because ids can be positive